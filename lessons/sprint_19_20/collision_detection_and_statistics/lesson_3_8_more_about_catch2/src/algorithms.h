#pragma once

#include <algorithm>
#include <numeric>
#include <random>
#include <stdexcept>
#include <string>
#include <vector>

template <class T>
T GetMax(const std::vector<T>& elems) {
    using namespace std::literals;

    if (elems.empty()) {
        throw std::invalid_argument("Attempt to get max of empty array"s);
    }

    return *std::max_element(elems.begin(), elems.end());
}

inline std::vector<int> GetRandomVector(std::size_t size) {
    std::vector<int> values(size);

    std::iota(values.begin(), values.end(), 0);

    std::random_device random_device;
    std::mt19937 generator(random_device());

    std::shuffle(values.begin(), values.end(), generator);

    return values;
}

template <class T>
class CountingComparator {
public:
    bool operator()(const T& left, const T& right) const {
        ++counter_;
        return left < right;
    }

    std::size_t GetCounter() const {
        return counter_;
    }

private:
    mutable std::size_t counter_ = 0;
};
