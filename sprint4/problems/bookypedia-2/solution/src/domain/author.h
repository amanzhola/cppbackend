#pragma once

#include <optional>
#include <string>
#include <vector>

#include "../util/tagged_uuid.h"

namespace domain {

namespace detail {
struct AuthorTag {};
struct BookTag {};
}  // namespace detail

using AuthorId = util::TaggedUUID<detail::AuthorTag>;
using BookId = util::TaggedUUID<detail::BookTag>;

class Author {
public:
    Author(AuthorId id, std::string name)
        : id_(std::move(id))
        , name_(std::move(name)) {
    }

    const AuthorId& GetId() const noexcept {
        return id_;
    }

    const std::string& GetName() const noexcept {
        return name_;
    }

private:
    AuthorId id_;
    std::string name_;
};

class Book {
public:
    Book(BookId id, AuthorId author_id, std::string title, int publication_year)
        : id_(std::move(id))
        , author_id_(std::move(author_id))
        , title_(std::move(title))
        , publication_year_(publication_year) {
    }

    const BookId& GetId() const noexcept {
        return id_;
    }

    const AuthorId& GetAuthorId() const noexcept {
        return author_id_;
    }

    const std::string& GetTitle() const noexcept {
        return title_;
    }

    int GetPublicationYear() const noexcept {
        return publication_year_;
    }

private:
    BookId id_;
    AuthorId author_id_;
    std::string title_;
    int publication_year_;
};

struct BookDetails {
    BookId id;
    AuthorId author_id;
    std::string title;
    std::string author_name;
    int publication_year = 0;
    std::vector<std::string> tags;
};

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;

    virtual std::vector<Author> GetAll() {
        return {};
    }

    virtual std::optional<Author> FindByName(const std::string&) {
        return std::nullopt;
    }

    virtual std::optional<Author> FindById(const AuthorId&) {
        return std::nullopt;
    }

    virtual bool Delete(const AuthorId&) {
        return false;
    }

    virtual bool Update(const Author&) {
        return false;
    }

protected:
    ~AuthorRepository() = default;
};

class BookRepository {
public:
    virtual void Save(const Book& book) {
        Save(book, {});
    }

    virtual void Save(const Book&, const std::vector<std::string>&) {
    }

    virtual std::vector<Book> GetAll() {
        return {};
    }

    virtual std::vector<Book> GetByAuthorId(const AuthorId&) {
        return {};
    }

    virtual std::vector<BookDetails> GetAllDetailed() {
        return {};
    }

    virtual std::vector<BookDetails> GetByTitleDetailed(const std::string&) {
        return {};
    }

    virtual std::optional<BookDetails> FindDetailedById(const BookId&) {
        return std::nullopt;
    }

    virtual bool Delete(const BookId&) {
        return false;
    }

    virtual bool Update(const Book&, const std::vector<std::string>&) {
        return false;
    }

protected:
    ~BookRepository() = default;
};

}  // namespace domain
