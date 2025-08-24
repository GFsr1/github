#ifndef __M_SINK_H__
#define __M_SINK_H__
/*
    日志落地模块
    1. 抽象落地基类
    2. 派生子类（根据不同的落地方向进行派生）
    3. 使用工厂模式进行创建与表示的分离
*/

#include <memory>
#include <fstream>
#include <cassert>
#include <sstream>
#include <mysql_connection.h>
#include <mysql_driver.h>
#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>
#include "util.hpp"

namespace mylog {
    class LogSink {
    public:
        using ptr = std::shared_ptr<LogSink>;
        LogSink() {}
        virtual ~LogSink() {}
        virtual void log(const char *data, size_t len) = 0;
    };

    // 落地方向：标准输出
    class StdoutSink : public LogSink {
    public:
        // 将日志消息写入到标准输出
        void log(const char *data, size_t len) {
            std::cout.write(data, len);
        }
    };
    // 落地方向：指定文件
    class FileSink : public LogSink {
    public:
        // 构造时传入文件名，并打开文件，将操作句柄管理起来
        FileSink(const std::string &pathname):_pathname(pathname) {
            // 1. 创建日志文件所在目录
            util::File::createDirectory(util::File::path(pathname));
            // 2. 创建并打开日志文件
            _ofs.open(_pathname, std::ios::binary | std::ios::app);
            assert(_ofs.is_open());
        }
        // 将日志消息写入到指定文件
        void log(const char *data, size_t len) {
            _ofs.write(data, len);
            assert(_ofs.good());
        }
    private:
        std::string _pathname;
        std::ofstream _ofs;
    };
    // 落地方向：滚动文件 （以大小进行滚动）
    class RollBySizeSink : public LogSink {
    public:
        // 构造时传入文件名，并打开文件，将操作句柄管理起来
        RollBySizeSink(const std::string &basename, size_t max_size):
            _basename(basename), _max_fsize(max_size), _cur_fsize(0), _name_count(0) {
            std::string pathname = createNewFile();
            // 1. 创建日志文件所在目录
            util::File::createDirectory(util::File::path(pathname));
            // 2. 创建并打开日志文件
            _ofs.open(pathname, std::ios::binary | std::ios::app);
        }
        // 将日志消息写入到标准输出 -- 写入前判断文件大小，超过了最大大小就要切换文件
        void log(const char *data, size_t len) {
            if (_cur_fsize >= _max_fsize) {
                _ofs.close(); // 关闭原来已经打开的文件
                std::string pathname = createNewFile();
                util::File::createDirectory(util::File::path(pathname));
                _ofs.open(pathname, std::ios::binary | std::ios::app);
                assert(_ofs.is_open());
                _cur_fsize = 0;
            }
            _ofs.write(data, len);
            assert(_ofs.good());
            _cur_fsize += len;
        }
    private:
        // 进行大小判断，超过指定大小则创建新文件
        std::string createNewFile() {
            // 获取系统时间，以时间来构造文件名扩展名
            time_t t = util::Date::now();
            struct tm lt;
            localtime_r(&t, &lt);
            std::stringstream filename;
            filename << _basename;
            filename << lt.tm_year + 1900;
            filename << lt.tm_mon + 1;
            filename << lt.tm_mday;
            filename << lt.tm_hour;
            filename << lt.tm_min;
            filename << lt.tm_sec;
            filename << "-";
            filename << _name_count++;
            filename << ".log";
            return filename.str();
        }
    private:
        // 通过基础文件名 + 扩展文件名（以时间生产）组成一个实际的当前输出文件名
        size_t _name_count;
        std::string _basename; // ./logs/base~       -> ./logs/base-202507101232.log
        std::ofstream _ofs;
        size_t _max_fsize; // 记录文件最大大小，当前文件超过了这个大小就要切换文件
        size_t _cur_fsize; // 记录当前文件已经写入的数据大小
    };

// 落地方向：MySQL数据库
    class MySQLSink : public LogSink {
    public:
        // 构造时传入数据库连接信息，创建数据库连接和日志表
        MySQLSink(const std::string &host, const std::string &user, const std::string &password, 
                  const std::string &database, int port = 3306) 
            : _host(host), _user(user), _password(password), _database(database), _port(port) {
            
            try {
                // 1. 获取MySQL驱动实例
                _driver = sql::mysql::get_mysql_driver_instance();
                
                // 2. 创建数据库连接
                std::stringstream connection_string;
                connection_string << "tcp://" << _host << ":" << _port;
                _connection.reset(_driver->connect(connection_string.str(), _user, _password));
                
                // 3. 选择数据库
                _connection->setSchema(_database);
                
                // 4. 创建日志表（如果不存在）
                createLogTable();
                
                // 5. 准备插入语句
                prepareInsertStatement();
                
            } catch (sql::SQLException &e) {
                std::cerr << "MySQL connection failed: " << e.what() << std::endl;
                std::cerr << "Error code: " << e.getErrorCode() << std::endl;
                std::cerr << "SQL state: " << e.getSQLState() << std::endl;
                assert(false);
            }
        }
        
        ~MySQLSink() {
        }
        
        // 将日志消息写入到MySQL数据库
        void log(const char *data, size_t len) {
            if (!_connection || !_prep_stmt) {
                return;
            }
            
            try {
                // 解析日志数据，提取各个字段
                LogInfo log_info = parseLogData(data, len);
                
                // 绑定参数并执行插入
                _prep_stmt->setString(1, log_info.timestamp);
                _prep_stmt->setString(2, log_info.level);
                _prep_stmt->setString(3, log_info.logger_name);
                _prep_stmt->setString(4, log_info.file);
                _prep_stmt->setInt(5, log_info.line);
                _prep_stmt->setInt64(6, log_info.thread_id);
                _prep_stmt->setString(7, log_info.message);
                _prep_stmt->setString(8, std::string(data, len));
                
                _prep_stmt->executeUpdate();
                
            } catch (sql::SQLException &e) {
                std::cerr << "Failed to insert log: " << e.what() << std::endl;
                std::cerr << "Error code: " << e.getErrorCode() << std::endl;
            }
        }
        
    private:
        struct LogInfo {
            std::string timestamp;
            std::string level;
            std::string logger_name;
            std::string file;
            int line;
            int64_t thread_id;
            std::string message;
        };
        
        void createLogTable() {
            try {
                std::unique_ptr<sql::Statement> stmt(_connection->createStatement());
                
                const std::string sql = R"(
                    CREATE TABLE IF NOT EXISTS logs (
                        id BIGINT AUTO_INCREMENT PRIMARY KEY,
                        timestamp DATETIME NOT NULL,
                        level VARCHAR(10) NOT NULL,
                        logger_name VARCHAR(255) NOT NULL,
                        file VARCHAR(500) NOT NULL,
                        line INT NOT NULL,
                        thread_id BIGINT NOT NULL,
                        message TEXT NOT NULL,
                        raw_log TEXT NOT NULL,
                        created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
                        INDEX idx_logs_timestamp (timestamp),
                        INDEX idx_logs_level (level),
                        INDEX idx_logs_logger_name (logger_name)
                    ) ENGINE=InnoDB DEFAULT CHARSET=utf8mb4
                )";
                
                stmt->execute(sql);
                
            } catch (sql::SQLException &e) {
                std::cerr << "Failed to create table: " << e.what() << std::endl;
                assert(false);
            }
        }
        
        void prepareInsertStatement() {
            try {
                const std::string sql = R"(
                    INSERT INTO logs (timestamp, level, logger_name, file, line, thread_id, message, raw_log)
                    VALUES (?, ?, ?, ?, ?, ?, ?, ?)
                )";
                
                _prep_stmt.reset(_connection->prepareStatement(sql));
                
            } catch (sql::SQLException &e) {
                std::cerr << "Failed to prepare statement: " << e.what() << std::endl;
                assert(false);
            }
        }
        
        LogInfo parseLogData(const char *data, size_t len) {
            LogInfo info;
            std::string log_str(data, len);
            
            // 简单的日志解析，假设格式为: [timestamp][level][logger_name]file:line message
            // 这里实现一个基本的解析器，实际使用中可能需要更复杂的解析逻辑
            
            // 默认值
            info.timestamp = getCurrentTimestamp();
            info.level = "INFO";
            info.logger_name = "unknown";
            info.file = "unknown";
            info.line = 0;
            info.thread_id = mylog::util::Thread::tid();
            info.message = log_str;
            
            // 尝试解析格式化的日志
            size_t pos = 0;
            
            // 解析时间戳 [timestamp]
            if (log_str[pos] == '[') {
                size_t end_pos = log_str.find(']', pos + 1);
                if (end_pos != std::string::npos) {
                    info.timestamp = log_str.substr(pos + 1, end_pos - pos - 1);
                    pos = end_pos + 1;
                }
            }
            
            // 解析日志级别 [level]
            if (pos < log_str.length() && log_str[pos] == '[') {
                size_t end_pos = log_str.find(']', pos + 1);
                if (end_pos != std::string::npos) {
                    info.level = log_str.substr(pos + 1, end_pos - pos - 1);
                    pos = end_pos + 1;
                }
            }
            
            // 解析日志器名称 [logger_name]
            if (pos < log_str.length() && log_str[pos] == '[') {
                size_t end_pos = log_str.find(']', pos + 1);
                if (end_pos != std::string::npos) {
                    info.logger_name = log_str.substr(pos + 1, end_pos - pos - 1);
                    pos = end_pos + 1;
                }
            }
            
            // 解析文件名和行号 file:line
            size_t colon_pos = log_str.find(':', pos);
            size_t space_pos = log_str.find(' ', pos);
            if (colon_pos != std::string::npos && space_pos != std::string::npos && colon_pos < space_pos) {
                info.file = log_str.substr(pos, colon_pos - pos);
                std::string line_str = log_str.substr(colon_pos + 1, space_pos - colon_pos - 1);
                try {
                    info.line = std::stoi(line_str);
                } catch (...) {
                    info.line = 0;
                }
                pos = space_pos + 1;
            }
            
            // 剩余部分作为消息内容
            if (pos < log_str.length()) {
                info.message = log_str.substr(pos);
                // 移除末尾的换行符
                if (!info.message.empty() && info.message.back() == '\n') {
                    info.message.pop_back();
                }
            }
            
            return info;
        }
        
        std::string getCurrentTimestamp() {
            time_t now = time(nullptr);
            struct tm *tm_info = localtime(&now);
            char buffer[20];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
            return std::string(buffer);
        }
        
    private:
        std::string _host;
        std::string _user;
        std::string _password;
        std::string _database;
        int _port;
        
        sql::mysql::MySQL_Driver *_driver;
        std::unique_ptr<sql::Connection> _connection;
        std::unique_ptr<sql::PreparedStatement> _prep_stmt;
    };

    
    class SinkFactory {
    public:
        template<typename SinkType, typename ...Args>
        static LogSink::ptr create(Args &&...args) {
            return std::make_shared<SinkType>(std::forward<Args>(args)...);
        }
    };

}


#endif /* __M_SINK_H__ */
