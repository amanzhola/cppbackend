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

std::optional<domain::Author> AuthorRepositoryImpl::GetByName(const std::string& name) {
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

bool AuthorRepositoryImpl::DeleteById(const domain::AuthorId& id) {
    pqxx::work work{connection_};

    work.exec_params(
        R"(
DELETE FROM book_tags
WHERE book_id IN (
    SELECT id FROM books WHERE author_id = $1
);
)"_zv,
        id.ToString()
    );

    work.exec_params(
        "DELETE FROM books WHERE author_id = $1;"_zv,
        id.ToString()
    );

    const auto result = work.exec_params(
        "DELETE FROM authors WHERE id = $1;"_zv,
        id.ToString()
    );

    work.commit();

    return result.affected_rows() == 1;
}

bool AuthorRepositoryImpl::DeleteByName(const std::string& name) {
    pqxx::work work{connection_};

    const pqxx::result rows = work.exec_params(
        "SELECT id FROM authors WHERE name = $1;"_zv,
        name
    );

    if (rows.empty()) {
        return false;
    }

    const std::string id = rows.front()["id"].c_str();

    work.exec_params(
        R"(
DELETE FROM book_tags
WHERE book_id IN (
    SELECT id FROM books WHERE author_id = $1
);
)"_zv,
        id
    );

    work.exec_params(
        "DELETE FROM books WHERE author_id = $1;"_zv,
        id
    );

    const auto result = work.exec_params(
        "DELETE FROM authors WHERE id = $1;"_zv,
        id
    );

    work.commit();

    return result.affected_rows() == 1;
}

bool AuthorRepositoryImpl::UpdateName(const domain::AuthorId& id, const std::string& new_name) {
    pqxx::work work{connection_};

    const auto result = work.exec_params(
        "UPDATE authors SET name = $2 WHERE id = $1;"_zv,
        id.ToString(),
        new_name
    );

    work.commit();

    return result.affected_rows() == 1;
}

void BookRepositoryImpl::Save(const domain::Book& book) {
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

    for (const auto& tag : book.GetTags()) {
        work.exec_params(
            R"(
INSERT INTO book_tags (book_id, tag)
VALUES ($1, $2)
ON CONFLICT DO NOTHING;
)"_zv,
            book.GetId().ToString(),
            tag
        );
    }

    work.commit();
}

bool BookRepositoryImpl::Update(const domain::Book& book) {
    pqxx::work work{connection_};

    const auto updated = work.exec_params(
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

    if (updated.affected_rows() != 1) {
        return false;
    }

    work.exec_params(
        "DELETE FROM book_tags WHERE book_id = $1;"_zv,
        book.GetId().ToString()
    );

    for (const auto& tag : book.GetTags()) {
        work.exec_params(
            R"(
INSERT INTO book_tags (book_id, tag)
VALUES ($1, $2)
ON CONFLICT DO NOTHING;
)"_zv,
            book.GetId().ToString(),
            tag
        );
    }

    work.commit();

    return true;
}

bool BookRepositoryImpl::DeleteById(const domain::BookId& id) {
    pqxx::work work{connection_};

    work.exec_params(
        "DELETE FROM book_tags WHERE book_id = $1;"_zv,
        id.ToString()
    );

    const auto result = work.exec_params(
        "DELETE FROM books WHERE id = $1;"_zv,
        id.ToString()
    );

    work.commit();

    return result.affected_rows() == 1;
}

std::vector<std::string> BookRepositoryImpl::GetTags(pqxx::transaction_base& tx,
                                                     const domain::BookId& book_id) {
    std::vector<std::string> tags;

    const pqxx::result rows = tx.exec_params(
        R"(
SELECT tag
FROM book_tags
WHERE book_id = $1
ORDER BY tag;
)"_zv,
        book_id.ToString()
    );

    for (const auto& row : rows) {
        tags.push_back(row["tag"].c_str());
    }

    return tags;
}

std::vector<domain::Book> BookRepositoryImpl::GetAll() {
    pqxx::read_transaction read{connection_};

    std::vector<domain::Book> result;

    const pqxx::result rows = read.exec(
        R"(
SELECT b.id, b.author_id, b.title, b.publication_year
FROM books b
JOIN authors a ON b.author_id = a.id
ORDER BY b.title, a.name, b.publication_year;
)"_zv
    );

    for (const auto& row : rows) {
        auto id = domain::BookId::FromString(row["id"].c_str());

        result.emplace_back(
            id,
            domain::AuthorId::FromString(row["author_id"].c_str()),
            row["title"].c_str(),
            row["publication_year"].as<int>(),
            GetTags(read, id)
        );
    }

    return result;
}

std::vector<domain::Book> BookRepositoryImpl::GetByTitle(const std::string& title) {
    pqxx::read_transaction read{connection_};

    std::vector<domain::Book> result;

    const pqxx::result rows = read.exec_params(
        R"(
SELECT b.id, b.author_id, b.title, b.publication_year
FROM books b
JOIN authors a ON b.author_id = a.id
WHERE b.title = $1
ORDER BY b.title, a.name, b.publication_year;
)"_zv,
        title
    );

    for (const auto& row : rows) {
        auto id = domain::BookId::FromString(row["id"].c_str());

        result.emplace_back(
            id,
            domain::AuthorId::FromString(row["author_id"].c_str()),
            row["title"].c_str(),
            row["publication_year"].as<int>(),
            GetTags(read, id)
        );
    }

    return result;
}

std::vector<domain::Book> BookRepositoryImpl::GetByAuthorId(const domain::AuthorId& author_id) {
    pqxx::read_transaction read{connection_};

    std::vector<domain::Book> result;

    const pqxx::result rows = read.exec_params(
        R"(
SELECT id, author_id, title, publication_year
FROM books
WHERE author_id = $1
ORDER BY publication_year, title;
)"_zv,
        author_id.ToString()
    );

    for (const auto& row : rows) {
        auto id = domain::BookId::FromString(row["id"].c_str());

        result.emplace_back(
            id,
            domain::AuthorId::FromString(row["author_id"].c_str()),
            row["title"].c_str(),
            row["publication_year"].as<int>(),
            GetTags(read, id)
        );
    }

    return result;
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
    UNIQUE(book_id, tag)
);
)"_zv);

    work.commit();
}

}  // namespace postgres
