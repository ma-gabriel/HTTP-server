//
// Created by rperrot on 5/20/25.
//

#include "AAtributes.hpp"


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


