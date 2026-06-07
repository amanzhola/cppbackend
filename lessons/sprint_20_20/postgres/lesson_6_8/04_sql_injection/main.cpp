#include <iostream>
#include <pqxx/pqxx>
#include <string>

using namespace std::literals;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return argc == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
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

        std::string title = "xxx', 1); DROP TABLE movies; --";
        int year = 2026;

        w.exec(
            "INSERT INTO movies (title, year) VALUES (" +
            w.quote(title) +
            ", " +
            std::to_string(year) +
            ")"
        );

        w.commit();

        std::cout << "Dangerous title was inserted safely" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
