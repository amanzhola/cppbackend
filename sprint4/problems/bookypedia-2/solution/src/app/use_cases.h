#pragma once

#include <optional>
#include <string>
#include <vector>

namespace app {

struct AuthorInfo {
    std::string id;
    std::string name;
};

struct BookInfo {
    std::string id;
    std::string title;
    std::string author_name;
    int publication_year = 0;
    std::vector<std::string> tags;
};

class UseCases {
public:
    virtual void AddAuthor(const std::string& name) = 0;

    virtual std::vector<AuthorInfo> GetAuthors() = 0;

    virtual std::optional<AuthorInfo> FindAuthorByName(const std::string& name) = 0;

    virtual bool DeleteAuthorById(const std::string& author_id) = 0;

    virtual bool DeleteAuthorByName(const std::string& name) = 0;

    virtual bool EditAuthor(const std::string& author_id, const std::string& new_name) = 0;

    virtual void AddBook(const std::string& author_id,
                         const std::string& title,
                         int publication_year,
                         std::vector<std::string> tags) = 0;

    virtual bool EditBook(const std::string& book_id,
                          const std::string& title,
                          int publication_year,
                          std::vector<std::string> tags) = 0;

    virtual bool DeleteBook(const std::string& book_id) = 0;

    virtual std::vector<BookInfo> GetBooks() = 0;

    virtual std::vector<BookInfo> GetBooksByTitle(const std::string& title) = 0;

    virtual std::vector<BookInfo> GetAuthorBooks(const std::string& author_id) = 0;

protected:
    ~UseCases() = default;
};

}  // namespace app
