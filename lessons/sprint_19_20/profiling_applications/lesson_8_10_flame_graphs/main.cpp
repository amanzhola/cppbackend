#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>
#include <numeric>
#include <random>
#include <vector>

namespace {

volatile double global_result = 0.0;

void FastFunction() {
    double result = 0.0;

    for (int i = 1; i <= 500000; ++i) {
        result += i * 0.000001;
    }

    global_result += result;
}

void MediumFunction() {
    double result = 0.0;

    for (int i = 1; i <= 3000000; ++i) {
        result += std::sqrt(static_cast<double>(i));
    }

    global_result += result;
}

void SlowFunction() {
    double result = 0.0;

    for (int i = 1; i <= 12000000; ++i) {
        result += std::sin(static_cast<double>(i)) * std::cos(static_cast<double>(i));
    }

    global_result += result;
}

void MemoryFunction() {
    std::vector<int> numbers(3000000);

    std::iota(numbers.begin(), numbers.end(), 1);

    std::shuffle(
        numbers.begin(),
        numbers.end(),
        std::mt19937{std::random_device{}()}
    );

    long long sum = 0;

    for (int value : numbers) {
        sum += value;
    }

    global_result += static_cast<double>(sum);
}

void ApplicationScenario() {
    FastFunction();
    MediumFunction();
    SlowFunction();
    MemoryFunction();
}

}  // namespace

int main() {
    std::cout << "Program started" << std::endl;

    const auto start = std::chrono::steady_clock::now();

    for (int iteration = 0; iteration < 100; ++iteration) {
        ApplicationScenario();
    }

    const auto finish = std::chrono::steady_clock::now();

    const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
        finish - start
    );

    std::cout << "Program finished" << std::endl;
    std::cout << "Duration: " << duration.count() << " ms" << std::endl;
    std::cout << "Global result: " << global_result << std::endl;

    return 0;
}
