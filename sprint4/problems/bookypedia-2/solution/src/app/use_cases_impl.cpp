#include "use_cases_impl.h"

#include <stdexcept>
#include <unordered_map>

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
            std::string(author.GetName())
        });
    }

    return result;
}

std::optional<AuthorInfo> UseCasesImpl::FindAuthorByName(const std::string& name) {
    auto author = authors_.GetByName(name);

    if (!author) {
        return std::nullopt;
    }

    return AuthorInfo{
        author->GetId().ToString(),
        std::string(author->GetName())
    };
}

bool UseCasesImpl::DeleteAuthorById(const std::string& author_id) {
    return authors_.DeleteById(domain::AuthorId::FromString(author_id));
}

bool UseCasesImpl::DeleteAuthorByName(const std::string& name) {
    return authors_.DeleteByName(name);
}

bool UseCasesImpl::EditAuthor(const std::string& author_id, const std::string& new_name) {
    if (new_name.empty()) {
        return false;
    }

    return authors_.UpdateName(domain::AuthorId::FromString(author_id), new_name);
}

void UseCasesImpl::AddBook(const std::string& author_id,
                           const std::string& title,
                           int publication_year,
                           std::vector<std::string> tags) {
    if (title.empty()) {
        throw std::invalid_argument("Book title is empty");
    }

    books_.Save({
        domain::BookId::New(),
        domain::AuthorId::FromString(author_id),
        title,
        publication_year,
        std::move(tags)
    });
}

bool UseCasesImpl::EditBook(const std::string& book_id,
                            const std::string& title,
                            int publication_year,
                            std::vector<std::string> tags) {
    if (title.empty()) {
        return false;
    }

    auto all_books = books_.GetAll();

    for (const auto& book : all_books) {
        if (book.GetId().ToString() == book_id) {
            return books_.Update({
                domain::BookId::FromString(book_id),
                book.GetAuthorId(),
                title,
                publication_year,
                std::move(tags)
            });
        }
    }

    return false;
}

bool UseCasesImpl::DeleteBook(const std::string& book_id) {
    return books_.DeleteById(domain::BookId::FromString(book_id));
}

std::vector<BookInfo> UseCasesImpl::ConvertBooks(std::vector<domain::Book> books) {
    std::unordered_map<std::string, std::string> author_names;

    for (const auto& author : authors_.GetAll()) {
        author_names[author.GetId().ToString()] = std::string(author.GetName());
    }

    std::vector<BookInfo> result;

    for (const auto& book : books) {
        result.push_back({
            book.GetId().ToString(),
            std::string(book.GetTitle()),
            author_names[book.GetAuthorId().ToString()],
            book.GetPublicationYear(),
            book.GetTags()
        });
    }

    return result;
}

std::vector<BookInfo> UseCasesImpl::GetBooks() {
    return ConvertBooks(books_.GetAll());
}

std::vector<BookInfo> UseCasesImpl::GetBooksByTitle(const std::string& title) {
    return ConvertBooks(books_.GetByTitle(title));
}

std::vector<BookInfo> UseCasesImpl::GetAuthorBooks(const std::string& author_id) {
    return ConvertBooks(books_.GetByAuthorId(domain::AuthorId::FromString(author_id)));
}

}  // namespace app
