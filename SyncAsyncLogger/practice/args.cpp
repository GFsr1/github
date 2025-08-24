/**C++风格的不定参函数的使用*/

#include <iostream>

void xprintf() {
    std::cout << std::endl;
}

template<typename T, typename ...Args>
void xprintf(const T &v, Args &&...args) {
    std::cout << v;
    if((sizeof ...(args) > 0)) {
        xprintf(std::forward<Args>(args)...);
    } else {
        xprintf();
    }

}


int main() {
    xprintf("zkp");
    xprintf("zkp", "hello");
    xprintf("zkp", "hello", 666);

    return 0;
}