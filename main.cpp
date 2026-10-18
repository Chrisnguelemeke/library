#include <iostream>
#include <limits>
#include <stdexcept>
#include <string>

#include "Library.h"

static std::string prompt(const std::string& label) {
    std::cout << label;
    std::string s;
    if (!std::getline(std::cin, s)) throw std::runtime_error("Input closed.");
    return s;
}

// Reads an integer; throws std::invalid_argument on bad input.
static int readInt(const std::string& label) {
    std::string s = prompt(label);
    size_t pos = 0;
    int v = 0;
    try {
        v = std::stoi(s, &pos);
    } catch (const std::exception&) {
        throw std::invalid_argument("\"" + s + "\" is not a valid whole number.");
    }
    if (pos != s.size()) throw std::invalid_argument("\"" + s + "\" is not a valid whole number.");
    return v;
}

static void showMenu() {
    std::cout << "\n===== Library Management System =====\n"
              << "1. View all books\n"
              << "2. View all patrons\n"
              << "3. Check out a book\n"
              << "4. Return a book\n"
              << "5. Add a new book\n"
              << "6. Add a new patron\n"
              << "7. Search books by author\n"
              << "8. Search books by genre\n"
              << "9. Save and exit\n";
}

static void addBookInteractive(Library& library) {
    std::string genreText = prompt("Genre (Fiction, NonFiction, Mystery, Science, Biography): ");
    Genre genre = stringToGenre(genreText);
    std::string title = prompt("Title: ");
    std::string author = prompt("Author: ");
    std::string type = prompt("Format (Printed or EBook): ");
    Book* b = nullptr;
    if (type == "Printed" || type == "printed") {
        b = new PrintedBook(title, author, genre, readInt("Page count: "));
    } else if (type == "EBook" || type == "ebook") {
        std::string sizeText = prompt("File size in MB: ");
        size_t pos = 0;
        double size = 0;
        try { size = std::stod(sizeText, &pos); }
        catch (const std::exception&) { throw std::invalid_argument("File size must be a number."); }
        b = new EBook(title, author, genre, size);
    } else {
        throw std::invalid_argument("Format must be Printed or EBook.");
    }
    library.addBook(b);                       // Library takes ownership
    std::cout << "Added: " << *b << "\n";
}

int main() {
    Library library;
    try {
        library.loadData();
        std::cout << "Data loaded successfully.\n";
    } catch (const std::exception& e) {
        std::cerr << "Application Error: " << e.what() << std::endl;
        return 1;
    }

    int choice = 0;
    do {
        showMenu();
        try {
            choice = readInt("Enter choice: ");
            switch (choice) {
                case 1: library.displayBooks(); break;
                case 2: library.displayPatrons(); break;
                case 3: {
                    int id = readInt("Patron ID: ");
                    std::string title = prompt("Book title: ");
                    library.checkoutBook(id, title);
                    break;
                }
                case 4: {
                    int id = readInt("Patron ID: ");
                    std::string title = prompt("Book title: ");
                    library.returnBook(id, title);
                    break;
                }
                case 5: addBookInteractive(library); break;
                case 6: {
                    std::string name = prompt("Patron name: ");
                    int id = readInt("Patron ID: ");
                    library.addPatron(Patron(name, id));
                    std::cout << "Added: " << Patron(name, id) << "\n";
                    break;
                }
                case 7: library.searchByAuthor(prompt("Author (full or partial name): ")); break;
                case 8: library.searchByGenre(prompt("Genre: ")); break;
                case 9:
                    library.saveData();
                    std::cout << "Data saved. Goodbye!\n";
                    break;
                default:
                    std::cout << "Please choose a number from 1 to 9.\n";
            }
        } catch (const std::runtime_error& e) {
            if (std::string(e.what()) == "Input closed.") break;
            std::cerr << "Error: " << e.what() << std::endl;
        } catch (const std::exception& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    } while (choice != 9);

    return 0;
}
