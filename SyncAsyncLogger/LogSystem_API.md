# C++ 同步&异步日志系统 API 文档
本文档详细介绍了 C++ 同步&异步日志系统的主要类、函数和宏，旨在帮助开发者理解和使用该日志库。

## 1. 日志级别(LogLevel)
`LogLevel` 枚举定义了日志消息的严重程度，从低到高依次为：
级别名称|值|描述|
-|-|-
`DEBUG`|0|调试信息，用于开发阶段的详细输出。
`INFO`|1|普通信息，程序运行的正常状态。
`WARN`|2|警告信息，可能存在问题但程序仍可继续运行。
`ERROR`|3|错误信息，程序运行中发生的错误，可能导致部分功能失效。
`FATAL`|4|致命错误，程序无法继续运行，即将崩溃。

**头文件**：`logs/level.hpp`

**示例**：
```cpp
mylog::LogLevel::value level = mylog::LogLevel::INFO;
```

## 2. 日志消息(LogMsg)
`LogMsg` 结构体封装了单条日志消息的所有相关信息，包括日志级别、时间戳、文件名、行号、线程ID、日志器名称以及实际的日志内容。

**头文件**：`logs/message.hpp`

**成员**：
成员名称|类型|描述
-|-|-
`_level`|`LogLevel::value`|日志级别。
`_time`|`time_t`|日志生成的时间戳。
`_line`|`size_t`|日志发生的文件行号。
`_file`|`std::string`|日志发生的文件名。
`_tid`|`uint64_t`|产生日志的线程ID。
`_logger_name`|`std::string`|记录此日志的日志器名称。
`_payload`|`std::string`|实际的日志内容。

**构造函数**：
```cpp
LogMsg(LogLevel::value level, size_t line, const std::string &file, const std::string &logger_name, const char *fmt, ...);
```

## 3. 日志格式化器(Formatter)
`Formatter` 类负责将 `LogMsg` 对象格式化为可读的字符串。它支持多种占位符，允许用户自定义日志输出的样式。

**头文件**：`logs/format.hpp`

**构造函数**：
```cpp
Formatter(const std::string &pattern = DEFAULT_PATTERN);
```

`pattern` 参数是一个格式化字符串，支持以下占位符：
占位符|描述
-|-
`%d`|日期和时间。可使用 `{%Y-%m-%d %H:%M:%S}` 等格式化字符串。
`%t`|线程ID。
`%p`|日志级别。
`%c`|日志器名称。
`%f`|文件名。
`%l`|行号。
`%m`|日志消息内容。
`%n`|换行符。
`%%`|百分号字面量。

**成员函数**：
```cpp
void format(std::stringstream &ss, const LogMsg &msg);
```

将 `LogMsg` 对象格式化后写入 `stringstream`。

## 4. 日志输出目的地(LogSink)
`LogSink` 是所有日志输出目的地的抽象基类，定义了日志消息的实际落地方式。具体的输出方式通过派生类实现。

**头文件**：`logs/sink.hpp`

**成员函数**：
```cpp
virtual void log(const char *data, size_t len) = 0;
```
纯虚函数，由派生类实现具体的日志写入逻辑。

### 4.1 StdoutSink
`StdoutSink` 是 `LogSink` 的派生类，将日志消息输出到标准输出（控制台）。

**头文件**：`logs/sink.hpp`

**示例**：
```cpp
mylog::StdoutSink stdoutSink;
mylog::LogMsg msg(mylog::LogLevel::INFO, __LINE__, __FILE__, "test_logger", "Hello from StdoutSink!");
std::stringstream ss;
mylog::Formatter formatter;
formatter.format(ss, msg);
stdoutSink.log(ss.str().data(), ss.str().size());
```

### 4.2 FileSink
`FileSink` 是 `LogSink` 的派生类，将日志消息输出到指定文件。

**头文件**：`logs/sink.hpp`

**构造函数**：
```cpp
FileSink(const std::string &filepath);
```

`filepath` 参数指定日志文件的路径。

**示例**：
```cpp
mylog::FileSink fileSink("./my_log.txt");
mylog::LogMsg msg(mylog::LogLevel::INFO, __LINE__, __FILE__, "test_logger", "Hello from FileSink!");
std::stringstream ss;
mylog::Formatter formatter;
formatter.format(ss, msg);
fileSink.log(ss.str().data(), ss.str().size());
```

### 4.3 RollBySizeSink
`RollBySizeSin` 是 `LogSink` 的派生类，当日志文件达到指定大小时，自动创建新的日志文件（滚动日志）。

**头文件**：`logs/sink.hpp`

**构造函数**：
```cpp
RollBySizeSink(const std::string &filepath, size_t max_size);
```

**示例**：
```cpp
mylog::RollBySizeSink rollSink("./logs/app.log", 10 * 1024 * 1024);
mylog::LogMsg msg(mylog::LogLevel::INFO, __LINE__, __FILE__, "test_logger", "Hello from RollBySizeSink!");
std::stringstream ss;
mylog::Formatter formatter;
formatter.format(ss, msg);
rollSink.log(ss.str().data(), ss.str().size());
```

## 5. 日志器(Logger)
`Logger` 是日志系统的核心，负责接收日志请求、处理日志消息并将其分发到配置的 `LogSink`。它是一个抽象基类，同步和异步日志器分别通过 `SyncLogger` 和 `AsyncLogger` 实现。

**头文件**：`logs/logger.hpp`

**成员**：
成员名称|类型|描述
-|-|-
`_logger_name`|`std::string`|日志器名称。
`_limit_level`|`std::atomic<LogLevel::value>`|日志器的最低输出级别。低于此级别的日志将被忽略。
`_formatter`|`Formatter::ptr`|日志格式化器。
`_sinks`|`std::vector<LogSink::ptr>`|日志输出目的地列表。

**成员函数**：
* 日志写入接口
* 内部方法

### 5.1  SyncLogger
`SyncLogger` 是 `Logger` 的派生类，实现同步日志写入。日志消息会立即通过配置的 `LogSink` 写入。

**头文件**：`logs/logger.hpp`

**特点**：
* 实时性高，日志立即输出。
* 写入操作可能阻塞调用线程，影响程序性能。

**示例**：
```cpp
mylog::Formatter::ptr formatter(new mylog::Formatter());
mylog::StdoutSink::ptr stdoutSink(new mylog::StdoutSink());
std::vector<mylog::LogSink::ptr> sinks = {stdoutSink};
mylog::SyncLogger syncLogger("sync_logger", mylog::LogLevel::INFO, formatter, sinks);
syncLogger.info(__FILE__, __LINE__, "This is a sync info message.");
```

### 5.2 AsyncLogger
`AsyncLogger` 是 `Logger` 的派生类，实现异步日志写入。日志消息首先被放入内部缓冲区，然后由独立的后台线程负责将缓冲区中的消息批量写入到 `LogSink`。

**头文件**：`logs/logger.hpp`

**特点**：
* 减少调用线程的阻塞，提高程序性能。
* 日志写入存在一定的延迟。
* 支持两种异步模式：`ASYNC_SAVE` （安全模式，确保所有日志写入）和 `ASYNC_UNSAVE` （非安全模式，可能丢失部分日志以追求极致性能）。

**构造函数**：
```cpp
AsyncLogger(const std::string &logger_name, LogLevel::value level, Formatter::ptr &formatter, std::vector<LogSink::ptr> &sinks, AsyncType looper_type);
```

**示例**：
```cpp
mylog::Formatter::ptr formatter(new mylog::Formatter());
mylog::StdoutSink::ptr stdoutSink(new mylog::StdoutSink());
std::vector<mylog::LogSink::ptr> sinks = {stdoutSink};
mylog::AsyncLogger asyncLogger("async_logger", mylog::LogLevel::INFO, formatter, sinks, mylog::AsyncType::ASYNC_SAVE);
asyncLogger.info(__FILE__, __LINE__, "This is an async info message.");
```

## 6. 异步循环器(AsyncLooper) 
`AsyncLooper` 负责 `AsyncLogger` 的异步写入逻辑。它维护一个缓冲区，并在一个单独的线程中周期性地将缓冲区中的日志数据写入到 `LogSink`。

**头文件**：`logs/looper.hpp`

**成员函数**：
```cpp
void push(const char *data, size_t len);
```

将日志数据推入缓冲区。

## 7. 日志管理器(LoggerManager)
`LoggerManager` 是一个单例类，负责管理所有已注册的日志器。它提供了获取、添加和检查日志器（通过日志器名称）的方法。

**头文件**：`logs/logger.hpp`

**成员函数**：
* `void addLogger(Logger::ptr &logger)`: 添加一个日志器到管理器。
* `bool hasLogger(const std::string &name)`: 检查是否存在指定名称的日志器。
* `Logger::ptr getLogger(const std::string &name)`: 根据名称获取日志器实例。
* `Logger::ptr rootLogger()`: 获取默认的根日志器。

## 8. 日志器建造者(LoggerBuilder)
`LoggerBuilder` 抽象类定义了构建日志器的接口，通过链式调用设置日志器的各种属性。它采用建造者模式，简化了日志器的创建过程。

**头文件**：`logs/logger.hpp`

**成员函数**：
* `buildLoggerType(LoggerType type)`: 设置日志器类型 (同步或异步)。
* `buildEnableUnSaveAsync()`: 启用非安全异步模式 (仅对异步日志器有效)。
* `buildLoggerName(const std::string &name)`: 设置日志器名称。
* `buildLoggerLevel(LogLevel::value level)`: 设置日志器的最低输出级别。
* `buildFormatter(const std::string &pattern)`: 设置日志格式化器。
* `template<typename SinkType, typename ...Args> void buildSink(Args && ...args)`: 添加日志输出目的地。
* `virtual Logger::ptr build() = 0`: 纯虚函数，由派生类实现具体的日志器构建逻辑并返回日志器实例。

### 8.1 LocalLoggerBuilder
`LocalLoggerBuilder` 是 `LoggerBuilder` 的派生类，用于构建独立的日志器实例，这些日志器不会自动注册到 `LoggerManager`.

**头文件**：`logs/logger.hpp`

**示例**：
```cpp
mylog::LocalLoggerBuilder builder;
builder.buildLoggerName("my_local_logger");
// ... 其他配置
mylog::Logger::ptr localLogger = builder.build();
```

### 8.2 GlobalLoggerBuilder
`GlobalLoggerBuilder` 是 `LoggerBuilder` 的派生类，用于构建日志器实例，并自动将其注册到全局的 `LoggerManager` 中，方便通过名称获取。

**头文件**：`logs/logger.hpp`

**示例**：
```cpp
mylog::GlobalLoggerBuilder builder;
builder.buildLoggerName("my_global_logger");
// ... 其他配置
mylog::Logger::ptr globalLogger = builder.build(); // 会自动添加到 LoggerManager
```

## 9. 用户接口宏与函数
`mylog.h` 头文件提供了一系列方便的宏和函数，简化了日志系统的使用。

**头文件**：`logs/mylog.h`

### 9.1 获取日志器函数
* `mylog::Logger::ptr getLogger(const std::string &name)`：获取指定名称的日志器。如果日志器不存在，返回空指针。
* `mylog::Logger::ptr rootLogger()`：获取默认的根日志器。

### 9.2 日志器代理宏（通过日志器实例写入）
这些宏用于通过日志器实例写入日志，会自动填充文件名和行号。
* `debug(fmt, ...)`
* `info(fmt, ...)`
* `warn(fmt, ...)`
* `error(fmt, ...)`
* `fatal(fmt, ...)`

**示例**：
```cpp
mylog::Logger::ptr logger = mylog::getLogger("my_logger");
logger->debug("This is a debug message."); // 实际调用 logger->debug(__FILE__, __LINE__, "This is a debug message.");
```

### 9.3 全局日志宏（通过根日志器写入）
这些宏直接通过根日志器写入日志，无需先获取日志器实例，并会自动填充文件名和行号。
* `DEBUG(fmt, ...)`
* `INFO(fmt, ...)`
* `WARN(fmt, ...)`
* `ERROR(fmt, ...)`
* `FATAL(fmt, ...)`

**示例**：
```cpp
mylog::INFO("Global info message."); // 实际调用 mylog::rootLogger()->info(__FILE__, __LINE__, "Global info message.");
```

## 10. 编译和依赖
### 10.1 基本编译
对于不使用数据库功能的基本日志系统：

```bash
g++ -o your_app your_app.cc -std=c++11 -lpthread
```

### 10.2 Makefile 示例
```cpp
CXX = g++
CXXFLAGS = -std=c++11 -g -Wall
LIBS = -lpthread

# 基本版本
basic_app: basic_app.cc
	$(CXX) -o $@ $^ $(CXXFLAGS) $(LIBS)

# 清理
clean:
	rm -f basic_app 

.PHONY: clean
```

## 11. 最佳实践
### 11.1 日志器选择
* **同步日志器**：适用于实时性要求高、日志量不大的的场景。
* **异步日志器**：适用于高并发、大日志量的场景。

### 11.2 输出目的地选择：
* `StdoutSink`: 适用于开发调试和简单应用
* `FileSink`: 适用于需要持久化存储的场景
* `RollBySizeSink`: 适用于长期运行的应用，避免单个日志文件过大

### 11.3 格式化建议
推荐的日志格式模式：
```cpp
// 详细格式（开发环境）
"[%d{%Y-%m-%d %H:%M:%S}][%t][%p][%c]%f:%l %m%n"

// 简洁格式（生产环境）
"[%d{%H:%M:%S}][%p]%m%n"
```

### 11.4 性能优化
* 在高并发场景下优先使用异步日志器
* 合理设置日志级别，避免输出过多调试信息
* 定期清理或归档旧的日志文件

### 11.5 错误处理
* 始终检查日志器的创建是否成功
* 为关键应用配置多个输出目的地作为备份
* 监控日志系统本身的健康状况

通过遵循这些最佳实践，您可以充分发挥日志系统的优势，构建稳定、高效的应用程序。
