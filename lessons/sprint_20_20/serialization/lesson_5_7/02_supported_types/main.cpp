#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/vector.hpp>

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Item {
    uint32_t id = 0;
    std::string title;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & id;
        ar & title;
    }
};

struct Inventory {
    std::vector<Item> items;

    template <class Archive>
    void serialize(Archive& ar, [[maybe_unused]] const unsigned int version) {
        ar & items;
    }
};

int main() {
    Inventory inventory;

    inventory.items.push_back({1, "sword"});
    inventory.items.push_back({2, "shield"});
    inventory.items.push_back({3, "potion"});

    std::stringstream ss;

    {
        boost::archive::text_oarchive oa{ss};

        oa << inventory;
    }

    {
        boost::archive::text_iarchive ia{ss};

        Inventory loaded_inventory;

        ia >> loaded_inventory;

        for (const Item& item : loaded_inventory.items) {
            std::cout << item.id << ": " << item.title << std::endl;
        }
    }
}

