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

    std::optional<domain::Author> FindByName(const std::string& name) override;

    std::optional<domain::Author> FindById(const domain::AuthorId& id) override;

    bool Delete(const domain::AuthorId& id) override;

    bool Update(const domain::Author& author) override;

private:
    pqxx::connection& connection_;
};

class BookRepositoryImpl : public domain::BookRepository {
public:
    explicit BookRepositoryImpl(pqxx::connection& connection)
        : connection_{connection} {
    }

    void Save(const domain::Book& book, const std::vector<std::string>& tags) override;

    std::vector<domain::BookDetails> GetAllDetailed() override;

    std::vector<domain::BookDetails> GetByTitleDetailed(const std::string& title) override;

    std::optional<domain::BookDetails> FindDetailedById(const domain::BookId& id) override;

    bool Delete(const domain::BookId& id) override;

    bool Update(const domain::Book& book, const std::vector<std::string>& tags) override;

private:
    std::vector<std::string> LoadTags(pqxx::transaction_base& tx, const domain::BookId& book_id);

    domain::BookDetails MakeBookDetails(pqxx::transaction_base& tx, const pqxx::row& row);

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
