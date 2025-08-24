/*房东要把一个房子通过中介租出去理解代理模式*/

#include <iostream>
#include <string>

class RentHouse {
public:
    virtual void rentHouse() = 0;
};

class Landord : public RentHouse {
public:
    void rentHouse() {
        std::cout << "将房子租出去\n";
    }
};

class Intermediary : public RentHouse {
public:
    void rentHouse() {
        std::cout << "发布招租启示\n";
        std::cout << "带人看房\n";
        _landord.rentHouse();
        std::cout << "负责租后维修\n";
    }
private:
    Landord _landord;
};

int main() {
    Intermediary intermediary;
    intermediary.rentHouse();
    return 0;
}