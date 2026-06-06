#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

struct SerDog {
    uint32_t id = 0;
    std::string name;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & id;
        ar & name;
    }
};

class Dog {
public:
    Dog() = default;

    Dog(uint32_t id, std::string name)
        : id_(id)
        , name_(std::move(name)) {
    }

    SerDog ToSerDog() const {
        return SerDog{
            id_,
            name_
        };
    }

    static Dog FromSerDog(const SerDog& ser_dog) {
        return Dog{
            ser_dog.id,
            ser_dog.name
        };
    }

    void Print() const {
        std::cout << id_ << ": " << name_ << std::endl;
    }

private:
    uint32_t id_ = 0;
    std::string name_;
};

int main() {
    Dog dog{10, "Buddy"};

    std::stringstream ss;

    {
        boost::archive::text_oarchive oa{ss};

        SerDog ser_dog = dog.ToSerDog();

        oa << ser_dog;
    }

    {
        boost::archive::text_iarchive ia{ss};

        SerDog ser_dog;

        ia >> ser_dog;

        Dog loaded_dog = Dog::FromSerDog(ser_dog);

        loaded_dog.Print();
    }
}

