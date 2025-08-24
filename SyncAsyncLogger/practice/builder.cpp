/*通过苹果笔记本电脑的构造理解建造者模式*/

#include <iostream>
#include <string>
#include <memory>

class Computer {
public:
    Computer() {
    }
    void setBoard(const std::string &board) {
        _board = board;
    }
    void setDisplat(const std::string &display) {
        _display = display;
    }
    void showParamaters() {
        std::string param = "Computer Paramaters:\n";
        param += "\tBoard:" + _board + "\n";
        param += "\tDisplay:" + _display + "\n";
        param += "\tOs:" + _os + "\n";
        std::cout << param << std::endl;
    }
    virtual void setOs() = 0;
protected:
    std::string _board;
    std::string _display;
    std::string _os;
};


class MacBook : public Computer {
public:
    void setOs() override {
        _os = "Mac OS x12";
    }
};

class Builder {
public:
    virtual void buildBoard(const std::string &board) = 0;
    virtual void buildDisplay(const std::string &display) = 0;
    virtual void buildOs() = 0;
    virtual std::shared_ptr<Computer> build() = 0;
};

class MacBookBuilder : public Builder {
public:
    MacBookBuilder(): _computer(new MacBook()) {
    }
    void buildBoard(const std::string &board) {
        _computer->setBoard(board);
    } 
    void buildDisplay(const std::string &display) {
        _computer->setDisplat(display);
    } 
    void buildOs() {
        _computer->setOs();
    }
    std::shared_ptr<Computer> build() {
        return _computer;
    }
private:
    std::shared_ptr<Computer> _computer;
};

class Director {
public:
    Director(Builder *builder): _builder(builder) {
    }
    void conststruct(const std::string &board, const std::string &display) {
        _builder->buildBoard(board);
        _builder->buildDisplay(display);
        _builder->buildOs();
    }
private:
    std::shared_ptr<Builder> _builder;
};

int main() {
    Builder *builder = new MacBookBuilder();
    std::unique_ptr<Director> director(new Director(builder));
    director->conststruct("华硕主板", "三星显示器");
    std::shared_ptr<Computer> computer = builder->build();
    computer->showParamaters();
    return 0;
}