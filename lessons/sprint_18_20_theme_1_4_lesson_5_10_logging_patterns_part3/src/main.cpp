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

    void LogMessage(std::string_view message) {
        log_file_ << GetTimeStamp() << ": "sv << message << std::endl;
    }

    template <typename... Args>
    void Log(const Args&... args) {
        log_file_ << GetTimeStamp() << ": "sv;
        (log_file_ << ... << args);
        log_file_ << std::endl;
    }

private:
    std::ofstream log_file_{"sample.log"s};
};

class Calculator {
public:
    int Add(int lhs, int rhs) {
        Logger::GetInstance().Log(
            "Calculator::Add called with "sv,
            lhs,
            " and "sv,
            rhs
        );

        int result = lhs + rhs;

        Logger::GetInstance().Log(
            "Calculator::Add result is "sv,
            result
        );

        return result;
    }

    int Multiply(int lhs, int rhs) {
        Logger::GetInstance().Log(
            "Calculator::Multiply called with "sv,
            lhs,
            " and "sv,
            rhs
        );

        int result = lhs * rhs;

        Logger::GetInstance().Log(
            "Calculator::Multiply result is "sv,
            result
        );

        return result;
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

    void HandleUserInput(int x, int y) {
        Logger::GetInstance().Log(
            "Server received user input: x = "sv,
            x,
            ", y = "sv,
            y
        );
    }
};

void DemonstrateOldStyle(int x, int y) {
    Logger::GetInstance().LogMessage(
        "Old style: User entered numbers "s
        + std::to_string(x)
        + " and "s
        + std::to_string(y)
    );
}

void DemonstrateNewStyle(int x, int y) {
    Logger::GetInstance().Log(
        "New style: User entered numbers "sv,
        x,
        " and "sv,
        y
    );
}

void DoSomeWork(int x, int y) {
    Logger::GetInstance().Log("DoSomeWork started"sv);

    DemonstrateOldStyle(x, y);
    DemonstrateNewStyle(x, y);

    Calculator calculator;

    int sum = calculator.Add(x, y);
    int product = calculator.Multiply(x, y);

    Logger::GetInstance().Log(
        "Final results: sum = "sv,
        sum,
        ", product = "sv,
        product
    );

    Logger::GetInstance().Log("DoSomeWork finished"sv);
}

int main() {
    Logger::GetInstance().Log("Program started"sv);

    int x = 0;
    int y = 0;

    std::cout << "Enter x and y: ";
    std::cin >> x >> y;

    Server server;
    server.Start();
    server.HandleUserInput(x, y);

    DoSomeWork(x, y);

    server.Stop();

    Logger::GetInstance().Log("Program finished"sv);

    Logger::GetInstance().Log(
    "Different types: int = "sv,
    42,
    ", double = "sv,
    3.14,
    ", bool = "sv,
    true,
    ", text = "sv,
    "hello"
    );

    return 0;
}
