#include "use_cases_impl.h"

#include <stdexcept>

namespace app {

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Author name is empty");
    }

    authors_.Save({domain::AuthorId::New(), name});
}

std::vector<AuthorInfo> UseCasesImpl::GetAuthors() {
    std::vector<AuthorInfo> result;

    for (const auto& author : authors_.GetAll()) {
        result.push_back({
            author.GetId().ToString(),
            author.GetName()
        });
    }

    return result;
}

void UseCasesImpl::AddBook(const std::string& author_id,
                           const std::string& title,
                           int publication_year) {
    if (!books_) {
        throw std::logic_error("Book repository is not configured");
    }

    if (title.empty()) {
        throw std::invalid_argument("Book title is empty");
    }

    books_->Save({
        domain::BookId::New(),
        domain::AuthorId::FromString(author_id),
        title,
        publication_year
    });
}

std::vector<BookInfo> UseCasesImpl::GetBooks() {
    if (!books_) {
        return {};
    }

    std::vector<BookInfo> result;

    for (const auto& book : books_->GetAll()) {
        result.push_back({
            book.GetTitle(),
            book.GetPublicationYear()
        });
    }

    return result;
}

std::vector<BookInfo> UseCasesImpl::GetAuthorBooks(const std::string& author_id) {
    if (!books_) {
        return {};
    }

    std::vector<BookInfo> result;

    for (const auto& book : books_->GetByAuthorId(domain::AuthorId::FromString(author_id))) {
        result.push_back({
            book.GetTitle(),
            book.GetPublicationYear()
        });
    }

    return result;
}

}  // namespace app
