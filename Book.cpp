#include "Book.h"

#include <algorithm>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <stdexcept>

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

std::string genreToString(Genre g) {
    switch (g) {
        case Genre::Fiction:    return "Fiction";
        case Genre::NonFiction: return "NonFiction";
        case Genre::Mystery:    return "Mystery";
        case Genre::Science:    return "Science";
        case Genre::Biography:  return "Biography";
    }
    return "Unknown";
}

Genre stringToGenre(const std::string& s) {
    std::string k = lower(s);
    if (k == "fiction")    return Genre::Fiction;
    if (k == "nonfiction") return Genre::NonFiction;
    if (k == "mystery")    return Genre::Mystery;
    if (k == "science")    return Genre::Science;
    if (k == "biography")  return Genre::Biography;
    throw std::invalid_argument("Invalid genre: \"" + s + "\"");
}

std::string statusToString(BookStatus s) {
    return s == BookStatus::Available ? "Available" : "Checked Out";
}

// ---------- Book ----------
Book::Book(std::string t, std::string a, Genre g)
    : title(std::move(t)), author(std::move(a)), genre(g), status(BookStatus::Available) {
    if (title.empty() || author.empty())
        throw std::invalid_argument("Book title and author cannot be empty.");
}

void Book::displayInfo() const {
    std::cout << "\"" << title << "\" by " << author
              << " [" << genreToString(genre) << "] - " << statusToString(status) << "\n";
}

void Book::setStatus(BookStatus s) { status = s; }
BookStatus Book::getStatus() const { return status; }

bool Book::operator==(const Book& other) const {
    return lower(title) == lower(other.title) && lower(author) == lower(other.author);
}

std::ostream& operator<<(std::ostream& os, const Book& b) {
    return os << b.title << " by " << b.author << " (" << genreToString(b.genre) << ")";
}

// ---------- EBook ----------
EBook::EBook(std::string t, std::string a, Genre g, double size)
    : Book(std::move(t), std::move(a), g), fileSizeMB(size) {
    if (size <= 0) throw std::invalid_argument("File size must be positive.");
}

void EBook::displayInfo() const {
    std::cout << "[EBook]   \"" << title << "\" by " << author
              << " [" << genreToString(genre) << "] " << std::fixed << std::setprecision(1)
              << fileSizeMB << " MB - " << statusToString(status) << "\n";
    std::cout.unsetf(std::ios::floatfield);
}

std::string EBook::detailField() const {
    std::ostringstream os;
    os << fileSizeMB;
    return os.str();
}

// ---------- PrintedBook ----------
PrintedBook::PrintedBook(std::string t, std::string a, Genre g, int pages)
    : Book(std::move(t), std::move(a), g), pageCount(pages) {
    if (pages <= 0) throw std::invalid_argument("Page count must be positive.");
}

void PrintedBook::displayInfo() const {
    std::cout << "[Printed] \"" << title << "\" by " << author
              << " [" << genreToString(genre) << "] " << pageCount
              << " pages - " << statusToString(status) << "\n";
}
