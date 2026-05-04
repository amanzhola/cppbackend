#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

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

    template <typename... Args>
    void Log(const Args&... args) {
        std::lock_guard<std::mutex> lock(mutex_);

        log_file_ << GetTimeStamp() << ": "sv;
        (log_file_ << ... << args);
        log_file_ << std::endl;
    }

private:
    std::ofstream log_file_{"sample.log"s};
    std::mutex mutex_;
};

void LogIndexInThread(int thread_id, int index) {
    Logger::GetInstance().Log(
        "Thread "sv,
        thread_id,
        " index "sv,
        index
    );
}

void RunThreadWork(int thread_id, int iterations) {
    for (int i = 0; i < iterations; ++i) {
        LogIndexInThread(thread_id, i);
    }
}

int main() {
    Logger::GetInstance().Log("Program started"sv);

    constexpr int iterations = 1000;

    std::thread thread1([] {
        RunThreadWork(1, iterations);
    });

    std::thread thread2([] {
        RunThreadWork(2, iterations);
    });

    thread1.join();
    thread2.join();

    Logger::GetInstance().Log("Program finished"sv);

    return 0;
}
