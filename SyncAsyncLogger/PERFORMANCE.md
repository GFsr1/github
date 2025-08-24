# Mylog 日志库性能测试报告
## 1. 测试环境 (Test Environment)
### 1.1. 硬件平台 (Hardware Platform)
* 主机型号 (Host Model): LAPTOP-HDINLGIU
* 处理器 (CPU):
    * 型号: 13th Gen Intel(R) Core(TM) i9-13980HX
    * 核心/线程数: 24 核心 (8 P-cores + 16 E-cores), 32 线程
    * 基础频率: 2.20 GHz
* 物理内存 (Host RAM): 
    * 容量: 32.0 GB
    * 规格: 2 x 16GB Hynix DDR5
* 宿主机磁盘 (Host Disk):
    * 类型: [请在此处补充：例如 NVMe SSD 或 SATA SSD]
    * 型号: [请在此处补充：例如 Samsung 980 Pro 1TB]
### 1.2. 软件与虚拟化环境 (Software & Virtualization Environment)
* 宿主机操作系统 (Host OS):
    * 类型: 64位 Windows 操作系统 [Windows 11 家庭中文版]
* 虚拟化方案 (Virtualization Software):
    * 名称与版本: [VMware Workstation Pro 17.6]
* 客户机操作系统 (Guest OS):
    * 发行版: [Ubuntu 24.04.2 LTS] 
    * Linux 内核: [6.14.0-24-generic] 
* 编译器 (Compiler):
    * 名称与版本: [g++ (Ubuntu 13.3.0-6ubuntu2~24.04) 13.3.0]
* 编译选项 (Build Flags):
  * 选项: [请在此处补充，例如 -std=c++17 -O3 -pthread] (查看您的编译命令或Makefile)
* 被测程序 (Program Under Test):
    * 项目: Mylog C++ Logger
### 1.3. 虚拟机资源配置 (VM Resource Allocation)
* 虚拟处理器 (vCPU): 8 核心
* 虚拟内存 (vRAM): 8.0 GB
## 2. 测试方法 (Benchmark Methodology)
* 测试场景:
    * 场景A (CPU核心饱和测试): 旨在评估日志库在充分利用CPU核心时的性能。
        * 参数: 8线程, 100万条日志, 100字节/条。
    * 场景B (超线程压力测试): 模拟高并发服务器环境，测试上下文切换开销和锁竞争。
        * 参数: 16线程, 100万条日志, 100字节/条。
    * 场景C (海量I/O压力测试): 旨在通过持续、大量的写入操作，测试后台I/O处理极限，并创造让异步队列饱和的条件。
        * 参数: 8线程, 1000万条日志, 100字节/条。
* 性能指标 (Metrics):
    * 主要指标: 业务线程感知耗时 (以所有线程中耗时最长的为准)，单位为秒(s)。
    * 衍生指标: 每秒输出日志数量 (条/s)，每秒输出日志大小 (KB/s)。
* 执行流程 (Execution Procedure):
    * 为消除硬件和操作系统缓存带来的冷启动影响，所有测试场景均在一次“预热”运行后开始正式计时。
    * 每个测试场景的最终结果，是连续执行10次测试后计算出的算术平均值，以保证结果的稳定性和可复现性。
## 3. 测试结果与分析 (Results & Analysis)
### 3.1 场景A: CPU核心饱和测试
### 3.2 场景B: 超线程压力测试
### 3.3 场景C: 海量I/O压力测试
## 4. 结论 (Conclusion)