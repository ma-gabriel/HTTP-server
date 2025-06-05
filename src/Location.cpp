//
// Created by rperrot on 5/20/25.
//

#include "Location.hpp"

Location::~Location() {

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
        this->addAttributes(it, end);
        if (*it == "}")
            return;
    }
    if (*it != "}")
        throw std::runtime_error("after location " + this->_path +" is not right brace\n");
    std::cout << "Location created with path: " << this->_path << "and root " << _root << std::endl;
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

