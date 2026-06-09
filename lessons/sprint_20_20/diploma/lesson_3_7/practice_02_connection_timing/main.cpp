#include <chrono>
#include <cstdlib>
#include <iostream>
#include <pqxx/pqxx>
#include <tuple>

int main() {
    using namespace std::chrono;

    try {
        const char* db_url = std::getenv("DB_URL");

        if (!db_url) {
            throw std::runtime_error("DB_URL is not specified");
        }

        const auto start_time = steady_clock::now();

        steady_clock::time_point conn_time;
        steady_clock::time_point tx_construction_time;
        steady_clock::time_point query_end_time;
        steady_clock::time_point tx_end_time;
        steady_clock::time_point conn_end_time;

        {
            pqxx::connection conn{db_url};
            conn_time = steady_clock::now();

            {
                pqxx::read_transaction tx{conn};
                tx_construction_time = steady_clock::now();

                std::ignore = tx.query1<int>("SELECT 1;");

                query_end_time = steady_clock::now();
            }

            tx_end_time = steady_clock::now();
        }

        conn_end_time = steady_clock::now();

        std::cout << "Connection time: "
                  << duration_cast<duration<double>>(conn_time - start_time).count()
                  << std::endl;

        std::cout << "Create transaction time: "
                  << duration_cast<duration<double>>(tx_construction_time - conn_time).count()
                  << std::endl;

        std::cout << "Query time: "
                  << duration_cast<duration<double>>(query_end_time - tx_construction_time).count()
                  << std::endl;

        std::cout << "Destroy transaction time: "
                  << duration_cast<duration<double>>(tx_end_time - query_end_time).count()
                  << std::endl;

        std::cout << "Close connection time: "
                  << duration_cast<duration<double>>(conn_end_time - tx_end_time).count()
                  << std::endl;

        std::cout << "Total time: "
                  << duration_cast<duration<double>>(conn_end_time - start_time).count()
                  << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
