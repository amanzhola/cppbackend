#include <iostream>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

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

            w.exec(
                "CREATE TABLE IF NOT EXISTS movies ("
                "id SERIAL PRIMARY KEY, "
                "title varchar(200) NOT NULL, "
                "year integer NOT NULL"
                ");"_zv
            );

            w.exec("DELETE FROM movies;"_zv);

            w.exec(
                "INSERT INTO movies (title, year) VALUES "
                "('Trash', 2014), "
                "('The Kid', 2000), "
                "('The Sting', 1973), "
                "('The Terminal', 2004), "
                "('Amarcord', 1973), "
                "('The King''s Speech', 2010), "
                "('Det sjunde inseglet', 1957), "
                "('Groundhog Day', 1993);"_zv
            );

            w.commit();
        }

        pqxx::read_transaction r(conn);

        {
            double cube_root_of_100 = r.query_value<double>("SELECT ||/100.;"_zv);
            std::cout << "Cube root of 100: " << cube_root_of_100 << std::endl;
        }

        {
            auto query_text = "SELECT id, title, year FROM movies WHERE year < 2000 ORDER BY year, title"_zv;

            for (auto [id, title, year] : r.query<int, std::string_view, int>(query_text)) {
                std::cout << "Movie "sv << title << " ["sv << id << "] filmed in "sv << year << std::endl;
            }
        }

        {
            auto [id, title] = r.query1<int, std::string>(
                "SELECT id, title FROM movies WHERE year=2004 LIMIT 1;"_zv
            );

            std::cout << "Movie of 2004: "sv << title << " ["sv << id << "];"sv << std::endl;
        }

        {
            std::optional result = r.query01<int, std::string>(
                "SELECT id, title FROM movies WHERE year=1999 LIMIT 1;"_zv
            );

            if (result) {
                auto [id, title] = *result;
                std::cout << "Movie of 1999: "sv << title << " ["sv << id << "];"sv << std::endl;
            } else {
                std::cout << "No movie of 1999 in database"sv << std::endl;
            }
        }

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
