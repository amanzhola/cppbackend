#pragma once

#include <string>
#include <string_view>
#include <vector>

#include "../util/tagged_uuid.h"

#include <optional>

namespace domain {

class Author;
using AuthorId = util::TaggedUUID<Author>;

class Book;
using BookId = util::TaggedUUID<Book>;

class Author {
public:
    Author(AuthorId id, std::string name)
        : id_(id)
        , name_(std::move(name)) {
    }

    const AuthorId& GetId() const noexcept {
        return id_;
    }

    std::string_view GetName() const noexcept {
        return name_;
    }

private:
    AuthorId id_;
    std::string name_;
};

class Book {
public:
    using Tags = std::vector<std::string>;

    Book(BookId id, AuthorId author_id, std::string title, int publication_year, Tags tags = {})
        : id_(id)
        , author_id_(author_id)
        , title_(std::move(title))
        , publication_year_(publication_year)
        , tags_(std::move(tags)) {
    }

    const BookId& GetId() const noexcept {
        return id_;
    }

    const AuthorId& GetAuthorId() const noexcept {
        return author_id_;
    }

    std::string_view GetTitle() const noexcept {
        return title_;
    }

    int GetPublicationYear() const noexcept {
        return publication_year_;
    }

    const Tags& GetTags() const noexcept {
        return tags_;
    }

private:
    BookId id_;
    AuthorId author_id_;
    std::string title_;
    int publication_year_;
    Tags tags_;
};

class AuthorRepository {
public:
    virtual void Save(const Author& author) = 0;
    virtual std::vector<Author> GetAll() = 0;
    virtual std::optional<Author> GetByName(const std::string& name) = 0;
    virtual bool DeleteById(const AuthorId& id) = 0;
    virtual bool DeleteByName(const std::string& name) = 0;
    virtual bool UpdateName(const AuthorId& id, const std::string& new_name) = 0;

protected:
    ~AuthorRepository() = default;
};

class BookRepository {
public:
    virtual void Save(const Book& book) = 0;
    virtual bool Update(const Book& book) = 0;
    virtual bool DeleteById(const BookId& id) = 0;

    virtual std::vector<Book> GetAll() = 0;
    virtual std::vector<Book> GetByTitle(const std::string& title) = 0;
    virtual std::vector<Book> GetByAuthorId(const AuthorId& author_id) = 0;

protected:
    ~BookRepository() = default;
};

}  // namespace domain
