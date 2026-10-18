#ifndef PATRON_H
#define PATRON_H

#include <iostream>
#include <string>
#include <vector>

#include "Book.h"

class Patron {
    std::string name;
    int id;
    std::vector<Book*> borrowedBooks;   // non-owning; Library owns the books

public:
    Patron(std::string n, int i);

    void borrowBook(Book* b);
    void returnBook(Book* b);           // throws if the patron does not hold the book
    bool hasBook(const Book* b) const;
    void displayPatron() const;

    int getId() const { return id; }
    const std::string& getName() const { return name; }
    const std::vector<Book*>& getBorrowed() const { return borrowedBooks; }

    bool operator==(const Patron& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Patron& p);
};

#endif
