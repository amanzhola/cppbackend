#include "view.h"

#include <boost/algorithm/string/trim.hpp>

#include <algorithm>
#include <cctype>
#include <iostream>
#include <set>
#include <sstream>

#include "../app/use_cases.h"
#include "../menu/menu.h"

using namespace std::literals;
namespace ph = std::placeholders;

namespace ui {
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
    menu_.AddAction("ShowBook"s, "[title]"s, "Show book"s, std::bind(&View::ShowBook, this, ph::_1));
    menu_.AddAction("DeleteBook"s, "[title]"s, "Delete book"s, std::bind(&View::DeleteBook, this, ph::_1));
    menu_.AddAction("EditBook"s, "[title]"s, "Edit book"s, std::bind(&View::EditBook, this, ph::_1));
    menu_.AddAction("DeleteAuthor"s, "[name]"s, "Delete author"s, std::bind(&View::DeleteAuthor, this, ph::_1));
    menu_.AddAction("EditAuthor"s, "[name]"s, "Edit author"s, std::bind(&View::EditAuthor, this, ph::_1));
}

bool View::AddAuthor(std::istream& cmd_input) const {
    try {
        std::string name;
        std::getline(cmd_input, name);
        boost::algorithm::trim(name);
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
            int i = 1;

            for (const auto& book : GetAuthorBooks(*author_id)) {
                output_ << i++ << " "
                        << book.title << ", "
                        << book.publication_year
                        << std::endl;
            }
        }
    } catch (const std::exception&) {
        throw std::runtime_error("Failed to Show Books");
    }

    return true;
}

bool View::ShowBook(std::istream& cmd_input) const {
    std::string title;
    std::getline(cmd_input, title);
    boost::algorithm::trim(title);

    std::vector<detail::BookInfo> books = title.empty() ? GetBooks() : GetBooksByTitle(title);

    if (books.empty()) {
        return true;
    }

    std::optional<detail::BookInfo> selected;

    if (books.size() == 1) {
        selected = books.front();
    } else {
        selected = SelectBook(books);
    }

    if (selected) {
        PrintBookDetails(*selected);
    }

    return true;
}

bool View::DeleteAuthor(std::istream& cmd_input) const {
    std::string name;
    std::getline(cmd_input, name);
    boost::algorithm::trim(name);

    try {
        bool success = false;

        if (name.empty()) {
            if (auto author_id = SelectAuthor()) {
                success = use_cases_.DeleteAuthorById(*author_id);
            } else {
                return true;
            }
        } else {
            success = use_cases_.DeleteAuthorByName(name);
        }

        if (!success) {
            output_ << "Failed to delete author"sv << std::endl;
        }
    } catch (const std::exception&) {
        output_ << "Failed to delete author"sv << std::endl;
    }

    return true;
}

bool View::EditAuthor(std::istream& cmd_input) const {
    std::string name;
    std::getline(cmd_input, name);
    boost::algorithm::trim(name);

    try {
        std::optional<std::string> author_id;

        if (name.empty()) {
            author_id = SelectAuthor();
        } else {
            auto author = use_cases_.FindAuthorByName(name);
            if (author) {
                author_id = author->id;
            }
        }

        if (!author_id) {
            output_ << "Failed to edit author"sv << std::endl;
            return true;
        }

        output_ << "Enter new name:" << std::endl;

        std::string new_name;
        std::getline(input_, new_name);
        boost::algorithm::trim(new_name);

        if (!use_cases_.EditAuthor(*author_id, new_name)) {
            output_ << "Failed to edit author"sv << std::endl;
        }
    } catch (const std::exception&) {
        output_ << "Failed to edit author"sv << std::endl;
    }

    return true;
}

bool View::DeleteBook(std::istream& cmd_input) const {
    std::string title;
    std::getline(cmd_input, title);
    boost::algorithm::trim(title);

    try {
        std::vector<detail::BookInfo> books = title.empty() ? GetBooks() : GetBooksByTitle(title);

        if (books.empty()) {
            output_ << "Book not found"sv << std::endl;
            return true;
        }

        std::optional<detail::BookInfo> selected;

        if (books.size() == 1 && !title.empty()) {
            selected = books.front();
        } else {
            selected = SelectBook(books);
        }

        if (!selected) {
            return true;
        }

        if (!use_cases_.DeleteBook(selected->id)) {
            output_ << "Failed to delete book"sv << std::endl;
        }
    } catch (const std::exception&) {
        output_ << "Failed to delete book"sv << std::endl;
    }

    return true;
}

bool View::EditBook(std::istream& cmd_input) const {
    std::string title;
    std::getline(cmd_input, title);
    boost::algorithm::trim(title);

    try {
        std::vector<detail::BookInfo> books = title.empty() ? GetBooks() : GetBooksByTitle(title);

        if (books.empty()) {
            output_ << "Book not found"sv << std::endl;
            return true;
        }

        std::optional<detail::BookInfo> selected;

        if (!title.empty()) {
            selected = books.front();
        } else {
            selected = SelectBook(books);
        }

        if (!selected) {
            output_ << "Book not found"sv << std::endl;
            return true;
        }

        output_ << "Enter new title or empty line to use the current one (" << selected->title << "):" << std::endl;

        std::string new_title;
        std::getline(input_, new_title);
        boost::algorithm::trim(new_title);

        if (new_title.empty()) {
            new_title = selected->title;
        }

        output_ << "Enter publication year or empty line to use the current one ("
                << selected->publication_year << "):" << std::endl;

        std::string year_line;
        std::getline(input_, year_line);
        boost::algorithm::trim(year_line);

        int new_year = selected->publication_year;

        if (!year_line.empty()) {
            new_year = std::stoi(year_line);
        }

        std::string current_tags;

        for (size_t i = 0; i < selected->tags.size(); ++i) {
            if (i > 0) {
                current_tags += ", ";
            }
            current_tags += selected->tags[i];
        }

        auto tags = ReadTags(current_tags);

        if (!use_cases_.EditBook(selected->id, new_title, new_year, tags)) {
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
    boost::algorithm::trim(params.title);

    output_ << "Enter author name or empty line to select from list:" << std::endl;

    std::string author_name;
    std::getline(input_, author_name);
    boost::algorithm::trim(author_name);

    auto author_id = SelectAuthorByNameOrList(author_name);

    if (!author_id) {
    	return std::nullopt;
    }

    params.author_id = *author_id;

    params.tags = ReadTags();

    return params;
}

std::optional<std::string> View::SelectAuthorByNameOrList(std::string name) const {
    if (name.empty()) {
        return SelectAuthor();
    }

    auto author = use_cases_.FindAuthorByName(name);

    if (author) {
        return author->id;
    }

    output_ << "No author found. Do you want to add " << name << " (y/n)?" << std::endl;

    std::string answer;
    std::getline(input_, answer);

    if (answer != "Y" && answer != "y") {
        throw std::runtime_error("Failed to add author");
    }

    use_cases_.AddAuthor(name);

    author = use_cases_.FindAuthorByName(name);

    if (!author) {
        return std::nullopt;
    }

    return author->id;
}

std::optional<std::string> View::SelectAuthor() const {
    output_ << "Select author:" << std::endl;
    auto authors = GetAuthors();
    PrintVector(output_, authors);
    output_ << "Enter author # or empty line to cancel" << std::endl;

    std::string str;
    if (!std::getline(input_, str) || str.empty()) {
        return std::nullopt;
    }

    int author_idx = 0;

    try {
        author_idx = std::stoi(str);
    } catch (const std::exception&) {
        return std::nullopt;
    }

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

    if (!std::getline(input_, str) || str.empty()) {
        return std::nullopt;
    }

    int book_idx = 0;

    try {
        book_idx = std::stoi(str);
    } catch (const std::exception&) {
        return std::nullopt;
    }

    --book_idx;

    if (book_idx < 0 || static_cast<size_t>(book_idx) >= books.size()) {
        return std::nullopt;
    }

    return books[book_idx];
}

std::vector<std::string> View::ReadTags(const std::string& current_tags) const {
    if (current_tags.empty()) {
        output_ << "Enter tags (comma separated):" << std::endl;
    } else {
        output_ << "Enter tags (current tags: " << current_tags << "):" << std::endl;
    }

    std::string line;
    std::getline(input_, line);

    return ParseTags(line);
}

std::vector<std::string> View::ParseTags(const std::string& line) const {
    auto normalize = [](std::string tag) {
        boost::algorithm::trim(tag);

        std::string result;
        bool previous_space = false;

        for (unsigned char ch : tag) {
            if (std::isspace(ch)) {
                if (!previous_space) {
                    result.push_back(' ');
                    previous_space = true;
                }
            } else {
                result.push_back(static_cast<char>(ch));
                previous_space = false;
            }
        }

        return result;
    };

    std::vector<std::string> result;
    std::set<std::string> seen;

    std::stringstream stream(line);
    std::string raw_tag;

    while (std::getline(stream, raw_tag, ',')) {
        std::string tag = normalize(raw_tag);

        if (tag.empty()) {
            continue;
        }

        if (seen.insert(tag).second) {
            result.push_back(std::move(tag));
        }
    }

    std::sort(result.begin(), result.end());

    return result;
}

std::vector<detail::AuthorInfo> View::GetAuthors() const {
    std::vector<detail::AuthorInfo> result;

    for (const auto& author : use_cases_.GetAuthors()) {
        result.push_back({
            author.id,
            author.name
        });
    }

    return result;
}

std::vector<detail::BookInfo> View::GetBooks() const {
    std::vector<detail::BookInfo> result;

    for (const auto& book : use_cases_.GetBooks()) {
        result.push_back({
            book.id,
            book.title,
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
        output_ << "Tags: ";

        for (size_t i = 0; i < book.tags.size(); ++i) {
            if (i > 0) {
                output_ << ", ";
            }
            output_ << book.tags[i];
        }

        output_ << std::endl;
    }
}

}  // namespace ui
