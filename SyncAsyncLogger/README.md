# #C++ 同步&异步日志系统 

- [#C++ 同步\&异步日志系统](#c-同步异步日志系统)
  - [项目概述](#项目概述)
  - [主要特性](#主要特性)
  - [目录结构](#目录结构)
  - [构建与运行](#构建与运行)
    - [1. 环境准备](#1-环境准备)
    - [2. 编译日志库](#2-编译日志库)
    - [3. 编译并运行示例](#3-编译并运行示例)
  - [使用示例](#使用示例)
    - [1. 基本文件日志](#1-基本文件日志)
    - [2. 异步滚动文件日志](#2-异步滚动文件日志)
  - [优化方向](#优化方向)


## 项目概述
这是一个用 C++ 实现的高性能、可配置的同步与异步日志系统。它旨在为应用程序提供灵活、高效的日志记录能力，支持多种日志级别、输出目的地和格式化选项。该系统采用模块化设计，易于扩展，并针对并发场景进行了优化。

## 主要特性
* **同步与异步日志模式**：
  * **同步日志器**：日志消息立即写入目的地，适用于对实时性要求高、日志量不大的景。
  * **异步日志器**：日志消息先缓存，然后由独立的后台线程批量写入目的地，显著提升高并发场景下的性能，避免阻塞主线程。
* **多日志级别支持**：支持 `DEBUG`, `INFO`, `WARN`, `ERROR`, `FATAL` 等多种日志级别，方便开发者根据需求过滤和控制日志输出。
* **灵活的日志格式化**：
    * 支持自定义日志格式模式，通过占位符（如 `%d` 时间、`%p` 级别、`%c` 日志器名称、`%f` 文件名、`%l` 行号、`%m` 消息、`%n` 换行）来控制日志内容的呈现。
    * 支持时间格式化，如 %d{%Y-%m-%d %H:%M:%S}。
* **多种日志输出目的地** (Sink)：
    * **控制台输出** (StdoutSink)：将日志打印到标准输出。
    * **文件输出** (FileSink)：将日志写入指定文件。
    * **滚动文件输出** (RollBySizeSink)：当日志文件达到指定大小时，自动创建新的日志文件，防止单个文件过大。
* **建造者模式配置**：采用建造者模式 (LoggerBuilder) 来构建和配置日志器，简化了用户接口，提高了配置的灵活性和可读性。
* **全局日志器管理**：通过单例模式 (LoggerManager) 实现全局日志器管理，方便在应用程序的任何地方获取和使用已注册的日志器，并支持设置默认的 root 日志器。
* **线程安全**：所有日志操作都经过精心设计，确保在多线程环境下的数据一致性和安全性。
* **轻量级与高性能**：在保证功能丰富的同时，注重性能优化，尤其是在异步模式下，能够处理高吞吐量的日志请求。



## 目录结构
```
repositories/
├── API_DOC.md          # API 文档
├── CHANGELOG.md        # 更新日志
├── README.md           # 项目说明文档
├── bench/              # 性能测试相关代码 (待补充)
├── example/            # 示例代码
│   ├── Makefile        # 示例代码的 Makefile
│   ├── async_sqlite_test.cc # 异步 SQLite 日志测试示例
│   ├── sqlite_test.cc  # 同步 SQLite 日志测试示例
│   └── test.cc         # 基本日志功能测试示例
├── extend/             # 扩展模块 (待补充)
├── logs/               # 核心日志库源代码
│   ├── Makefile        # 日志库的 Makefile
│   ├── format.hpp      # 日志格式化模块
│   ├── level.hpp       # 日志级别定义
│   ├── logger.hpp      # 日志器核心实现 (同步/异步日志器，建造者模式，管理器)
│   ├── looper.hpp      # 异步日志循环器 (缓冲区管理，后台线程)
│   ├── mylog.h         # 日志系统对外接口头文件
│   ├── sink.hpp        # 日志输出目的地 (Sink) 抽象及具体实现 (StdoutSink, FileSink, RollBySizeSink)
│   └── util.hpp        # 工具类 (文件操作，时间，线程ID等)
└── practice/           # 实践代码 (待补充)
```

## 构建与运行
### 1. 环境准备
* C++ 编译器：支持 C++11 或更高标准的编译器（如 GCC, Clang）。
* `make` 工具：用于编译项目。

### 2. 编译日志库
进入 `logs` 目录并执行 `make` 命令：
```bash
cd logs
make
```
这会编译日志库的核心组件。

### 3. 编译并运行示例
进入 `example` 目录，这里包含了一个日志功能的示例程序。您可以编译示例并运行。
```bash
cd example

# 编译示例程序
make 

# 运行示例
./test
```

## 使用示例
### 1. 基本文件日志
```cpp
#include "mylog.h"

int main() {
    // 创建一个名为 "my_app_logger" 的日志器
    // 日志级别为 INFO，格式为 "[%d{%Y-%m-%d %H:%M:%S}][%p][%c]%f:%l %m%n"
    // 输出到文件 "./logs/app.log"
    mylog::GlobalLoggerBuilder builder;
    builder.buildLoggerName("my_app_logger");
    builder.buildLoggerLevel(mylog::LogLevel::value::INFO);
    builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%p][%c]%f:%l %m%n");
    builder.buildSink<mylog::FileSink>("./logs/app.log");
    mylog::Logger::ptr logger = builder.build();

    logger->info(__FILE__, __LINE__, "Application started.");
    logger->debug(__FILE__, __LINE__, "This debug message will not be shown."); // 低于INFO级别，不显示
    logger->error(__FILE__, __LINE__, "An error occurred: %s", "File not found");

    return 0;
}
```

### 2. 异步滚动文件日志
```cpp
#include "mylog.h"
#include <thread>
#include <chrono>

int main() {
    // 创建一个异步日志器，支持文件大小滚动
    mylog::GlobalLoggerBuilder builder;
    builder.buildLoggerName("async_roll_logger");
    builder.buildLoggerLevel(mylog::LogLevel::value::DEBUG);
    builder.buildFormatter("[%d{%Y-%m-%d %H:%M:%S}][%p][%c]%f:%l %m%n");
    // 每 10MB 滚动一次日志文件，日志文件名为 "./logs/async_roll.log"
    builder.buildSink<mylog::RollBySizeSink>("./logs/async_roll.log", 10 * 1024 * 1024);
    builder.buildLoggerType(mylog::LoggerType::LOGGER_ASYNC);
    mylog::Logger::ptr asyncLogger = builder.build();

    for (int i = 0; i < 100000; ++i) {
        asyncLogger->info(__FILE__, __LINE__, "Processing data record %d", i);
    }

    std::this_thread::sleep_for(std::chrono::seconds(2)); // 等待异步日志写入完成

    return 0;
}
```

## 优化方向
