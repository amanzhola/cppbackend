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

    Logger() {
        std::cout << "Logger constructed" << std::endl;
    }

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    void Log(std::string_view message) {
        log_file_ << GetTimeStamp() << ": "sv << message << std::endl;
    }

private:
    std::ofstream log_file_{"sample.log"s};
};

class Calculator {
public:
    int Add(int lhs, int rhs) {
        Logger::GetInstance().Log(
            "Calculator::Add called with "s
            + std::to_string(lhs)
            + " and "s
            + std::to_string(rhs)
        );

        return lhs + rhs;
    }
};

class Server {
public:
    void Start() {
        Logger::GetInstance().Log("Server started"sv);
    }

    void Stop() {
        Logger::GetInstance().Log("Server stopped"sv);
    }
};

void DoSomeWork() {
    Logger::GetInstance().Log("DoSomeWork started"sv);

    Calculator calculator;
    int result = calculator.Add(10, 20);

    Logger::GetInstance().Log("DoSomeWork result is "s + std::to_string(result));
}

int main() {

    Logger::GetInstance().Log("Program started"sv);

    Server server;
    server.Start();

    DoSomeWork();

    server.Stop();

    Logger::GetInstance().Log("Program finished"sv);

    return 0;
}
