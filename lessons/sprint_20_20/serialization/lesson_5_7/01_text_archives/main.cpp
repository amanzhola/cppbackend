#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

struct Box {
    uint32_t id = 0;
    std::string name;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & id;
        ar & name;
    }
};

int main() {
    std::stringstream ss;

    {
        boost::archive::text_oarchive oa{ss};

        const Box box{1, "winter clothes"};

        oa << box;

        std::cout << "Serialized text:" << std::endl;
        std::cout << ss.str() << std::endl;
    }

    {
        boost::archive::text_iarchive ia{ss};

        Box box;

        ia >> box;

        std::cout << "Deserialized object:" << std::endl;
        std::cout << "id = " << box.id << std::endl;
        std::cout << "name = " << box.name << std::endl;
    }
}
