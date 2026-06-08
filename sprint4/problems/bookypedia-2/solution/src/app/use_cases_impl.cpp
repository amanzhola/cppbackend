#include "use_cases_impl.h"

#include <stdexcept>

namespace app {

AuthorInfo UseCasesImpl::ToAuthorInfo(const domain::Author& author) {
    return {
        author.GetId().ToString(),
        author.GetName()
    };
}

BookInfo UseCasesImpl::ToBookInfo(const domain::BookDetails& book) {
    return {
        book.id.ToString(),
        book.title,
        book.author_id.ToString(),
        book.author_name,
        book.publication_year,
        book.tags
    };
}

void UseCasesImpl::AddAuthor(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Author name is empty");
    }

    authors_.Save({domain::AuthorId::New(), name});
}

AuthorInfo UseCasesImpl::AddAuthorAndGet(const std::string& name) {
    if (name.empty()) {
        throw std::invalid_argument("Author name is empty");
    }

    domain::Author author{domain::AuthorId::New(), name};
    authors_.Save(author);

    return ToAuthorInfo(author);
}

std::vector<AuthorInfo> UseCasesImpl::GetAuthors() {
    std::vector<AuthorInfo> result;

    for (const auto& author : authors_.GetAll()) {
        result.push_back(ToAuthorInfo(author));
    }

    return result;
}

std::optional<AuthorInfo> UseCasesImpl::FindAuthorByName(const std::string& name) {
    auto author = authors_.FindByName(name);

    if (!author) {
        return std::nullopt;
    }

    return ToAuthorInfo(*author);
}

bool UseCasesImpl::DeleteAuthor(const std::string& author_id) {
    return authors_.Delete(domain::AuthorId::FromString(author_id));
}

bool UseCasesImpl::EditAuthor(const std::string& author_id, const std::string& new_name) {
    if (new_name.empty()) {
        return false;
    }

    return authors_.Update({
        domain::AuthorId::FromString(author_id),
        new_name
    });
}

void UseCasesImpl::AddBook(const std::string& author_id,
                           const std::string& title,
                           int publication_year,
                           const std::vector<std::string>& tags) {
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
    }, tags);
}

std::vector<BookInfo> UseCasesImpl::GetBooks() {
    if (!books_) {
        return {};
    }

    std::vector<BookInfo> result;

    for (const auto& book : books_->GetAllDetailed()) {
        result.push_back(ToBookInfo(book));
    }

    return result;
}

std::vector<BookInfo> UseCasesImpl::GetBooksByTitle(const std::string& title) {
    if (!books_) {
        return {};
    }

    std::vector<BookInfo> result;

    for (const auto& book : books_->GetByTitleDetailed(title)) {
        result.push_back(ToBookInfo(book));
    }

    return result;
}

std::optional<BookInfo> UseCasesImpl::GetBookById(const std::string& book_id) {
    if (!books_) {
        return std::nullopt;
    }

    auto book = books_->FindDetailedById(domain::BookId::FromString(book_id));

    if (!book) {
        return std::nullopt;
    }

    return ToBookInfo(*book);
}

bool UseCasesImpl::DeleteBook(const std::string& book_id) {
    if (!books_) {
        return false;
    }

    return books_->Delete(domain::BookId::FromString(book_id));
}

bool UseCasesImpl::EditBook(const std::string& book_id,
                            const std::string& title,
                            int publication_year,
                            const std::vector<std::string>& tags) {
    if (!books_) {
        return false;
    }

    auto old_book = books_->FindDetailedById(domain::BookId::FromString(book_id));

    if (!old_book) {
        return false;
    }

    return books_->Update({
        domain::BookId::FromString(book_id),
        domain::AuthorId::FromString(old_book->author_id.ToString()),
        title,
        publication_year
    }, tags);
}

std::vector<BookInfo> UseCasesImpl::GetAuthorBooks(const std::string& author_id) {
    if (!books_) {
        return {};
    }

    std::vector<BookInfo> result;

    for (const auto& book : books_->GetAllDetailed()) {
        if (book.author_id.ToString() == author_id) {
            result.push_back(ToBookInfo(book));
        }
    }

    return result;
}

}  // namespace app
