#include <iostream>
#include <memory>

class Fruit {
public:
    virtual void name() = 0;
};

class Apple : public Fruit {
public:
    void name() override {
        std::cout << "我是一个苹果\n";
    }
};

class Banana : public Fruit {
public:
    void name() override {
        std::cout << "我是一个香蕉\n";
    }
};

class Animal {
public:
    virtual void name() = 0;
};

class Lamp : public Animal {
public:
    void name() override {
        std::cout << "我是一只山羊！\n";
    }
};

class Dog : public Animal {
public:
    void name() override {
        std::cout << "我是一只土狗！\n";
    }
};

class Factory {
public:
    virtual std::shared_ptr<Fruit> getFruit(const std::string &name) = 0;
    virtual std::shared_ptr<Animal> getAnimal(const std::string &name) = 0;
};

class FruitFactory : public Factory {
public:
    std::shared_ptr<Animal> getAnimal(const std::string &name) {
        return std::shared_ptr<Animal>();
    }
    std::shared_ptr<Fruit> getFruit(const std::string &name) {
        if (name == "苹果") {
            return std::make_shared<Apple>();
        } else {
            return std::make_shared<Banana>();
        }
    }
};

class AnimalFactory : public Factory {
public:
    std::shared_ptr<Fruit> getFruit(const std::string &name) {
        return std::shared_ptr<Fruit>();
    }   
    std::shared_ptr<Animal> getAnimal(const std::string &name) {
        if (name == "小狗") {
            return std::make_shared<Dog>();
        } else {
            return std::make_shared<Lamp>();
        }
    }
};

class FactoryProducer {
public:
    static std::shared_ptr<Factory> create(const std::string &name) {
        if (name == "水果") {
            return std::make_shared<FruitFactory>();
        } else {
            return std::make_shared<AnimalFactory>();
        }
    }
};

// class FruitFactory {
// public:
//     static std::shared_ptr<Fruit> create(const std::string &name) {
//         if (name == "苹果") {
//             return std::make_shared<Apple>();
//         } else {
//             return std::make_shared<Banana>();
//         }
//     }
// };

// class FruitFactory {
// public: 
//     virtual std::shared_ptr<Fruit> create() = 0;
// };

// class AppleFactory : public FruitFactory {
// public:
//     std::shared_ptr<Fruit> create() override {
//         return std::make_shared<Apple>();
//     }
// };

// class BananaFactory : public FruitFactory {
// public:
//     std::shared_ptr<Fruit> create() override {
//         return std::make_shared<Banana>();
//     }
// };



int main() {
    // std::shared_ptr<Fruit> fruit = FruitFactory::create("苹果");
    // fruit->name();
    // fruit = FruitFactory::create("香蕉");
    // fruit->name();

    // std::shared_ptr<FruitFactory> ff(new AppleFactory());
    // std::shared_ptr<Fruit> fruit = ff->create();
    // fruit->name();
    // ff.reset(new BananaFactory());
    // fruit = ff->create();
    // fruit->name();

    std::shared_ptr<Factory> ff = FactoryProducer::create("水果");
    std::shared_ptr<Fruit> fruit = ff->getFruit("苹果");
    fruit->name();
    fruit = ff->getFruit("香蕉");
    fruit->name();

    std::shared_ptr<Factory> factory = FactoryProducer::create("动物");
    std::shared_ptr<Animal> animal = factory->getAnimal("小狗");
    fruit->name();
    animal = factory->getAnimal("山羊");
    if (animal.get() == nullptr) {
        return 0;
    }
    animal->name();

    return 0;
}