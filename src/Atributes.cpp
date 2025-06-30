//
// Created by rperrot on 5/20/25.
//

#include "Atributes.hpp"
#include <algorithm>
#include <iostream>
#include <string>


Atributes::Atributes(): _autoIndex(Null), _maxBodySize(0){}

Atributes::~Atributes() {}

Atributes::Atributes(const Atributes &a)
{
    this->_autoIndex = a._autoIndex;
    this->_maxBodySize = a._maxBodySize;
    this->_cgi = a._cgi;
    this->_autoIndex = a._autoIndex;
    this->_httpMethode = a._httpMethode;
    this->_errorPages = a._errorPages;
    this->_root = a._root;
    this->_index = a._index;
}

Atributes &Atributes::operator=(const Atributes &a) {
    if (this == &a)
        return *this;
    this->_autoIndex = a._autoIndex;
    this->_maxBodySize = a._maxBodySize;
    this->_cgi = a._cgi;
    this->_httpMethode = a._httpMethode;
    this->_errorPages = a._errorPages;
    this->_root = a._root;
    this->_index = a._index;
    return *this;

}

const std::string &Atributes::getRoot() const
{
    return _root;
}

const std::map<int, std::string> &Atributes::getErrorPages() const
{
    return _errorPages;
}

const std::vector<EHttpMethode> &Atributes::getHttpMethode() const
{
    return _httpMethode;
}


const std::vector<std::string> &Atributes::getIndex() const
{
    return _index;
}

bool Atributes::isAutoIndex() const
{
    return _autoIndex;
}

char toLowerChar(char c) {
    return std::tolower(static_cast<unsigned char>(c));
}
bool Atributes::addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end)
{
    typedef void (Atributes::*Action)(std::vector<std::string>::iterator &, int);
    std::map<std::string, Action> actions;
    actions["root"] = &Atributes::addRoot;
    actions["autoindex"] = &Atributes::addAutoIndex;
    actions["index"] = &Atributes::addIndex;
    actions["error_page"] = &Atributes::addErrorPages;
    actions["limit_except"] = &Atributes::addHttpIndex;
    actions["client_max_body_size"] = &Atributes::addClientMaxBodySize;
    actions["cgi"] = &Atributes::addCgi;

    std::transform(it->begin(), it->end(), it->begin(),
                  toLowerChar);
    if (it == end) {
        return false;
    }
    if (actions.find(*it) == actions.end()) {
        return false;
    }
    Action itAction = actions[*it];
    int n = getSizeLine(it, end);
    (this->*itAction)(it, n);
    it++;
    return true;
}

void Atributes::addRoot(std::vector<std::string>::iterator &it, int n) {
    if (!this->_root.empty()) {
        throw std::runtime_error("Root attribute already set.");
    }
    if (n != 2) {
       throw std::runtime_error("Root attribute requires a value.");
    }
    this->_root = (*(++it));
}

void Atributes::addClientMaxBodySize(std::vector<std::string>::iterator &it, int n) {
    if (this->_maxBodySize != 0)
        throw std::runtime_error("max_body_size already set.");
    if (n != 2) {
        throw std::runtime_error("Root attribute requires a value.");
    }
    if (!strIsDigit(*(++it))) {
        throw std::runtime_error("Client max body size must be a digit.");
    }
    double maxBodySizeDouble = std::strtod((*it).c_str(), NULL);
    this ->_maxBodySize = static_cast<int>(maxBodySizeDouble);
}

void Atributes::addIndex(std::vector<std::string>::iterator &it, int n) {
    if (!this->_index.empty())
        throw std::runtime_error("index already set.");
   if (n == 1) {
       throw std::runtime_error("index attribute requires a value.");
   }
   std::vector<std::string> index;
    for (int i = 1; i < n; ++i) {
         index.push_back(*(++it));
    }
    this->_index = index;
}

void Atributes::addCgi(std::vector<std::string>::iterator &it, int n) {
   if (!this->_cgi.empty())
       throw std::runtime_error("Cgi attribute already set.");
   if (n == 1) {
       throw std::runtime_error("Cgi attribute requires least one value.");
   }
   std::vector<std::string> cgi;
    for (int i = 1; i < n; ++i) {
        cgi.push_back(*(++it));
    }
    this->_cgi = cgi;
}

void Atributes::addAutoIndex(std::vector<std::string>::iterator &it, int n) {
    if (this->_autoIndex != Null) {
        throw std::runtime_error("AutoIndex already set.");
    }
    if (n != 2) {
        throw std::runtime_error("AutoIndex requires on or off.");
    }
    if (strcmpNocase(*(++it), "on")) {
        this->_autoIndex = TRUE;
    } else if (strcmpNocase(*(it), "off")) {
        this->_autoIndex = FALSE;
    } else {
        throw std::runtime_error("AutoIndex attribute must be 'on' or 'off'.");
    }
}

void Atributes::addHttpIndex(std::vector<std::string>::iterator &it, int n) {
    if (!this->_httpMethode.empty())
        throw std::runtime_error("limit_except already set");
    for (int i = 1; i < n; ++i) {
        if (strcmpNocase(*(++it), "get")) {
            this->_httpMethode.push_back(Get);
        } else if (strcmpNocase(*(it), "post")) {
            this->_httpMethode.push_back(Post);
        } else if (strcmpNocase(*(it), "delete")) {
            this->_httpMethode.push_back(Delete);
        } else if (strcmpNocase(*(it), "put")) {
            this->_httpMethode.push_back(Put);
        } else {
            throw std::runtime_error("HTTP method must be 'GET', 'PUT', 'POST' or 'DELETE'.");
        }
    }
}

void Atributes::addErrorPages(std::vector<std::string>::iterator &it, int n) {
    std::vector<int> errorCodes;
    if (n < 3 ) {
        throw std::runtime_error("Error pages attribute requires pairs of error code and page.");
    }
    for (int i = 1; i < n - 1; ++i) {
        if (!strIsDigit(*(++it))) {
            throw std::runtime_error("Error code must be a digit.");
        }
        double errorDouble = std::strtod((*it).c_str(), NULL);
        int errorCode = static_cast<int>(errorDouble);
        if (errorCode < 100 || errorCode > 599) {
            throw std::runtime_error("Error code must be between 100 and 599.");
        }
        if (this->_errorPages.find(errorCode) != this->_errorPages.end()) {
            throw std::runtime_error("Duplicate error code: " + *it);
        }
        errorCodes.push_back(errorCode);
    }
    it++;
    for (std::vector<int>::iterator itError = errorCodes.begin(); itError != errorCodes.end(); ++itError) {
        this->_errorPages.insert(std::make_pair(*itError, *it));
    }
}

int Atributes::getSizeLine(std::vector<std::string>::iterator it, const std::vector<std::string>::iterator &end) {
    int n = 0;
    while (it != end && *it != ";") {
        it++;
        n++;
    }
    if (it == end || *it != ";") {
        throw std::runtime_error("Expected ';' at the end of the line.");
    }
    return n;
}

const std::vector<std::string> &Atributes::getCgi() const {
    return _cgi;
}

int Atributes::getMaxBodySize() const {
    return _maxBodySize;
}

std::ostream &operator<<(std::ostream &os, const Atributes &attributes) {
    os << "Root: " << attributes.getRoot() << "\n";
    os << "AutoIndex: " << (attributes.isAutoIndex() ? "on" : "off") << "\n";
    os << "Error Pages:\n";
    for (std::map<int, std::string>::const_iterator  it = attributes.getErrorPages().begin();
         it != attributes.getErrorPages().end(); ++it) {
        os << "\t" << it->first << ": " << it->second << "\n";
    }
    os << "HTTP Methods: " << attributes.getHttpMethode().size() << " " ;
    for (std::vector<EHttpMethode>::const_iterator it = attributes.getHttpMethode().begin(); it != attributes.getHttpMethode().end(); ++it) {
        os << getMethodString(*it) << " ";
    }
    os << "\nIndex: ";
    for (std::vector<std::string>::const_iterator it = attributes.getIndex().begin(); it != attributes.getIndex().end(); ++it) {
        os << *it << " ";
    }
    os << "\ncgi: ";
    for (std::vector<std::string>::const_iterator it = attributes.getCgi().begin(); it != attributes.getCgi().end(); ++it) {
        os << *it << " ";
    }
    os << "\nMax Body Size: " << attributes.getMaxBodySize() << " bytes";
    os << "\n";
    return os;
}

void Atributes::fillAtributes(Atributes &atributes) {
    if (this->_root.empty())
        this->_root = atributes._root;
    if (this->_errorPages.empty())
        this->_errorPages = atributes._errorPages;
    if (this->_httpMethode.empty())
        this->_httpMethode = atributes._httpMethode;
    if (this->_autoIndex == Null)
        this->_autoIndex = atributes._autoIndex;
    if (this->_maxBodySize == 0)
        this->_maxBodySize = atributes._maxBodySize;
    if (this->_cgi.empty())
        this->_cgi = atributes._cgi;
    if (this->_index.empty())
        this->_index = atributes._index;
}