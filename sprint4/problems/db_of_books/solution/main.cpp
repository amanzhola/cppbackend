#include <boost/json.hpp>
#include <iostream>
#include <optional>
#include <pqxx/pqxx>
#include <string>
#include <string_view>

using namespace std::literals;
using pqxx::operator"" _zv;

namespace json = boost::json;

namespace {

constexpr auto kInsertBookWithIsbn = "insert_book_with_isbn"_zv;
constexpr auto kInsertBookWithoutIsbn = "insert_book_without_isbn"_zv;

void CreateTable(pqxx::connection& conn) {
    pqxx::work tx{conn};

    tx.exec(R"(
        CREATE TABLE IF NOT EXISTS books (
            id SERIAL PRIMARY KEY,
            title varchar(100) NOT NULL,
            author varchar(100) NOT NULL,
            year integer NOT NULL,
            isbn char(13) UNIQUE
        );
    )"_zv);

    tx.commit();
}

void PrepareQueries(pqxx::connection& conn) {
    conn.prepare(
        kInsertBookWithIsbn,
        "INSERT INTO books (title, author, year, isbn) VALUES ($1, $2, $3, $4)"_zv
    );

    conn.prepare(
        kInsertBookWithoutIsbn,
        "INSERT INTO books (title, author, year, isbn) VALUES ($1, $2, $3, NULL)"_zv
    );
}

bool AddBook(pqxx::connection& conn, const json::object& payload) {
    const std::string title = std::string(payload.at("title").as_string());
    const std::string author = std::string(payload.at("author").as_string());
    const int year = static_cast<int>(payload.at("year").as_int64());

    try {
        pqxx::work tx{conn};

        const json::value& isbn_value = payload.at("ISBN");

        if (isbn_value.is_null()) {
            tx.exec_prepared(kInsertBookWithoutIsbn, title, author, year);
        } else {
            const std::string isbn = std::string(isbn_value.as_string());
            tx.exec_prepared(kInsertBookWithIsbn, title, author, year, isbn);
        }

        tx.commit();
        return true;

    } catch (const pqxx::sql_error&) {
        return false;
    }
}

json::array GetAllBooks(pqxx::connection& conn) {
    pqxx::read_transaction tx{conn};

    json::array result;

    for (auto [id, title, author, year, isbn] :
         tx.query<int, std::string, std::string, int, std::optional<std::string>>(
             "SELECT id, title, author, year, isbn "
             "FROM books "
             "ORDER BY year DESC, title ASC, author ASC, isbn ASC"_zv)) {
        json::object book;

        book["id"] = id;
        book["title"] = title;
        book["author"] = author;
        book["year"] = year;

        if (isbn) {
            book["ISBN"] = *isbn;
        } else {
            book["ISBN"] = nullptr;
        }

        result.emplace_back(std::move(book));
    }

    return result;
}

void PrintAddResult(bool success) {
    json::object response;
    response["result"] = success;
    std::cout << json::serialize(response) << std::endl;
}

}  // namespace

int main(int argc, const char* argv[]) {
    try {
        if (argc != 2) {
            return EXIT_FAILURE;
        }

        pqxx::connection conn{argv[1]};

        CreateTable(conn);
        PrepareQueries(conn);

        std::string line;

        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                continue;
            }

            json::value request_value = json::parse(line);
            const json::object& request = request_value.as_object();

            const std::string action = std::string(request.at("action").as_string());
            const json::object& payload = request.at("payload").as_object();

            if (action == "add_book") {
                PrintAddResult(AddBook(conn, payload));
            } else if (action == "all_books") {
                std::cout << json::serialize(GetAllBooks(conn)) << std::endl;
            } else if (action == "exit") {
                break;
            }
        }

        return EXIT_SUCCESS;

    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
}
