#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

struct SerDog {
    uint32_t id = 0;
    uint32_t direction = 0;
    double pos_x = 0.0;
    double pos_y = 0.0;
    double speed_x = 0.0;
    double speed_y = 0.0;
    std::string name;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & id;
        ar & direction;
        ar & pos_x;
        ar & pos_y;
        ar & speed_x;
        ar & speed_y;
        ar & name;
    }
};

int main() {
    std::stringstream ss;

    {
        boost::archive::text_oarchive oa{ss};

        SerDog ser_dog{
            5,
            2,
            0.5,
            1.0,
            0.0,
            0.0,
            "Puppy"
        };

        oa << ser_dog;
    }

    {
        boost::archive::text_iarchive ia{ss};

        SerDog ser_dog;

        ia >> ser_dog;

        std::cout << "id: " << ser_dog.id << std::endl;
        std::cout << "direction: " << ser_dog.direction << std::endl;
        std::cout << "pos_x: " << ser_dog.pos_x << std::endl;
        std::cout << "pos_y: " << ser_dog.pos_y << std::endl;
        std::cout << "speed_x: " << ser_dog.speed_x << std::endl;
        std::cout << "speed_y: " << ser_dog.speed_y << std::endl;
        std::cout << "name: " << ser_dog.name << std::endl;
    }
}
