#include <iostream>

// 饿汉模式：以空间换时间
// class Singleton {
// private:
//     // 类内声明
//     static Singleton _eton;
//     Singleton(): _data(99) {
//         std::cout << "单例对象构造！\n";
//     }
//     Singleton(const Singleton&) = delete;
//     ~Singleton() {}
// private:
//     int _data;
// public:
//     static Singleton &getInstance() {
//         return _eton;
//     }
//     int getData() { return _data; }
// };
// 
// 类外定义
// 饿汉模式，直接定义单例对象
// Singleton Singleton::_eton;

// 懒汉模式，懒加载--延迟加载的思想-- 一个对象在用的时候再进行实例化
class Singleton {
private:
    // 类内声明
    Singleton():_data(99) {
        std::cout << "单例对象构造！\n";
    }
    Singleton(const Singleton &) = delete;
    ~Singleton() {}
private:
    int _data;
public:
    static Singleton &getInstance() {
        static Singleton _eton;
        return _eton;
    }
    int getData() { return _data; }
};


int main() {
    std::cout << Singleton::getInstance().getData() << std::endl;
    return 0;
}