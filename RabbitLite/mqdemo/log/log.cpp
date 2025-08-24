#include <iostream>
#include <ctime>

// 封装一个日志宏，通过日志宏进行日志的打印，在打印的信息前带有系统时间以及文件名和行号
//      [23:31:25] [log.cpp:12] 打开文件失败

#define DBG_LEVEL 0
#define INF_LEVEL 1
#define ERR_LEVEL 2
#define DEFAULT_LEVEL DBG_LEVEL
#define LOG(lev_str, level, format, ...) do {\
    if (level >= DEFAULT_LEVEL) {\
        time_t t = time(nullptr);\
        struct tm* ptm = localtime(&t);\
        char time_str[32];\
        strftime(time_str, sizeof(time_str)-1, "%H:%M:%S", ptm);\
        printf("[%s][%s][%s:%d]\t" format "\n", \
               lev_str, time_str, __FILE__, __LINE__, ##__VA_ARGS__);\
    }\
} while(0)  // 用 do-while(0) 确保宏在各种场景下语法正确

#define DLOG(format, ...) LOG("DBG", DBG_LEVEL, format, ##__VA_ARGS__)
#define ILOG(format, ...) LOG("INF", INF_LEVEL, format, ##__VA_ARGS__)
#define ELOG(format, ...) LOG("ERR", ERR_LEVEL, format, ##__VA_ARGS__)

int main() 
{
    DLOG("Hello World");
    ILOG("Hello World");
    ELOG("Hello World");
    return 0;
}