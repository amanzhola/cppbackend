#pragma once

#include "use_cases.h"

#include "../domain/author.h"

namespace app {

class UseCasesImpl : public UseCases {
public:
    explicit UseCasesImpl(domain::AuthorRepository& authors)
        : authors_(authors)
        , books_(nullptr) {
    }

    UseCasesImpl(domain::AuthorRepository& authors, domain::BookRepository& books)
        : authors_(authors)
        , books_(&books) {
    }

    void AddAuthor(const std::string& name) override;

    AuthorInfo AddAuthorAndGet(const std::string& name) override;

    std::vector<AuthorInfo> GetAuthors() override;

    std::optional<AuthorInfo> FindAuthorByName(const std::string& name) override;

    bool DeleteAuthor(const std::string& author_id) override;

    bool EditAuthor(const std::string& author_id, const std::string& new_name) override;

    void AddBook(const std::string& author_id,
                 const std::string& title,
                 int publication_year,
                 const std::vector<std::string>& tags) override;

    std::vector<BookInfo> GetBooks() override;

    std::vector<BookInfo> GetBooksByTitle(const std::string& title) override;

    std::optional<BookInfo> GetBookById(const std::string& book_id) override;

    bool DeleteBook(const std::string& book_id) override;

    bool EditBook(const std::string& book_id,
                  const std::string& title,
                  int publication_year,
                  const std::vector<std::string>& tags) override;

    std::vector<BookInfo> GetAuthorBooks(const std::string& author_id) override;

private:
    static AuthorInfo ToAuthorInfo(const domain::Author& author);
    static BookInfo ToBookInfo(const domain::BookDetails& book);

private:
    domain::AuthorRepository& authors_;
    domain::BookRepository* books_ = nullptr;
};


}  // namespace app
