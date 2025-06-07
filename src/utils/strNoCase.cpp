#include <string>
#include <cctype>

bool strcmpNocase(const std::string &a, const std::string &b) {
    std::size_t len = a.length();
    if (b.length() != len)
        return false;

    for (std::size_t i = 0; i < len; ++i) {
        char ca = std::tolower(a[i]);
        char cb = std::tolower(b[i]);
        if (ca != cb)
            return false;
    }
    return true;
}
