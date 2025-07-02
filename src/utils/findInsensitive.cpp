#include <iostream>
#include <algorithm>
std::string toLower(const std::string& s) {
    std::string res = s;
    std::transform(res.begin(), res.end(), res.begin(), ::tolower);
    return res;
}

// Recherche insensible à la casse
size_t findInsensitive(const std::string& haystack, const std::string& needle) {
    std::string haystackLower = toLower(haystack);
    std::string needleLower = toLower(needle);
    return haystackLower.find(needleLower);
}
