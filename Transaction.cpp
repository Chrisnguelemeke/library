#include "Transaction.h"

#include <iostream>

Transaction::Transaction(int pid, std::string bt, std::string t, int loanDays)
    : patronId(pid), bookTitle(std::move(bt)), type(std::move(t)),
      date(), dueDate(type == "CHECKOUT" ? date.addDays(loanDays) : date) {}

void Transaction::displayTransaction() const {
    std::cout << date << "  " << type << "  patron " << patronId
              << "  \"" << bookTitle << "\"";
    if (type == "CHECKOUT") std::cout << "  due " << dueDate;
    std::cout << "\n";
}

std::string Transaction::toCsv() const {
    return std::to_string(patronId) + "," + bookTitle + "," + type + "," +
           date.toString() + "," + dueDate.toString();
}
