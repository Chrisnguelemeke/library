#include "Library.h"

#include <algorithm>
#include <cctype>
#include <iostream>
#include <sstream>
#include <stdexcept>

#include "FileIO.h"

static std::string lower(std::string s) {
    std::transform(s.begin(), s.end(), s.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return s;
}

static std::vector<std::string> split(const std::string& line, char delim = ',') {
    std::vector<std::string> parts;
    std::string field;
    std::istringstream ss(line);
    while (std::getline(ss, field, delim)) parts.push_back(field);
    return parts;
}

Library::Library(std::string booksF, std::string patronsF, std::string logF)
    : booksFile(std::move(booksF)), patronsFile(std::move(patronsF)), logFile(std::move(logF)) {}

Library::~Library() {
    for (Book* b : books) delete b;
}

void Library::rebuildPatronIndex() {
    patronIndex.clear();
    for (Patron& p : patrons) patronIndex[p.getId()] = &p;
}

Patron& Library::findPatron(int patronId) {
    auto it = patronIndex.find(patronId);
    if (it == patronIndex.end())
        throw std::invalid_argument("Invalid patron ID: " + std::to_string(patronId) + ".");
    return *it->second;
}

Book& Library::findBook(const std::string& title) {
    auto it = bookIndex.find(lower(title));
    if (it == bookIndex.end())
        throw std::runtime_error("Book not found: \"" + title + "\".");
    return *it->second;
}

// ---------- file I/O ----------
void Library::loadData() {
    // Patrons line format: id,name
    std::vector<Patron> loadedPatrons = loadFromFile<Patron>(patronsFile, [](const std::string& line) {
        std::vector<std::string> f = split(line);
        if (f.size() != 2) throw std::invalid_argument("expected id,name");
        return Patron(f[1], std::stoi(f[0]));
    });
    for (const Patron& p : loadedPatrons) addPatron(p);

    // Books line format: Genre,Title,Author,Printed|EBook,PagesOrSizeMB[,CheckedOut,patronId]
    std::vector<Book*> loadedBooks = loadFromFile<Book*>(booksFile, [](const std::string& line) -> Book* {
        std::vector<std::string> f = split(line);
        if (f.size() != 5 && f.size() != 7)
            throw std::invalid_argument("expected Genre,Title,Author,Type,Value");
        Genre g = stringToGenre(f[0]);
        if (f[3] == "Printed") return new PrintedBook(f[1], f[2], g, std::stoi(f[4]));
        if (f[3] == "EBook")   return new EBook(f[1], f[2], g, std::stod(f[4]));
        throw std::invalid_argument("book type must be Printed or EBook");
    });

    // Re-read the optional status fields and attach checked-out books to patrons.
    std::vector<std::string> lines = loadFromFile<std::string>(booksFile,
        [](const std::string& line) { return line; });
    for (size_t i = 0; i < loadedBooks.size(); ++i) {
        try {
            addBook(loadedBooks[i]);
        } catch (...) {
            for (size_t j = i + 1; j < loadedBooks.size(); ++j) delete loadedBooks[j];
            throw;
        }
        std::vector<std::string> f = split(lines[i]);
        if (f.size() == 7 && f[5] == "CheckedOut") {
            Patron& p = findPatron(std::stoi(f[6]));
            loadedBooks[i]->setStatus(BookStatus::CheckedOut);
            p.borrowBook(loadedBooks[i]);
        }
    }
}

void Library::saveData() {
    // Which patron holds each checked-out book?
    std::map<const Book*, int> holder;
    for (const Patron& p : patrons)
        for (const Book* b : p.getBorrowed()) holder[b] = p.getId();

    saveToFile<Book*>(booksFile, books, [&holder](const Book* b) {
        std::string line = genreToString(b->getGenre()) + "," + b->getTitle() + "," +
                           b->getAuthor() + "," + b->typeName() + "," + b->detailField();
        if (b->getStatus() == BookStatus::CheckedOut && holder.count(b))
            line += ",CheckedOut," + std::to_string(holder.at(b));
        return line;
    });
    saveToFile<Patron>(patronsFile, patrons, [](const Patron& p) {
        return std::to_string(p.getId()) + "," + p.getName();
    });
}

// ---------- mutators ----------
void Library::addBook(Book* b) {
    if (!b) throw std::invalid_argument("Cannot add a null book.");
    std::string key = lower(b->getTitle());
    if (bookIndex.count(key)) {
        delete b;
        throw std::runtime_error("A book with that title already exists.");
    }
    books.push_back(b);
    bookIndex[key] = b;
}

void Library::addPatron(const Patron& p) {
    if (patronIndex.count(p.getId()))
        throw std::runtime_error("A patron with ID " + std::to_string(p.getId()) + " already exists.");
    patrons.push_back(p);
    rebuildPatronIndex();      // vector may have reallocated
}

void Library::checkoutBook(int patronId, const std::string& title) {
    Patron& patron = findPatron(patronId);
    Book& book = findBook(title);
    if (book.getStatus() == BookStatus::CheckedOut)
        throw std::runtime_error("Book is already checked out.");

    book.setStatus(BookStatus::CheckedOut);
    patron.borrowBook(&book);

    Transaction t(patronId, book.getTitle(), "CHECKOUT");
    transactions.push_back(t);
    appendLine(logFile, t.toCsv());
    std::cout << patron.getName() << " checked out \"" << book.getTitle()
              << "\". Due date: " << t.getDueDate() << "\n";
}

void Library::returnBook(int patronId, const std::string& title) {
    Patron& patron = findPatron(patronId);
    Book& book = findBook(title);
    patron.returnBook(&book);              // throws if this patron does not hold it
    book.setStatus(BookStatus::Available);

    Transaction t(patronId, book.getTitle(), "RETURN");
    transactions.push_back(t);
    appendLine(logFile, t.toCsv());
    std::cout << patron.getName() << " returned \"" << book.getTitle() << "\".\n";
}

// ---------- queries ----------
void Library::searchByAuthor(const std::string& author) const {
    std::string key = lower(author);
    int found = 0;
    for (const Book* b : books) {
        if (lower(b->getAuthor()).find(key) != std::string::npos) {
            b->displayInfo();
            ++found;
        }
    }
    if (!found) std::cout << "No books found for author \"" << author << "\".\n";
}

void Library::searchByGenre(const std::string& genreText) const {
    Genre g = stringToGenre(genreText);    // throws on invalid genre
    int found = 0;
    for (const Book* b : books) {
        if (b->getGenre() == g) {
            b->displayInfo();
            ++found;
        }
    }
    if (!found) std::cout << "No books found in genre " << genreToString(g) << ".\n";
}

void Library::displayBooks() const {
    if (books.empty()) { std::cout << "No books in the library.\n"; return; }
    for (const Book* b : books) b->displayInfo();     // polymorphic call
}

void Library::displayPatrons() const {
    if (patrons.empty()) { std::cout << "No patrons registered.\n"; return; }
    for (const Patron& p : patrons) p.displayPatron();
}

void Library::displayTransactions() const {
    if (transactions.empty()) { std::cout << "No transactions this session.\n"; return; }
    for (const Transaction& t : transactions) t.displayTransaction();
}
