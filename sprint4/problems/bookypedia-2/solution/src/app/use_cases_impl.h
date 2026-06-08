#pragma once

#include "use_cases.h"

#include "../domain/author.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_(authors)
        , books_(books) {
    }

    void AddAuthor(const std::string& name) override;

    std::vector<AuthorInfo> GetAuthors() override;

    std::optional<AuthorInfo> FindAuthorByName(const std::string& name) override;

    bool DeleteAuthorById(const std::string& author_id) override;

    bool DeleteAuthorByName(const std::string& name) override;

    bool EditAuthor(const std::string& author_id, const std::string& new_name) override;

    void AddBook(const std::string& author_id,
                 const std::string& title,
                 int publication_year,
                 std::vector<std::string> tags) override;

    bool EditBook(const std::string& book_id,
                  const std::string& title,
                  int publication_year,
                  std::vector<std::string> tags) override;

    bool DeleteBook(const std::string& book_id) override;

    std::vector<BookInfo> GetBooks() override;

    std::vector<BookInfo> GetBooksByTitle(const std::string& title) override;

    std::vector<BookInfo> GetAuthorBooks(const std::string& author_id) override;

private:
    std::vector<BookInfo> ConvertBooks(std::vector<domain::Book> books);

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository& books_;
};

}  // namespace app
