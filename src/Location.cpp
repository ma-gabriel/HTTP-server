//
// Created by rperrot on 5/20/25.
//

#include "Location.hpp"

Location::~Location() {

}

Location::Location():Atributes(), _path("") {
    // std::cout << "Location default constructor called" << std::endl;
}

Location::Location(const Location &location):Atributes(location){
    this->_path = location._path;
}

Location &Location::operator=(const Location &location) {
    if (this != &location) {
        Atributes::operator=(location);  // Appel opérateur = de la classe parente
        this->_path = location._path;   // Copier les membres propres
    }
    return *this;
}



Location::Location(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end){
    if (++it == end)
        throw std::runtime_error("Path of location is the end of file ");
    this->_path = *it;
    correctPath();
    if (++it == end || *it != "{") {
        throw std::runtime_error("after location : " + this->_path + " is not left brace \n");
    }
    while (++it != end && *it != "}") {
        if (!this->addAttributes(it, end))
            throw  std::runtime_error("Unknow Attributes " + *it);
        if (*it == "}")
            return;
    }
    if (*it != "}")
        throw std::runtime_error("after location " + this->_path +" is not right brace\n");
//    std::cout << *this << std::endl;
}
const std::string &Location::getPath() const {
    return _path;
}

void Location::setPath(const std::string &path) {
    _path = path;
}
void Location::correctPath(){
    if (_path[0] != '/')
        throw StartNoWithSlashLocationException(this->_path);
    if (_path.find("//") != std::string::npos)
        throw  DoubleSlashExceptionLocation(this->_path);
}

