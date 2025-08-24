#include "../logs/mylog.h"
#include <iostream>
#include <thread>
#include <chrono>

int main() {
    std::cout << "MySQL Sink Test Started..." << std::endl;
    
    try {
        // 创建一个使用MySQL数据库的同步日志器
        // 注意：请根据您的MySQL服务器配置修改连接参数
        mylog::GlobalLoggerBuilder builder;
        builder.buildLoggerName("mysql_logger");
        builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
        builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%p][%c]%f:%l %m%n");
        
        // MySQL连接参数：host, user, password, database, port
        // 使用测试数据库配置
        builder.buildSink<mylog::MySQLSink>("localhost", "testuser", "@A123456789", "test_logs", 3306);
        
        mylog::Logger::ptr mysqlLogger = builder.build();
        
        std::cout << "MySQL Logger created successfully!" << std::endl;
        
        // 测试不同级别的日志
        mysqlLogger->debug(__FILE__, __LINE__, "This is a debug message from MySQL test");
        mysqlLogger->info(__FILE__, __LINE__, "MySQL logging test started successfully");
        mysqlLogger->warn(__FILE__, __LINE__, "This is a warning message: %s", "Test warning");
        mysqlLogger->error(__FILE__, __LINE__, "This is an error message: %d", 404);
        mysqlLogger->fatal(__FILE__, __LINE__, "This is a fatal message: %s", "Critical error");
        
        // 测试批量日志写入
        std::cout << "Writing batch logs..." << std::endl;
        for (int i = 1; i <= 50; ++i) {
            mysqlLogger->info(__FILE__, __LINE__, "Batch log message #%d - Processing item %d", i, i * 10);
            if (i % 10 == 0) {
                mysqlLogger->warn(__FILE__, __LINE__, "Checkpoint reached: %d messages processed", i);
            }
        }
        
        std::cout << "MySQL Sink Test Completed!" << std::endl;
        std::cout << "Please check your MySQL database 'test_logs' table 'logs' for the log entries." << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}

