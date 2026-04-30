#pragma once

#include <chrono>
#include <fstream>
#include <iomanip>
#include <mutex>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>

using namespace std::literals;

#define LOG(...) Logger::GetInstance().Log(__VA_ARGS__)

class Logger {
    auto GetTime() const {
        if (manual_ts_) {
            return *manual_ts_;
        }

        return std::chrono::system_clock::now();
    }

    auto GetTimeStamp() const {
        const auto now = GetTime();
        const auto time = std::chrono::system_clock::to_time_t(now);

        return std::put_time(std::localtime(&time), "%F %T");
    }

    std::string GetFileTimeStamp() const {
        const auto now = GetTime();
        const auto time = std::chrono::system_clock::to_time_t(now);

        std::ostringstream out;
        out << std::put_time(std::localtime(&time), "%Y_%m_%d");

        return out.str();
    }

    std::string MakeLogFileName() const {
        return "/var/log/sample_log_"s + GetFileTimeStamp() + ".log"s;
    }

    void OpenActualLogFile() {
        const std::string actual_file_name = MakeLogFileName();

        if (actual_file_name == current_file_name_ && log_file_.is_open()) {
            return;
        }

        if (log_file_.is_open()) {
            log_file_.close();
        }

        current_file_name_ = actual_file_name;
        log_file_.open(current_file_name_, std::ios::app);
    }

    Logger() = default;

public:
    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;

    Logger(Logger&&) = delete;
    Logger& operator=(Logger&&) = delete;

    static Logger& GetInstance() {
        static Logger obj;
        return obj;
    }

    template <class... Ts>
    void Log(const Ts&... args) {
        std::lock_guard<std::mutex> lock(mutex_);

        OpenActualLogFile();

        if (!log_file_) {
            return;
        }

        log_file_ << GetTimeStamp() << ": "sv;
        (log_file_ << ... << args);
        log_file_ << std::endl;
    }

    void SetTimestamp(std::chrono::system_clock::time_point ts) {
        std::lock_guard<std::mutex> lock(mutex_);
        manual_ts_ = ts;
    }

private:
    std::optional<std::chrono::system_clock::time_point> manual_ts_;
    std::ofstream log_file_;
    std::string current_file_name_;
    std::mutex mutex_;
};
