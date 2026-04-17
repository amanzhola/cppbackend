#include <boost/asio.hpp>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <syncstream>
#include <thread>

namespace net = boost::asio;

int main() {
    using osync = std::osyncstream;
    using namespace std::chrono;

    // Создаём пул, содержащий два потока
    net::thread_pool tp{2};
    const auto start = steady_clock::now();

    auto print = [start](char ch) {
        const auto t = duration_cast<duration<double>>(steady_clock::now() - start).count();
        osync(std::cout) << std::fixed << std::setprecision(6)
                         << t << "> " << ch << ':' << std::this_thread::get_id()
                         << std::endl;
    };

    net::post(tp, [&tp, print] {
        print('A');

        // Теперь вместо defer используется post
        net::post(tp, [print] { print('B'); });
        net::post(tp, [print] { print('C'); });
        net::post(tp, [print] { print('D'); });

        // Засыпаем, чтобы дать шанс другим потокам сделать свою работу
        std::this_thread::sleep_for(std::chrono::seconds(1));
    });

    // Дожидаемся окончания работы потоков
    tp.wait();
}
