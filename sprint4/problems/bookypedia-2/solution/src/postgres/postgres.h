#pragma once

#include <pqxx/connection>
#include <pqxx/transaction>

#include "../domain/author.h"

namespace postgres {

class AuthorRepositoryImpl : public domain::AuthorRepository {
public:
    explicit AuthorRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Author& author) override;

    std::vector<domain::Author> GetAll() override;

    std::optional<domain::Author> GetByName(const std::string& name) override;

    bool DeleteById(const domain::AuthorId& id) override;

    bool DeleteByName(const std::string& name) override;

    bool UpdateName(const domain::AuthorId& id, const std::string& new_name) override;

private:
    pqxx::connection& connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Book& book) override;

    bool Update(const domain::Book& book) override;

    bool DeleteById(const domain::BookId& id) override;

    std::vector<domain::Book> GetAll() override;

    std::vector<domain::Book> GetByTitle(const std::string& title) override;

    std::vector<domain::Book> GetByAuthorId(const domain::AuthorId& author_id) override;

private:
    std::vector<std::string> GetTags(pqxx::transaction_base& tx, const domain::BookId& book_id);

private:
    pqxx::connection& connection_;
};

class Database {
public:
    explicit Database(pqxx::connection connection);

    AuthorRepositoryImpl& GetAuthors() & {
        return authors_;
    }

    BookRepositoryImpl& GetBooks() & {
        return books_;
    }

private:
    pqxx::connection connection_;
    AuthorRepositoryImpl authors_{connection_};
    BookRepositoryImpl books_{connection_};
};

}  // namespace postgres
