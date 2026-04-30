#include <boost/chrono.hpp>

#include <iostream>

int main() {
    boost::chrono::steady_clock::time_point start = boost::chrono::steady_clock::now();

    volatile long long sum = 0;

    for (long long i = 0; i < 1000000; ++i) {
        sum += i;
    }

    boost::chrono::steady_clock::time_point finish = boost::chrono::steady_clock::now();

    boost::chrono::milliseconds elapsed =
        boost::chrono::duration_cast<boost::chrono::milliseconds>(finish - start);

    std::cout << "Boost.Chrono linked successfully" << std::endl;
    std::cout << "Elapsed milliseconds: " << elapsed.count() << std::endl;

    return 0;
}
