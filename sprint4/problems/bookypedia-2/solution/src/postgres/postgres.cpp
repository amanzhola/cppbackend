#include "postgres.h"

#include <pqxx/pqxx>
#include <pqxx/zview.hxx>

namespace postgres {

using namespace std::literals;
using pqxx::operator"" _zv;

void AuthorRepositoryImpl::Save(const domain::Author& author) {
    pqxx::work work{connection_};

    work.exec_params(
        R"(
INSERT INTO authors (id, name)
VALUES ($1, $2);
)"_zv,
        author.GetId().ToString(),
        author.GetName()
    );

    work.commit();
}

std::vector<domain::Author> AuthorRepositoryImpl::GetAll() {
    pqxx::read_transaction read{connection_};

    std::vector<domain::Author> result;

    const pqxx::result rows = read.exec(
        "SELECT id, name FROM authors ORDER BY name;"_zv
    );

    for (const auto& row : rows) {
        result.emplace_back(
            domain::AuthorId::FromString(row["id"].c_str()),
            row["name"].c_str()
        );
    }

    return result;
}

std::optional<domain::Author> AuthorRepositoryImpl::FindByName(const std::string& name) {
    pqxx::read_transaction read{connection_};

    const pqxx::result rows = read.exec_params(
        "SELECT id, name FROM authors WHERE name = $1;"_zv,
        name
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    const auto& row = rows.front();

    return domain::Author{
        domain::AuthorId::FromString(row["id"].c_str()),
        row["name"].c_str()
    };
}

std::optional<domain::Author> AuthorRepositoryImpl::FindById(const domain::AuthorId& id) {
    pqxx::read_transaction read{connection_};

    const pqxx::result rows = read.exec_params(
        "SELECT id, name FROM authors WHERE id = $1;"_zv,
        id.ToString()
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    const auto& row = rows.front();

    return domain::Author{
        domain::AuthorId::FromString(row["id"].c_str()),
        row["name"].c_str()
    };
}

bool AuthorRepositoryImpl::Delete(const domain::AuthorId& id) {
    pqxx::work work{connection_};

    work.exec_params(R"(
DELETE FROM book_tags
WHERE book_id IN (
    SELECT id FROM books WHERE author_id = $1
);
)"_zv, id.ToString());

    work.exec_params(
        "DELETE FROM books WHERE author_id = $1;"_zv,
        id.ToString()
    );

    const pqxx::result result = work.exec_params(
        "DELETE FROM authors WHERE id = $1;"_zv,
        id.ToString()
    );

    work.commit();

    return result.affected_rows() == 1;
}

bool AuthorRepositoryImpl::Update(const domain::Author& author) {
    pqxx::work work{connection_};

    const pqxx::result result = work.exec_params(
        "UPDATE authors SET name = $2 WHERE id = $1;"_zv,
        author.GetId().ToString(),
        author.GetName()
    );

    work.commit();

    return result.affected_rows() == 1;
}

void BookRepositoryImpl::Save(const domain::Book& book, const std::vector<std::string>& tags) {
    pqxx::work work{connection_};

    work.exec_params(
        R"(
INSERT INTO books (id, author_id, title, publication_year)
VALUES ($1, $2, $3, $4);
)"_zv,
        book.GetId().ToString(),
        book.GetAuthorId().ToString(),
        book.GetTitle(),
        book.GetPublicationYear()
    );

    for (const auto& tag : tags) {
        work.exec_params(
            "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2) ON CONFLICT DO NOTHING;"_zv,
            book.GetId().ToString(),
            tag
        );
    }

    work.commit();
}

std::vector<std::string> BookRepositoryImpl::LoadTags(pqxx::transaction_base& tx,
                                                      const domain::BookId& book_id) {
    std::vector<std::string> tags;

    const pqxx::result rows = tx.exec_params(
        "SELECT tag FROM book_tags WHERE book_id = $1 ORDER BY tag;"_zv,
        book_id.ToString()
    );

    for (const auto& row : rows) {
        tags.push_back(row["tag"].c_str());
    }

    return tags;
}

domain::BookDetails BookRepositoryImpl::MakeBookDetails(pqxx::transaction_base& tx,
                                                        const pqxx::row& row) {
    domain::BookDetails book{
        domain::BookId::FromString(row["id"].c_str()),
        domain::AuthorId::FromString(row["author_id"].c_str()),
        row["title"].c_str(),
        row["author_name"].c_str(),
        row["publication_year"].as<int>(),
        {}
    };

    book.tags = LoadTags(tx, book.id);

    return book;
}

std::vector<domain::BookDetails> BookRepositoryImpl::GetAllDetailed() {
    pqxx::read_transaction read{connection_};

    std::vector<domain::BookDetails> result;

    const pqxx::result rows = read.exec(
        R"(
SELECT b.id, b.author_id, b.title, b.publication_year, a.name AS author_name
FROM books b
JOIN authors a ON b.author_id = a.id
ORDER BY b.title, a.name, b.publication_year;
)"_zv
    );

    for (const auto& row : rows) {
        result.push_back(MakeBookDetails(read, row));
    }

    return result;
}

std::vector<domain::BookDetails> BookRepositoryImpl::GetByTitleDetailed(const std::string& title) {
    pqxx::read_transaction read{connection_};

    std::vector<domain::BookDetails> result;

    const pqxx::result rows = read.exec_params(
        R"(
SELECT b.id, b.author_id, b.title, b.publication_year, a.name AS author_name
FROM books b
JOIN authors a ON b.author_id = a.id
WHERE b.title = $1
ORDER BY b.title, a.name, b.publication_year;
)"_zv,
        title
    );

    for (const auto& row : rows) {
        result.push_back(MakeBookDetails(read, row));
    }

    return result;
}

std::optional<domain::BookDetails> BookRepositoryImpl::FindDetailedById(const domain::BookId& id) {
    pqxx::read_transaction read{connection_};

    const pqxx::result rows = read.exec_params(
        R"(
SELECT b.id, b.author_id, b.title, b.publication_year, a.name AS author_name
FROM books b
JOIN authors a ON b.author_id = a.id
WHERE b.id = $1;
)"_zv,
        id.ToString()
    );

    if (rows.empty()) {
        return std::nullopt;
    }

    return MakeBookDetails(read, rows.front());
}

bool BookRepositoryImpl::Delete(const domain::BookId& id) {
    pqxx::work work{connection_};

    work.exec_params(
        "DELETE FROM book_tags WHERE book_id = $1;"_zv,
        id.ToString()
    );

    const pqxx::result result = work.exec_params(
        "DELETE FROM books WHERE id = $1;"_zv,
        id.ToString()
    );

    work.commit();

    return result.affected_rows() == 1;
}

bool BookRepositoryImpl::Update(const domain::Book& book, const std::vector<std::string>& tags) {
    pqxx::work work{connection_};

    const pqxx::result result = work.exec_params(
        R"(
UPDATE books
SET title = $2,
    publication_year = $3
WHERE id = $1;
)"_zv,
        book.GetId().ToString(),
        book.GetTitle(),
        book.GetPublicationYear()
    );

    if (result.affected_rows() != 1) {
        return false;
    }

    work.exec_params(
        "DELETE FROM book_tags WHERE book_id = $1;"_zv,
        book.GetId().ToString()
    );

    for (const auto& tag : tags) {
        work.exec_params(
            "INSERT INTO book_tags (book_id, tag) VALUES ($1, $2) ON CONFLICT DO NOTHING;"_zv,
            book.GetId().ToString(),
            tag
        );
    }

    work.commit();

    return true;
}

Database::Database(pqxx::connection connection)
    : connection_{std::move(connection)} {
    pqxx::work work{connection_};

    work.exec(R"(
CREATE TABLE IF NOT EXISTS authors (
    id UUID CONSTRAINT author_id_constraint PRIMARY KEY,
    name varchar(100) UNIQUE NOT NULL
);
)"_zv);

    work.exec(R"(
CREATE TABLE IF NOT EXISTS books (
    id UUID CONSTRAINT book_id_constraint PRIMARY KEY,
    author_id UUID NOT NULL REFERENCES authors(id) ON DELETE CASCADE,
    title varchar(100) NOT NULL,
    publication_year integer NOT NULL
);
)"_zv);

    work.exec(R"(
CREATE TABLE IF NOT EXISTS book_tags (
    book_id UUID NOT NULL REFERENCES books(id) ON DELETE CASCADE,
    tag varchar(30) NOT NULL,
    PRIMARY KEY (book_id, tag)
);
)"_zv);

    work.commit();
}

}  // namespace postgres
