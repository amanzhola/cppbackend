#include <boost/uuid/random_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <iostream>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

using namespace std::literals;
using pqxx::operator"" _zv;

class Movie {
public:
    using Id = boost::uuids::uuid;

    Movie(Id id, std::string title, int year)
        : id_(id)
        , title_(std::move(title))
        , year_(year) {
    }

    Id GetId() const {
        return id_;
    }

    std::string_view GetTitle() const {
        return title_;
    }

    int GetYear() const {
        return year_;
    }

private:
    Id id_;
    std::string title_;
    int year_;
};

class MovieRepository {
public:
    virtual ~MovieRepository() = default;

    virtual void Save(const Movie& movie) = 0;

    virtual std::optional<Movie> Load(const Movie::Id& id) = 0;

    virtual std::vector<Movie> LoadAll() = 0;
};

class PostgresMovieRepository : public MovieRepository {
public:
    explicit PostgresMovieRepository(pqxx::connection& conn)
        : conn_(conn) {
        CreateTable();
        PrepareQueries();
    }

    void Save(const Movie& movie) override {
        pqxx::work tx(conn_);

        tx.exec_prepared(
            "save_movie",
            boost::uuids::to_string(movie.GetId()),
            std::string(movie.GetTitle()),
            movie.GetYear()
        );

        tx.commit();
    }

    std::optional<Movie> Load(const Movie::Id& id) override {
        pqxx::read_transaction tx(conn_);

        auto result = tx.query01<std::string, std::string, int>(
            "SELECT id, title, year FROM movies WHERE id = " +
            tx.quote(boost::uuids::to_string(id))
        );

        if (!result) {
            return std::nullopt;
        }

        auto [id_text, title, year] = *result;

        boost::uuids::string_generator generator;
        return Movie{
            generator(id_text),
            title,
            year
        };
    }

    std::vector<Movie> LoadAll() override {
        pqxx::read_transaction tx(conn_);

        std::vector<Movie> result;

        boost::uuids::string_generator generator;

        for (auto [id_text, title, year] : tx.query<std::string, std::string, int>(
                 "SELECT id, title, year FROM movies ORDER BY year DESC, title ASC"_zv)) {
            result.emplace_back(
                generator(id_text),
                title,
                year
            );
        }

        return result;
    }

private:
    void CreateTable() {
        pqxx::work tx(conn_);

        tx.exec(
            "CREATE TABLE IF NOT EXISTS movies ("
            "id uuid PRIMARY KEY, "
            "title varchar(200) NOT NULL, "
            "year integer NOT NULL"
            ");"_zv
        );

        tx.commit();
    }

    void PrepareQueries() {
        conn_.prepare(
            "save_movie",
            "INSERT INTO movies (id, title, year) VALUES ($1, $2, $3) "
            "ON CONFLICT (id) DO UPDATE SET title = EXCLUDED.title, year = EXCLUDED.year"_zv
        );
    }

private:
    pqxx::connection& conn_;
};

class MovieUseCase {
public:
    explicit MovieUseCase(MovieRepository& repository)
        : repository_(repository) {
    }

    Movie AddMovie(std::string title, int year) {
        Movie movie{
            boost::uuids::random_generator{}(),
            std::move(title),
            year
        };

        repository_.Save(movie);

        return movie;
    }

    void PrintAllMovies() {
        for (const Movie& movie : repository_.LoadAll()) {
            std::cout
                << boost::uuids::to_string(movie.GetId())
                << " | "
                << movie.GetTitle()
                << " | "
                << movie.GetYear()
                << std::endl;
        }
    }

private:
    MovieRepository& repository_;
};

int main(int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            std::cerr << "Usage: repository_example <conn-string>" << std::endl;
            return EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};

        PostgresMovieRepository repository{conn};

        MovieUseCase use_case{repository};

        use_case.AddMovie("The Terminal", 2004);
        use_case.AddMovie("Groundhog Day", 1993);
        use_case.AddMovie("The King's Speech", 2010);

        use_case.PrintAllMovies();

        return EXIT_SUCCESS;

    } catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
