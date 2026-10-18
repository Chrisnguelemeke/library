#ifndef BOOK_H
#define BOOK_H

#include <iostream>
#include <string>

enum class Genre { Fiction, NonFiction, Mystery, Science, Biography };
enum class BookStatus { Available, CheckedOut };

// Conversions between enums and text (used for display and file I/O).
std::string genreToString(Genre g);
Genre stringToGenre(const std::string& s);   // throws std::invalid_argument
std::string statusToString(BookStatus s);

class Book {
protected:
    std::string title;
    std::string author;
    Genre genre;
    BookStatus status;

public:
    Book(std::string t, std::string a, Genre g);
    virtual ~Book() = default;

    virtual void displayInfo() const;
    virtual std::string typeName() const { return "Book"; }
    virtual std::string detailField() const { return "0"; }   // pages or size (for files)

    void setStatus(BookStatus s);
    BookStatus getStatus() const;
    const std::string& getTitle() const { return title; }
    const std::string& getAuthor() const { return author; }
    Genre getGenre() const { return genre; }

    bool operator==(const Book& other) const;
    friend std::ostream& operator<<(std::ostream& os, const Book& b);
};

class EBook : public Book {
    double fileSizeMB;

public:
    EBook(std::string t, std::string a, Genre g, double size);
    void displayInfo() const override;
    std::string typeName() const override { return "EBook"; }
    std::string detailField() const override;
};

class PrintedBook : public Book {
    int pageCount;

public:
    PrintedBook(std::string t, std::string a, Genre g, int pages);
    void displayInfo() const override;
    std::string typeName() const override { return "Printed"; }
    std::string detailField() const override { return std::to_string(pageCount); }
};

#endif
