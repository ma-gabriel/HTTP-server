#include "iostream"

bool strIsDigit(const std::string &str) {
    for (int i = 0; i < static_cast<int>(str.length()); ++i) {
        if (!std::isdigit(static_cast<unsigned char>(str[i]))) {
            return false;
        }
    }
    return !str.empty();
}
