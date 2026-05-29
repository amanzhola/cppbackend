#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <thread>
#include <vector>

void FastFunction() {
    volatile long long result = 0;

    for (int i = 0; i < 1000000; ++i) {
        result += i;
    }
}

void MediumFunction() {
    volatile double result = 0.0;

    for (int i = 1; i < 5000000; ++i) {
        result += std::sqrt(i);
    }
}

void SlowFunction() {
    volatile double result = 0.0;

    for (int i = 1; i < 15000000; ++i) {
        result += std::sin(i) * std::cos(i);
    }
}

void VerySlowFunction() {
    std::vector<int> numbers(3000000);

    std::iota(numbers.begin(), numbers.end(), 1);

    volatile long long sum = 0;

    for (int number : numbers) {
        sum += number;
    }
}

void ApplicationScenario() {
    FastFunction();

    MediumFunction();

    SlowFunction();

    VerySlowFunction();
}

int main() {
    std::cout << "Program started" << std::endl;

    ApplicationScenario();

    std::cout << "Program finished" << std::endl;

    return 0;
}
