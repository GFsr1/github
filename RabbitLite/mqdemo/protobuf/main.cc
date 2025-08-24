#include "contacts.pb.h"

int main() {
    contacts::contact conn;
    conn.set_sn(1000);
    conn.set_name("小张");
    conn.set_score(98.5);

    // 持久化的数据就放在 str 对象中，这时候可以对 str 进行持久化或网络传输
    std::string str = conn.SerializeAsString();

    contacts::contact stu;
    bool ret = stu.ParseFromString(str);
    if (ret == false) {
        std::cout << "反序列化失败！\n";
        return -1;
    }

    std::cout << stu.sn() << std::endl;
    std::cout << stu.name() << std::endl;
    std::cout << stu.score() << std::endl;

    return 0;
}