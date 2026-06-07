#include <iostream>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <utility>

using namespace std::literals;
using pqxx::operator"" _zv;

class Movie {
public:
    using Id = std::string;

    Movie(Id id, std::string title, int year)
        : id_(std::move(id))
        , title_(std::move(title))
        , year_(year) {
    }

    const Id& GetId() const {
        return id_;
    }

    std::string_view GetTitle() const {
        return title_;
    }

    int GetYear() const {
        return year_;
    }

    static void CreateTable(pqxx::connection& conn) {
        pqxx::work w(conn);

        w.exec(
            "CREATE TABLE IF NOT EXISTS naive_movies ("
            "id varchar(100) PRIMARY KEY, "
            "title varchar(200) NOT NULL, "
            "year integer NOT NULL"
            ");"_zv
        );

        w.commit();
    }

    static std::optional<Movie> LoadFromDB(pqxx::connection& conn, const Id& id) {
        pqxx::read_transaction r(conn);

        auto result = r.query01<std::string, int>(
            "SELECT title, year FROM naive_movies WHERE id = " + r.quote(id)
        );

        if (!result) {
            return std::nullopt;
        }

        auto [title, year] = *result;

        return Movie{id, title, year};
    }

    void SaveToDB(pqxx::connection& conn) const {
        pqxx::work w(conn);

        w.exec(
            "INSERT INTO naive_movies (id, title, year) VALUES (" +
            w.quote(id_) + ", " +
            w.quote(title_) + ", " +
            std::to_string(year_) +
            ") "
            "ON CONFLICT (id) DO UPDATE SET "
            "title = EXCLUDED.title, "
            "year = EXCLUDED.year"
        );

        w.commit();
    }

private:
    Id id_;
    std::string title_;
    int year_;
};

int main(int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: naive_example <conn-string>" << std::endl;
            return EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};

        Movie::CreateTable(conn);

        Movie movie{
            "movie-1",
            "The Terminal",
            2004
        };

        movie.SaveToDB(conn);

        std::optional<Movie> loaded = Movie::LoadFromDB(conn, "movie-1");

        if (loaded) {
            std::cout
                << loaded->GetId()
                << " | "
                << loaded->GetTitle()
                << " | "
                << loaded->GetYear()
                << std::endl;
        } else {
            std::cout << "Movie not found" << std::endl;
        }

        return EXIT_SUCCESS;

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
