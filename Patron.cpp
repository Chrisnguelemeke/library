#include "Patron.h"

#include <algorithm>
#include <stdexcept>

Patron::Patron(std::string n, int i) : name(std::move(n)), id(i) {
    if (name.empty()) throw std::invalid_argument("Patron name cannot be empty.");
    if (id <= 0) throw std::invalid_argument("Patron ID must be a positive integer.");
}

void Patron::borrowBook(Book* b) {
    if (!b) throw std::invalid_argument("Cannot borrow a null book.");
    borrowedBooks.push_back(b);
}

void Patron::returnBook(Book* b) {
    auto it = std::find(borrowedBooks.begin(), borrowedBooks.end(), b);
    if (it == borrowedBooks.end())
        throw std::runtime_error("Patron did not borrow this book.");
    borrowedBooks.erase(it);
}

bool Patron::hasBook(const Book* b) const {
    return std::find(borrowedBooks.begin(), borrowedBooks.end(), b) != borrowedBooks.end();
}

void Patron::displayPatron() const {
    std::cout << "ID " << id << ": " << name << " - ";
    if (borrowedBooks.empty()) {
        std::cout << "no books borrowed\n";
        return;
    }
    std::cout << "borrowed: ";
    for (std::vector<Book*>::const_iterator it = borrowedBooks.begin();
         it != borrowedBooks.end(); ++it) {
        if (it != borrowedBooks.begin()) std::cout << "; ";
        std::cout << "\"" << (*it)->getTitle() << "\"";
    }
    std::cout << "\n";
}

bool Patron::operator==(const Patron& other) const { return id == other.id; }

std::ostream& operator<<(std::ostream& os, const Patron& p) {
    return os << "Patron #" << p.id << " (" << p.name << ")";
}
