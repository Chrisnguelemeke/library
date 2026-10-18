#ifndef FILEIO_H
#define FILEIO_H

#include <fstream>
#include <stdexcept>
#include <string>
#include <vector>

// Function template: write each item of a collection as one line of text.
// toLine converts an item to its line (a function, lambda, or functor).
template <typename T, typename ToLine>
void saveToFile(const std::string& filename, const std::vector<T>& items, ToLine toLine) {
    std::ofstream out(filename);
    if (!out) throw std::runtime_error("Failed to open " + filename + " for writing.");
    for (const T& item : items) out << toLine(item) << "\n";
}

// Function template: read a text file line by line and build a collection.
// parse converts one non-empty line to a T and may throw on bad data.
template <typename T, typename Parse>
std::vector<T> loadFromFile(const std::string& filename, Parse parse) {
    std::ifstream in(filename);
    if (!in) throw std::runtime_error("Failed to open " + filename + ".");
    std::vector<T> items;
    std::string line;
    int lineNo = 0;
    while (std::getline(in, line)) {
        ++lineNo;
        if (!line.empty() && line.back() == '\r') line.pop_back();
        if (line.empty()) continue;
        try {
            items.push_back(parse(line));
        } catch (const std::exception& e) {
            throw std::runtime_error(filename + " line " + std::to_string(lineNo) + ": " + e.what());
        }
    }
    return items;
}

// Append one line to a text file (used for the transaction log).
inline void appendLine(const std::string& filename, const std::string& line) {
    std::ofstream out(filename, std::ios::app);
    if (!out) throw std::runtime_error("Failed to open " + filename + " for appending.");
    out << line << "\n";
}

#endif
