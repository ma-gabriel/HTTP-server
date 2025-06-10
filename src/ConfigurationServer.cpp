//
// Created by rperrot on 5/22/25.
//

#include "ConfigurationServer.hpp"
#include <netdb.h>
#include <cstdlib>


ConfigurationServer::ConfigurationServer(): AAtributes(), _portString(NULL), _host(NULL), _port(0) {
}

ConfigurationServer::~ConfigurationServer() {

}

ConfigurationServer::ConfigurationServer(std::vector<std::string>::iterator &begin, const std::vector<std::string>::iterator &end): AAtributes(), _portString(NULL), _host(NULL), _port(0){
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
    // for (std::map<std::string, Location>::iterator it = this->_location.begin();
    //      it != this->_location.end();
    //      ++it)
    // {
    //     std::cout << "dedebug location : " << std::endl;
    //     std::cout << it->second << std::endl;
    // }
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
    if (n != 3)
        throw std::runtime_error("Server requires an address and a port.");
    this->_host = (*(++it)).c_str();
    if (!strIsDigit(*(++it))) {
        throw std::runtime_error("Port must be a digit.");
    }
    double host = std::strtod((*it).c_str(), NULL);
    int port = static_cast<int>(host);
    if (port < 0 || port > 65535) {
        throw std::runtime_error("Port must be between 0 and 65535.");
    }
    this->_port = port;
    this->_portString = it->c_str();
}



const std::vector<std::string> &ConfigurationServer::getServerNames() const {
    return _serverNames;
}

const std::map<std::string, Location> &ConfigurationServer::getLocation() const {
    return _location;
}

const char  *ConfigurationServer::getPortString() const {
    return  this->_portString;
}



ConfigurationServer &ConfigurationServer::operator=(const ConfigurationServer &from){

    // std::cout << GREY << "Epoll '=' overload called" << RESET << std::endl;
    if (this == &from)
        return (*this);
    this->_host = from._host;
    this->_port = from._port;
    this->_portString = from._portString;
    this->_serverNames = from._serverNames;
    return (*this);
}

ConfigurationServer::ConfigurationServer(const ConfigurationServer &from):
AAtributes(from)
{
    this->_host = from._host;
    this->_port = from._port;
    this->_serverNames = from._serverNames;
    this->_location = from._location;
    this->_portString = from._portString;
}

int ConfigurationServer::getPort() const {
    return _port;
}

const char *ConfigurationServer::getHost() const {
    return _host;
}

std::ostream &operator<<(std::ostream &os, const ConfigurationServer &server) {
    os << "server name: " << std::endl;
    for (std::vector<std::string>::const_iterator it = server.getServerNames().begin(); it != server.getServerNames().end(); ++it) {
        os << "\t" << *it;
    }
    return os;
}