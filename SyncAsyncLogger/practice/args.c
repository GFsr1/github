/*不定参宏函数*/

#define _GNU_SOURCE
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>

#define LOG(fmt, ...) printf("[%s:%d] " fmt, __FILE__, __LINE__, ##__VA_ARGS__); // ## 表示如果不定参为空，则取消前面的这个逗号


/*C语言中不定参函数的使用，不定参数据的访问*/
void printNum(int count, ...) {
    va_list ap;
    va_start(ap, count); // 获取指定参数的起始地址，这里是获取 count 参数之后的第一个参数的起始地址
    for (int i = 0; i < count; ++i) {
        int num = va_arg(ap, int);
        printf("param[%d]:%d\n", i, num);
    }
    va_end(ap); // 将 ap 指针置空
} 

void myprintf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    char *res;
    int ret = vasprintf(&res, fmt, ap);
    if (ret != -1) {
        printf("%s", res);
        free(res);
    }
    va_end(ap); // 将 ap 指针置空
}

int main() {
    LOG("%s-%d\n", "hello world", 666);
    LOG("hello zkp\n");
    printNum(2, 666, 888);
    printNum(5, 1, 2, 3, 4, 5);
    myprintf("%s-%d\n", "hello zkp", 666);

    return 0;
}