#include <iostream>
#include <pqxx/pqxx>

using namespace std::literals;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
    try {
        if (argc == 1) {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return EXIT_SUCCESS;
        } else if (argc != 2) {
            std::cerr << "Invalid command line\n"sv;
            return EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};

        pqxx::work w(conn);

        w.exec(
            "CREATE TABLE IF NOT EXISTS movies ("
            "id SERIAL PRIMARY KEY, "
            "title varchar(200) NOT NULL, "
            "year integer NOT NULL"
            ");"_zv
        );

        w.commit();

        std::cout << "Table movies is ready" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
