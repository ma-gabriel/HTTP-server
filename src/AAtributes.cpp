//
// Created by rperrot on 5/20/25.
//

#include "AAtributes.hpp"
#include <functional>


AAtributes::AAtributes():_autoIndex(false)
{
}

AAtributes::~AAtributes()
{

}

AAtributes::AAtributes(const std::string &root, const std::map<int, std::string> &error_pages, const std::vector<EHttpMethode> &http_methode, const bool auto_index, const std::vector<std::string> &index)
        : _root(root), _errorPages(error_pages), _httpMethode(http_methode), _autoIndex(auto_index), _index(index)
{}

const std::string &AAtributes::getRoot() const
{
    return _root;
}

const std::map<int, std::string> &AAtributes::getErrorPages() const
{
    return _errorPages;
}

const std::vector<EHttpMethode> &AAtributes::getHttpMethode() const
{
    return _httpMethode;
}


const std::vector<std::string> &AAtributes::getIndex() const
{
    return _index;
}

void AAtributes::setRoot(const std::string &root)
{
    _root = root;
}

void AAtributes::setHttpMethode(const std::vector<EHttpMethode> &httpMethode)
{
    _httpMethode = httpMethode;
}

void AAtributes::setErrorPages(const std::map<int, std::string> &errorPages)
{
    _errorPages = errorPages;
}

void AAtributes::setIndex(const std::vector<std::string> &index)
{
    _index = index;
}

void AAtributes::setAutoIndex(bool autoIndex)
{
    _autoIndex = autoIndex;
}

bool AAtributes::isAutoIndex() const
{
    return _autoIndex;
}

bool AAtributes::addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end)
{
    typedef void (AAtributes::*Action)(std::vector<std::string>::iterator &, const std::vector<std::string>::iterator &);
    std::map<std::string, Action> actions;
    actions["root"] = &AAtributes::addRoot;
    if (it == end) {
        std::cerr << "Error: No attributes found." << std::endl;
        return false;
    }
    if (actions.find(*it) == actions.end()) {
        return false;
    }
    Action itAction = actions[*it];
    (this->*itAction)(it, end);
    return true;
}

void AAtributes::addRoot(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end){
    if (it == end) {
        std::cerr << "Error: No root path provided." << std::endl;
    }
    if (++it == end || *it == "{" || *it == "}") {
        throw std::runtime_error("Is the end of the file.");
    }
    this->_root = *it;
    if (++it == end || *it != ";") {
        throw std::runtime_error("Expected ';' after root path.");
    }
}

