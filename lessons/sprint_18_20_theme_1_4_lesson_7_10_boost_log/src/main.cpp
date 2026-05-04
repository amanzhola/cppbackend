#include <boost/date_time.hpp>

#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/trivial.hpp>

#include <boost/log/utility/manipulators/add_value.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>

#include <chrono>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <mutex>
#include <string>
#include <string_view>
#include <thread>

using namespace std::literals;

namespace logging = boost::log;
namespace keywords = boost::log::keywords;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;

BOOST_LOG_ATTRIBUTE_KEYWORD(line_id, "LineID", unsigned int)
BOOST_LOG_ATTRIBUTE_KEYWORD(timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(file, "File", std::string)
BOOST_LOG_ATTRIBUTE_KEYWORD(line, "Line", int)

#define BOOST_LOG_WITH_LOCATION(level) \
    BOOST_LOG_TRIVIAL(level)           \
        << logging::add_value(file, std::string(__FILE__)) \
        << logging::add_value(line, __LINE__)

#define CUSTOM_LOG(X) \
    CustomLogger::GetInstance().Log([&](std::ostream& out) { out << X; })

void MyFormatter(logging::record_view const& rec, logging::formatting_ostream& strm) {
    strm << rec[line_id] << ": ";

    auto ts = rec[timestamp];
    if (ts) {
        strm << to_iso_extended_string(*ts) << ": ";
    } else {
        strm << "no timestamp: ";
    }

    strm << "<" << rec[logging::trivial::severity] << "> ";

    auto file_value = rec[file];
    auto line_value = rec[line];

    if (file_value && line_value) {
        strm << "[" << *file_value << ":" << *line_value << "] ";
    }

    strm << rec[expr::smessage];
}

void InitBoostLog() {
    logging::add_common_attributes();

    logging::core::get()->set_filter(
        logging::trivial::severity >= logging::trivial::info
    );

    logging::add_file_log(
        keywords::file_name = "logs/boost_sample_%N.log",
        keywords::format = &MyFormatter,
        keywords::open_mode = std::ios_base::app | std::ios_base::out,
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(12, 0, 0),
        keywords::auto_flush = true
    );

    logging::add_console_log(
        std::clog,
        keywords::format = &MyFormatter,
        keywords::auto_flush = true
    );
}

void DemonstrateBasicBoostLog() {
    std::cout << "\n[1] Используем Boost.Log\n"sv;

    BOOST_LOG_TRIVIAL(trace) << "Сообщение уровня trace"sv;
    BOOST_LOG_TRIVIAL(debug) << "Сообщение уровня debug"sv;
    BOOST_LOG_TRIVIAL(info) << "Сообщение уровня info"sv;
    BOOST_LOG_TRIVIAL(warning) << "Сообщение уровня warning"sv;
    BOOST_LOG_TRIVIAL(error) << "Сообщение уровня error"sv;
    BOOST_LOG_TRIVIAL(fatal) << "Сообщение уровня fatal"sv;

    std::cout << "Current thread id: "
              << std::hex
              << std::this_thread::get_id()
              << std::dec
              << std::endl;
}

void DemonstrateEventsAndAttributes() {
    std::cout << "\n[2] События и их параметры\n"sv;

    BOOST_LOG_WITH_LOCATION(info) << "Программа перешла в демонстрацию атрибутов"sv;
    BOOST_LOG_WITH_LOCATION(warning) << "Это предупреждение с файлом и строкой"sv;
    BOOST_LOG_WITH_LOCATION(error) << "Это ошибка с пользовательскими атрибутами"sv;
}

void DemonstrateFilters() {
    std::cout << "\n[3] Фильтры\n"sv;

    int log_num = 1;

    BOOST_LOG_TRIVIAL(trace) << "trace не должен вычислиться, номер "sv << log_num++;
    BOOST_LOG_TRIVIAL(debug) << "debug не должен вычислиться, номер "sv << log_num++;
    BOOST_LOG_TRIVIAL(info) << "info должен вычислиться, номер "sv << log_num++;
    BOOST_LOG_TRIVIAL(warning) << "warning должен вычислиться, номер "sv << log_num++;
    BOOST_LOG_TRIVIAL(error) << "error должен вычислиться, номер "sv << log_num++;
    BOOST_LOG_TRIVIAL(fatal) << "fatal должен вычислиться, номер "sv << log_num++;

    std::cout << "После логирования log_num = " << log_num << std::endl;
    std::cout << "Если фильтр работает, log_num должен быть 5." << std::endl;
}

void DemonstrateFileLogging() {
    std::cout << "\n[4] Логируем в файл\n"sv;

    BOOST_LOG_WITH_LOCATION(info) << "Это сообщение попадёт и в консоль, и в файл"sv;
    BOOST_LOG_WITH_LOCATION(warning) << "Файл не должен обнуляться при новом запуске"sv;
    BOOST_LOG_WITH_LOCATION(error) << "Файл поддерживает ротацию по размеру и времени"sv;
}

void DemonstrateProfessionalFormatting() {
    std::cout << "\n[5] Форматируем профессионально\n"sv;

    BOOST_LOG_WITH_LOCATION(info) << "Форматтер выводит LineID, TimeStamp, severity, File, Line и Message"sv;
}

class CustomLogger {
    static auto GetTimeStamp() {
        const auto now = std::chrono::system_clock::now();
        const auto time = std::chrono::system_clock::to_time_t(now);

        return std::put_time(std::localtime(&time), "%F %T");
    }

public:
    static CustomLogger& GetInstance() {
        static CustomLogger logger;
        return logger;
    }

    template <class Callback>
    void Log(Callback&& callback) {
        std::lock_guard guard{mutex_};

        log_file_ << GetTimeStamp() << ": "sv;
        callback(log_file_);
        log_file_ << '\n';
    }

private:
    std::ofstream log_file_{"logs/custom_sample.log"s, std::ios_base::app};
    std::mutex mutex_;
};

class DurationMeasure {
public:
    explicit DurationMeasure(std::string_view title)
        : title_(title)
        , start_ts_(std::chrono::steady_clock::now()) {
        std::cout << title_ << ": "sv << std::flush;
    }

    ~DurationMeasure() {
        const auto end_ts = std::chrono::steady_clock::now();
        const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(
            end_ts - start_ts_
        );

        std::cout << duration.count() << " ms"sv << std::endl;
    }

private:
    std::string_view title_;
    std::chrono::steady_clock::time_point start_ts_;
};

void BoostLogIndexInThread(int thread_number, int index) {
    BOOST_LOG_TRIVIAL(info)
        << "Thread "sv << thread_number << " index "sv << index;
}

void CustomLogIndexInThread(int thread_number, int index) {
    CUSTOM_LOG("Thread "sv << thread_number << " index "sv << index);
}

void DemonstrateBenchmarks() {
    std::cout << "\n[6] Бенчмарки\n"sv;

    static const int num_iterations = 10000;

    {
        DurationMeasure measure{"Boost.Log"};
        std::thread thread1([] {
            for (int i = 0; i < num_iterations; ++i) {
                BoostLogIndexInThread(1, i);
            }
        });

        std::thread thread2([] {
            for (int i = 0; i < num_iterations; ++i) {
                BoostLogIndexInThread(2, i);
            }
        });

        thread1.join();
        thread2.join();
    }

    {
        DurationMeasure measure{"Custom logger"};
        std::thread thread1([] {
            for (int i = 0; i < num_iterations; ++i) {
                CustomLogIndexInThread(1, i);
            }
        });

        std::thread thread2([] {
            for (int i = 0; i < num_iterations; ++i) {
                CustomLogIndexInThread(2, i);
            }
        });

        thread1.join();
        thread2.join();
    }
}

int main() {
    system("mkdir -p logs");

    InitBoostLog();

    DemonstrateBasicBoostLog();
    DemonstrateEventsAndAttributes();
    DemonstrateFilters();
    DemonstrateFileLogging();
    DemonstrateProfessionalFormatting();
    DemonstrateBenchmarks();

    std::cout << "\nГотово. Проверьте файлы в папке logs." << std::endl;

    return 0;
}
