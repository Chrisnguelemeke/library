#include "Date.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <stdexcept>

Date::Date() {
    std::time_t now = std::time(nullptr);
    std::tm* t = std::localtime(&now);
    day = t->tm_mday;
    month = t->tm_mon + 1;
    year = t->tm_year + 1900;
}

Date::Date(int d, int m, int y) : day(d), month(m), year(y) {
    if (y < 1900 || m < 1 || m > 12 || d < 1 || d > 31)
        throw std::invalid_argument("Invalid date.");
}

Date Date::addDays(int n) const {
    std::tm t = {};
    t.tm_mday = day + n;      // mktime normalizes overflow into month/year
    t.tm_mon = month - 1;
    t.tm_year = year - 1900;
    t.tm_hour = 12;
    std::mktime(&t);
    return Date(t.tm_mday, t.tm_mon + 1, t.tm_year + 1900);
}

std::string Date::toString() const {
    std::ostringstream os;
    os << std::setfill('0') << std::setw(4) << year << '-'
       << std::setw(2) << month << '-' << std::setw(2) << day;
    return os.str();
}

std::ostream& operator<<(std::ostream& os, const Date& d) {
    return os << d.toString();
}
