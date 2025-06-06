//
// Created by rperrot on 5/22/25.
//

#include "ConfigurationServer.hpp"


ConfigurationServer::ConfigurationServer(std::vector<std::string>::iterator &begin, const std::vector<std::string>::iterator &end) {
    if (++begin == end || *begin != "{")
        throw std::runtime_error("after server is not left brace ");
    while (++begin != end && *begin != "}") {
        if (*begin == "location")
        {
            Location location(begin, end);
            if (this->_location.find(location.getPath()) != this->_location.end()) {
                throw std::runtime_error("Location " + location.getPath() + " already exists.");
            }
            this->_location.insert(std::make_pair(location.getPath(), location));
        }
        else if  (*begin == "}")
            return;
        else if (!this->addAttributes(begin, end)) {
        }
    }
    if (begin == end || *begin != "}") {
        throw std::runtime_error("after server is not right brace ");
    }
    for (std::map<std::string, Location>::iterator it = this->_location.begin();
         it != this->_location.end();
         ++it)
    {
        std::cout << "dedebug location : " << std::endl;
        std::cout << it->second << std::endl;
    }
}

bool ConfigurationServer::addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end){
    if (AAtributes::addAttributes(it, end)) {
        return true;
    }
    int n = this->getSizeLine(it, end);
    if (n < 2)
        throw std::runtime_error("Error: " + *it + " requires a value.");
    if (strcmpNocase(*it, "listen"))
        return true;
    return false;

}


const sockaddr_in &ConfigurationServer::getAddr() const {
    return _addr;
}

void ConfigurationServer::setAddr(const sockaddr_in &addr) {
    _addr = addr;
}

const std::string &ConfigurationServer::getHostName() const {
    return _hostName;
}

void ConfigurationServer::setHostName(const std::string &hostName) {
    _hostName = hostName;
}
