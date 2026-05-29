#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <string_view>
#include <thread>

class LogDuration {
public:
    explicit LogDuration(std::string_view msg = "")
        : message(std::string(msg) + ": ")
        , start(std::chrono::steady_clock::now())
    {
    }

    ~LogDuration() {
        auto finish = std::chrono::steady_clock::now();
        auto duration = finish - start;

        std::ostringstream output;

        output << message
               << std::chrono::duration_cast<std::chrono::milliseconds>(duration).count()
               << " ms"
               << std::endl;

        std::cerr << output.str();
    }

private:
    std::string message;
    std::chrono::steady_clock::time_point start;
};

#ifndef UNIQ_ID
#define UNIQ_ID_IMPL(line_number) local_log_duration_object_##line_number
#define UNIQ_ID(line_number) UNIQ_ID_IMPL(line_number)
#endif

#define LOG_DURATION(message) \
    LogDuration UNIQ_ID(__LINE__){message};

void LoadConfiguration() {
    LOG_DURATION("LoadConfiguration");

    std::this_thread::sleep_for(std::chrono::milliseconds(40));
}

void ConnectToDatabase() {
    LOG_DURATION("ConnectToDatabase");

    std::this_thread::sleep_for(std::chrono::milliseconds(90));
}

void ReadDataFromDatabase() {
    LOG_DURATION("ReadDataFromDatabase");

    std::this_thread::sleep_for(std::chrono::milliseconds(130));
}

void ProcessData() {
    LOG_DURATION("ProcessData");

    std::this_thread::sleep_for(std::chrono::milliseconds(70));
}

void GenerateReport() {
    LOG_DURATION("GenerateReport");

    std::this_thread::sleep_for(std::chrono::milliseconds(55));
}

void ApplicationScenario() {
    LOG_DURATION("ApplicationScenario");

    LoadConfiguration();
    ConnectToDatabase();
    ReadDataFromDatabase();
    ProcessData();
    GenerateReport();
}

int main() {
    LOG_DURATION("main");

    std::cout << "Program started" << std::endl;

    ApplicationScenario();

    std::cout << "Program finished" << std::endl;

    return 0;
}
