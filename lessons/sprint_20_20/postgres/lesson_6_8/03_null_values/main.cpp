#include <iostream>
#include <optional>
#include <pqxx/pqxx>

using namespace std::literals;
using pqxx::operator"" _zv;

int main(int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cout << "Usage: db_example <conn-string>\n"sv;
            return argc == 1 ? EXIT_SUCCESS : EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};

        {
            pqxx::work w(conn);

            w.exec("CREATE TABLE IF NOT EXISTS points (x int, y int);"_zv);
            w.exec("DELETE FROM points;"_zv);
            w.exec("INSERT INTO points VALUES (DEFAULT, 10), (20, DEFAULT);"_zv);

            w.commit();
        }

        pqxx::read_transaction r(conn);

        for (auto [x, y] : r.query<std::optional<int>, std::optional<int>>(
                 "SELECT x, y FROM points;"_zv)) {
            std::cout << x.value_or(-9999) << ":" << y.value_or(-9999) << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
