#ifndef DATE_H
#define DATE_H

#include <iostream>
#include <string>

// Simple calendar date used for transaction dates and due dates.
class Date {
    int day;
    int month;
    int year;

public:
    Date();                          // today's date
    Date(int d, int m, int y);       // explicit date (validated)

    Date addDays(int n) const;       // returns a new Date n days later
    std::string toString() const;    // YYYY-MM-DD

    int getDay() const { return day; }
    int getMonth() const { return month; }
    int getYear() const { return year; }

    friend std::ostream& operator<<(std::ostream& os, const Date& d);
};

#endif
