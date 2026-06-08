#include "view.h"

#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <unordered_set>

#include "../app/use_cases.h"
#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
namespace {

std::string Trim(std::string value) {
    boost::algorithm::trim(value);
    return value;
}

std::string NormalizeSpaces(std::string value) {
    value = Trim(std::move(value));

    std::string result;
    bool was_space = false;

    for (unsigned char ch : value) {
        if (std::isspace(ch)) {
            if (!was_space) {
                result.push_back(' ');
            }
            was_space = true;
        } else {
            result.push_back(static_cast<char>(ch));
            was_space = false;
        }
    }

    return result;
}

std::vector<std::string> ParseTags(const std::string& line) {
    std::vector<std::string> result;
    std::unordered_set<std::string> seen;

    std::stringstream ss(line);
    std::string part;

    while (std::getline(ss, part, ',')) {
        std::string tag = NormalizeSpaces(part);

        if (tag.empty()) {
            continue;
        }

        if (tag.size() > 30) {
            tag.resize(30);
            tag = Trim(tag);
        }

        if (seen.insert(tag).second) {
            result.push_back(tag);
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

std::string JoinTags(const std::vector<std::string>& tags) {
    std::string result;

    for (size_t i = 0; i < tags.size(); ++i) {
        if (i != 0) {
            result += ", ";
        }
        result += tags[i];
    }

    return result;
}

}  // namespace

namespace detail {

std::ostream& operator<<(std::ostream& out, const AuthorInfo& author) {
    out << author.name;
    return out;
}

std::ostream& operator<<(std::ostream& out, const BookInfo& book) {
    out << book.title << " by " << book.author_name << ", " << book.publication_year;
    return out;
}

}  // namespace detail

template <typename T>
void PrintVector(std::ostream& out, const std::vector<T>& vector) {
    int i = 1;
    for (const auto& value : vector) {
        out << i++ << " " << value << std::endl;
    }
}

View::View(menu::Menu& menu, app::UseCases& use_cases, std::istream& input, std::ostream& output)
    : menu_{menu}
    , use_cases_{use_cases}
    , input_{input}
    , output_{output} {
    menu_.AddAction("AddAuthor"s, "name"s, "Adds author"s, std::bind(&View::AddAuthor, this, ph::_1));
    menu_.AddAction("AddBook"s, "<pub year> <title>"s, "Adds book"s, std::bind(&View::AddBook, this, ph::_1));
    menu_.AddAction("ShowAuthors"s, {}, "Show authors"s, std::bind(&View::ShowAuthors, this));
    menu_.AddAction("ShowBooks"s, {}, "Show books"s, std::bind(&View::ShowBooks, this));
    menu_.AddAction("ShowAuthorBooks"s, {}, "Show author books"s, std::bind(&View::ShowAuthorBooks, this));
    menu_.AddAction("ShowBook"s, "title"s, "Show book"s, std::bind(&View::ShowBook, this, ph::_1));
    menu_.AddAction("DeleteBook"s, "title"s, "Delete book"s, std::bind(&View::DeleteBook, this, ph::_1));
    menu_.AddAction("DeleteAuthor"s, "name"s, "Delete author"s, std::bind(&View::DeleteAuthor, this, ph::_1));
    menu_.AddAction("EditAuthor"s, "name"s, "Edit author"s, std::bind(&View::EditAuthor, this, ph::_1));
    menu_.AddAction("EditBook"s, "title"s, "Edit book"s, std::bind(&View::EditBook, this, ph::_1));
}

bool View::AddAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        name = Trim(std::move(name));
        use_cases_.AddAuthor(name);
    } catch (const std::exception&) {
        output_ << "Failed to add author"sv << std::endl;
    }

    return true;
}

bool View::AddBook(std::istream& cmd_input) const {
    try {
        if (auto params = GetBookParams(cmd_input)) {
            use_cases_.AddBook(
                params->author_id,
                params->title,
                params->publication_year,
                params->tags
            );
        } else {
            output_ << "Failed to add book"sv << std::endl;
        }
    } catch (const std::exception&) {
        output_ << "Failed to add book"sv << std::endl;
    }

    return true;
}

bool View::ShowAuthors() const {
    PrintVector(output_, GetAuthors());
    return true;
}

bool View::ShowBooks() const {
    PrintVector(output_, GetBooks());
    return true;
}

bool View::ShowAuthorBooks() const {
    try {
        if (auto author_id = SelectAuthor()) {
            PrintVector(output_, GetAuthorBooks(*author_id));
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to Show Books");
    }

    return true;
}

bool View::ShowBook(std::istream& cmd_input) const {
    std::string title;
    std::getline(cmd_input, title);
    title = Trim(std::move(title));

    auto book = ResolveBook(title);
    if (book) {
        PrintBookDetails(*book);
    }

    return true;
}

bool View::DeleteBook(std::istream& cmd_input) const {
    std::string title;
    std::getline(cmd_input, title);
    title = Trim(std::move(title));

    auto books = title.empty() ? GetBooks() : GetBooksByTitle(title);

    if (books.empty()) {
        output_ << "Book not found"sv << std::endl;
        return true;
    }

    std::optional<detail::BookInfo> book;

    if (books.size() == 1) {
        book = books.front();
    } else {
        book = SelectBook(books);
    }

    if (!book) {
        return true;
    }

    if (!use_cases_.DeleteBook(book->id)) {
        output_ << "Failed to delete book"sv << std::endl;
    }

    return true;
}

bool View::DeleteAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        name = Trim(std::move(name));

        if (name.empty()) {
            auto author_id = SelectAuthor();
            if (!author_id) {
                return true;
            }

            use_cases_.DeleteAuthor(*author_id);
            return true;
        }

        auto found = use_cases_.FindAuthorByName(name);

        if (!found || !use_cases_.DeleteAuthor(found->id)) {
            output_ << "Failed to delete author"sv << std::endl;
        }

    } catch (const std::exception&) {
        output_ << "Failed to delete author"sv << std::endl;
    }

    return true;
}

bool View::EditAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        name = Trim(std::move(name));

        std::optional<detail::AuthorInfo> author;

        if (name.empty()) {
            output_ << "Select author:" << std::endl;

            auto author_id = SelectAuthor();
            if (author_id) {
                for (const auto& item : GetAuthors()) {
                    if (item.id == *author_id) {
                        author = item;
                        break;
                    }
                }
            }
        } else {
            std::stringstream name_as_choice(name);
            int author_idx = 0;

            if (name_as_choice >> author_idx) {
                std::string author_name_from_choice;
                std::getline(name_as_choice, author_name_from_choice);
                author_name_from_choice = Trim(std::move(author_name_from_choice));

                if (!author_name_from_choice.empty()) {
                    auto found = use_cases_.FindAuthorByName(author_name_from_choice);
                    if (found) {
                        author = detail::AuthorInfo{found->id, found->name};
                    }
                } else {
                    auto authors = GetAuthors();
                    --author_idx;

                    if (author_idx >= 0 && static_cast<size_t>(author_idx) < authors.size()) {
                        author = authors[author_idx];
                    }
                }
            } else {
                auto found = use_cases_.FindAuthorByName(name);
                if (found) {
                    author = detail::AuthorInfo{found->id, found->name};
                }
            }
        }

        if (!author) {
            output_ << "Failed to edit author"sv << std::endl;
            return true;
        }

        output_ << "Enter new name:" << std::endl;

        std::string new_name;
        std::getline(input_, new_name);
        new_name = Trim(std::move(new_name));

        if (!use_cases_.EditAuthor(author->id, new_name)) {
            output_ << "Failed to edit author"sv << std::endl;
        }

    } catch (const std::exception&) {
        output_ << "Failed to edit author"sv << std::endl;
    }

    return true;
}

bool View::EditBook(std::istream& cmd_input) const {
    try {
        std::string title;
        std::getline(cmd_input, title);
        title = Trim(std::move(title));

        auto book = ResolveBook(title);

        if (!book) {
            output_ << "Book not found"sv << std::endl;
            return true;
        }

        output_ << "Enter new title or empty line to use the current one ("
                << book->title << "):" << std::endl;

        std::string new_title;
        std::getline(input_, new_title);
        new_title = Trim(std::move(new_title));

        if (new_title.empty()) {
            new_title = book->title;
        }

        output_ << "Enter publication year or empty line to use the current one ("
                << book->publication_year << "):" << std::endl;

        std::string year_line;
        std::getline(input_, year_line);
        year_line = Trim(std::move(year_line));

        int new_year = book->publication_year;
        if (!year_line.empty()) {
            new_year = std::stoi(year_line);
        }

        output_ << "Enter tags (current tags: " << JoinTags(book->tags) << "):" << std::endl;

        std::string tags_line;
        std::getline(input_, tags_line);

        auto tags = ParseTags(tags_line);

        if (!use_cases_.EditBook(book->id, new_title, new_year, tags)) {
            output_ << "Book not found"sv << std::endl;
        }

    } catch (const std::exception&) {
        output_ << "Book not found"sv << std::endl;
    }

    return true;
}

std::optional<detail::AddBookParams> View::GetBookParams(std::istream& cmd_input) const {
    detail::AddBookParams params;

    cmd_input >> params.publication_year;
    std::getline(cmd_input, params.title);
    params.title = Trim(std::move(params.title));

    if (params.title.empty()) {
        return std::nullopt;
    }

    output_ << "Enter author name or empty line to select from list:" << std::endl;

    std::string author_line;
    std::getline(input_, author_line);
    author_line = Trim(std::move(author_line));

    if (author_line.empty()) {
        auto author_id = SelectAuthor();
        if (!author_id) {
            return std::nullopt;
        }
        params.author_id = *author_id;
    } else {
        std::stringstream choice(author_line);
        int author_idx = 0;

        if (choice >> author_idx) {
            std::string author_name_from_choice;
            std::getline(choice, author_name_from_choice);
            author_name_from_choice = Trim(std::move(author_name_from_choice));

            if (!author_name_from_choice.empty()) {
                auto author = use_cases_.FindAuthorByName(author_name_from_choice);

                if (!author) {
                    return std::nullopt;
                }

                params.author_id = author->id;
            } else {
                auto authors = GetAuthors();
                --author_idx;

                if (author_idx < 0 || static_cast<size_t>(author_idx) >= authors.size()) {
                    return std::nullopt;
                }

                params.author_id = authors[author_idx].id;
            }
        } else {
            auto author = use_cases_.FindAuthorByName(author_line);

            if (!author) {
                output_ << "No author found. Do you want to add "
                        << author_line << " (y/n)?" << std::endl;

                std::string answer;
                std::getline(input_, answer);
                answer = Trim(std::move(answer));

                if (answer != "y" && answer != "Y") {
                    return std::nullopt;
                }

                author = use_cases_.AddAuthorAndGet(author_line);
            }

            params.author_id = author->id;
        }
    }

    output_ << "Enter tags (comma separated):" << std::endl;

    std::string tags_line;
    std::getline(input_, tags_line);

    params.tags = ParseTags(tags_line);

    return params;
}

std::optional<std::string> View::SelectAuthor() const {
    auto authors = GetAuthors();

    PrintVector(output_, authors);
    output_ << "Enter author # or empty line to cancel" << std::endl;

    std::string str;
    if (!std::getline(input_, str)) {
        return std::nullopt;
    }

    str = Trim(std::move(str));

    if (str.empty()) {
        return std::nullopt;
    }

    std::stringstream ss(str);
    int author_idx = 0;
    ss >> author_idx;
    --author_idx;

    if (author_idx < 0 || static_cast<size_t>(author_idx) >= authors.size()) {
        return std::nullopt;
    }

    return authors[author_idx].id;
}

std::optional<detail::BookInfo> View::SelectBook(const std::vector<detail::BookInfo>& books) const {
    PrintVector(output_, books);
    output_ << "Enter the book # or empty line to cancel:" << std::endl;

    std::string str;
    if (!std::getline(input_, str)) {
        return std::nullopt;
    }

    str = Trim(std::move(str));

    if (str.empty()) {
        return std::nullopt;
    }

    std::stringstream ss(str);
    int book_idx = 0;
    ss >> book_idx;
    --book_idx;

    if (book_idx < 0 || static_cast<size_t>(book_idx) >= books.size()) {
        return std::nullopt;
    }

    return books[book_idx];
}

std::optional<detail::BookInfo> View::ResolveBook(const std::string& title) const {
    auto books = title.empty() ? GetBooks() : GetBooksByTitle(title);

    if (books.empty()) {
        return std::nullopt;
    }

    if (books.size() == 1) {
        return books.front();
    }

    return SelectBook(books);
}

std::vector<detail::AuthorInfo> View::GetAuthors() const {
    std::vector<detail::AuthorInfo> result;

    for (const auto& author : use_cases_.GetAuthors()) {
        result.push_back({author.id, author.name});
    }

    return result;
}

std::vector<detail::BookInfo> View::GetBooks() const {
    std::vector<detail::BookInfo> result;

    for (const auto& book : use_cases_.GetBooks()) {
        result.push_back({
            book.id,
            book.title,
            book.author_id,
            book.author_name,
            book.publication_year,
            book.tags
        });
    }

    return result;
}

std::vector<detail::BookInfo> View::GetBooksByTitle(const std::string& title) const {
    std::vector<detail::BookInfo> result;

    for (const auto& book : use_cases_.GetBooksByTitle(title)) {
        result.push_back({
            book.id,
            book.title,
            book.author_id,
            book.author_name,
            book.publication_year,
            book.tags
        });
    }

    return result;
}

std::vector<detail::BookInfo> View::GetAuthorBooks(const std::string& author_id) const {
    std::vector<detail::BookInfo> result;

    for (const auto& book : use_cases_.GetAuthorBooks(author_id)) {
        result.push_back({
            book.id,
            book.title,
            book.author_id,
            book.author_name,
            book.publication_year,
            book.tags
        });
    }

    return result;
}

void View::PrintBookDetails(const detail::BookInfo& book) const {
    output_ << "Title: " << book.title << std::endl;
    output_ << "Author: " << book.author_name << std::endl;
    output_ << "Publication year: " << book.publication_year << std::endl;

    if (!book.tags.empty()) {
        output_ << "Tags: " << JoinTags(book.tags) << std::endl;
    }
}

}  // namespace ui
