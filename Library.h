#ifndef LIBRARY_H
#define LIBRARY_H

#include <map>
#include <string>
#include <vector>

#include "Book.h"
#include "Patron.h"
#include "Transaction.h"

class Library {
    std::vector<Book*> books;                 // Library owns these (deleted in destructor)
    std::vector<Patron> patrons;
    std::vector<Transaction> transactions;
    std::map<std::string, Book*> bookIndex;   // lowercase title -> book
    std::map<int, Patron*> patronIndex;       // patron id -> patron

    std::string booksFile, patronsFile, logFile;

    void rebuildPatronIndex();
    Patron& findPatron(int patronId);
    Book& findBook(const std::string& title);

public:
    Library(std::string booksF = "books.txt", std::string patronsF = "patrons.txt",
            std::string logF = "transactions.txt");
    ~Library();
    Library(const Library&) = delete;              // raw pointers: no copying
    Library& operator=(const Library&) = delete;

    void loadData();
    void saveData();

    void addBook(Book* b);                         // takes ownership
    void addPatron(const Patron& p);
    void checkoutBook(int patronId, const std::string& title);
    void returnBook(int patronId, const std::string& title);

    void searchByAuthor(const std::string& author) const;
    void searchByGenre(const std::string& genreText) const;

    void displayBooks() const;
    void displayPatrons() const;
    void displayTransactions() const;
};

#endif
