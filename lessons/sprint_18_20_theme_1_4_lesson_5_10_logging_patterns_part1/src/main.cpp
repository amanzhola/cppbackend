#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <string>
#include <string_view>

using namespace std::literals;

class Logger {
    static auto GetTimeStamp() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        return std::put_time(std::gmtime(&time), "%F %T");
    }

public:
    Logger() {
        if (!log_file_) {
            std::cerr << "Cannot open log file: /var/log/sample.log" << std::endl;
        }
    }

    void Log(std::string_view message) {
        if (!log_file_) {
            return;
        }

        log_file_ << GetTimeStamp() << ": "sv << message << std::endl;
    }

private:
    std::ofstream log_file_{"/var/log/sample.log"s};
};

int main() {
    Logger log;

    log.Log("Program started"sv);

    int x = 0;
    int y = 0;

    std::cout << "Enter x and y: ";
    std::cin >> x >> y;

    log.Log("User entered numbers "s + std::to_string(x) + " and "s + std::to_string(y));

    int sum = x + y;

    std::cout << "The sum of x and y is " << sum << std::endl;

    log.Log("The sum "s + std::to_string(sum) + " is computed"s);

    return 0;
}
