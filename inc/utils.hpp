

#ifndef WEBSERV2_UTILS_HPP
#define WEBSERV2_UTILS_HPP

bool        isSeparator(const char c);
bool        strcmpNocase(const std::string &a, const std::string &b);
bool        strIsDigit(const std::string &str);
bool        check_args(int argc, char **argv);
std::string toLower(const std::string& s);
size_t      findInsensitive(const std::string& haystack, const std::string& needle);
#endif // WEBSERV2_UTILS_HPP

