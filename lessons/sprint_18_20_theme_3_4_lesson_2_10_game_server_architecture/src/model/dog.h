#pragma once

#include <cstdint>
#include <string>
#include <utility>

namespace model {

class Dog {
public:
    using Id = std::uint64_t;

    Dog(Id id, std::string name)
        : id_{id}
        , name_{std::move(name)} {
    }

    Id GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

private:
    Id id_;
    std::string name_;
};

}  // namespace model
