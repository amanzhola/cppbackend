#include <chrono>
#include <cmath>
#include <iostream>
#include <thread>

double HeavyCalculation()
{
    double result = 0.0;

    for (long long i = 1; i < 1000000000LL; ++i)
    {
        result += std::sin(i) * std::cos(i);
    }

    return result;
}

void Worker()
{
    while (true)
    {
        double value = HeavyCalculation();

        std::cout << value << std::endl;

        std::this_thread::sleep_for(
            std::chrono::milliseconds(500)
        );
    }
}

int main()
{
    Worker();

    return 0;
}
