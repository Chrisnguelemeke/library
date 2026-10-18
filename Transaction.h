#ifndef TRANSACTION_H
#define TRANSACTION_H

#include <string>

#include "Date.h"

class Transaction {
    int patronId;
    std::string bookTitle;
    std::string type;     // "CHECKOUT" or "RETURN"
    Date date;
    Date dueDate;         // meaningful for CHECKOUT; equals date for RETURN

public:
    Transaction(int pid, std::string bt, std::string type, int loanDays = 14);

    void displayTransaction() const;
    std::string toCsv() const;
    const Date& getDueDate() const { return dueDate; }
};

#endif
