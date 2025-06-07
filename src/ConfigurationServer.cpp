//
// Created by rperrot on 5/22/25.
//

#include "ConfigurationServer.hpp"
#include <netdb.h>


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
    std::cout << *this << std::endl;
}

bool ConfigurationServer::addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end){
    if (AAtributes::addAttributes(it, end))
        return true;
    int n = this->getSizeLine(it, end);
    if (strcmpNocase(*it, "listen"))
        addListen(it, n);
    else if (strcmpNocase(*it, "server_name"))
        addServerName(it, n);
    else
        return false;
    return true;
}

void ConfigurationServer::addServerName(std::vector<std::string>::iterator &it, int n) {
    if (n < 2) {
        throw std::runtime_error("Server name requires at least one name.");
    }
    for (int i = 1; i < n; ++i) {
        this->_serverNames.push_back(*(++it));
    }
}

void ConfigurationServer::addListen(std::vector<std::string>::iterator &it, int n) {
    if (n != 3) {
        throw std::runtime_error("Server requires an address and a port.");
    }
    struct addrinfo hints = (struct addrinfo){};
    hints.ai_family = AF_INET; // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP
    if (getaddrinfo((*(++it)).c_str(), (*(++it)).c_str(), &hints, &this->_addr) != 0) {
        throw std::runtime_error("Invalid address or port in listen directive.");
    }
}


const addrinfo *ConfigurationServer::getAddr() const {
    return _addr;
}

const std::vector<std::string> &ConfigurationServer::getServerNames() const {
    return _serverNames;
}

const std::map<std::string, Location> &ConfigurationServer::getLocation() const {
    return _location;
}

std::ostream &operator<<(std::ostream &os, const ConfigurationServer &server) {
    os << "server name: " << std::endl;
    for (std::vector<std::string>::const_iterator it = server.getServerNames().begin(); it != server.getServerNames().end(); ++it) {
        os << "\t" << *it;
    }
    return os;
}