
#include "ConfigurationServer.hpp"
#include <netdb.h>
#include <cstdlib>


ConfigurationServer::ConfigurationServer(): Atributes(), _port(0) {
    // std::cout << "ConfigurationServer default constructor called" << std::endl;
}

ConfigurationServer::~ConfigurationServer() {

}

ConfigurationServer::ConfigurationServer(std::vector<std::string>::iterator &begin, const std::vector<std::string>::iterator &end): Atributes(), _port(0){
    if (++begin == end || *begin != "{")
        throw std::runtime_error("after server is not left brace ");
    while (++begin != end && *begin != "}") {
        if (*begin == "location")
        {
            Location location(begin, end);
            if (this->_location.find(location.getPath()) != this->_location.end()) {
                throw std::runtime_error("Location " + location.getPath() + " already exists.");
            }
            this->_location[location.getPath()] = location;
        }
        else if  (*begin == "}")
            return;
        else if (!this->addAttributes(begin, end)) {
            throw std::runtime_error("Unknow Attributes " + *begin);
        }
    }
    if (begin == end || *begin != "}") {
        throw std::runtime_error("after server is not right brace ");
    }
    if (begin == end || *begin != "}") {
        throw std::runtime_error("after server is not right brace ");
    }
    if (this->_port == 0) {
        throw std::runtime_error("Server requires a listen attribute.");
    }

}

bool ConfigurationServer::addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end){
    if (Atributes::addAttributes(it, end))
        return true;
    int n = this->getSizeLine(it, end);
    if (strcmpNocase(*it, "listen"))
        addListen(it, n);
    else if (strcmpNocase(*it, "server_name"))
        addServerName(it, n);
    else
        return false;
    it++;
    return true;
}

void ConfigurationServer::addServerName(std::vector<std::string>::iterator &it, int n) {
    if (!this->getServerNames().empty())
        std::cout << "Server name already set." << std::endl;
    if (n < 2) {
        throw std::runtime_error("Server name requires at least one name.");
    }
    for (int i = 1; i < n; ++i) {
        this->_serverNames.push_back(*(++it));
    }
}

void ConfigurationServer::addListen(std::vector<std::string>::iterator &it, int n) {
    if (this->_port != 0) {
        throw std::runtime_error("Listen attribute already set.");
    }
    if (n != 3)
        throw std::runtime_error("Server requires an address and a port.");
    this->_host = (*(++it)).c_str();
    createHostValue(this->_host);
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

void ConfigurationServer::createHostValue(const std::string &host) {
    this->_hostValue = 0;
    size_t index = 0;
    size_t findPoint;
    int    octetValue;

    for (size_t i = 0; i < 4; ++i) {
        if (i != 3) {
             findPoint = host.substr(index, host.size()).find('.');
            if (findPoint == std::string::npos)
                throw std::runtime_error("Host must be an IPv4 address.");
        }
        else
            findPoint = host.size() - index;
        if (findPoint > 3 || !strIsDigit(host.substr(index, findPoint)))
            throw std::runtime_error("Host must be an IPv4 address.");
        std::string octet = host.substr(index, findPoint);
        octetValue = std::atoi(octet.c_str());
        index += findPoint + 1;
        if (octetValue < 0 || octetValue > 255) {
            throw std::runtime_error("Octet value must be between 0 and 255.");

        }
        this->_hostValue = this->_hostValue * 256 + octetValue;
    }
}

const std::vector<std::string> &ConfigurationServer::getServerNames() const {
    return _serverNames;
}

 std::map<std::string, Location> &ConfigurationServer::getLocation(){
    return _location;
}

std::string &ConfigurationServer::getPortString() {
    return this->_portString;
}

ConfigurationServer &ConfigurationServer::operator=(const ConfigurationServer &from){
    // std::cout << "ConfigurationServer aftectation operator called" << std::endl;
    // std::cout << GREY << "Epoll '=' overload called" << RESET << std::endl;
    if (this == &from)
        return (*this);
    Atributes::operator=(from);
    this->_host = from._host;
    this->_port = from._port;
    this->_portString = from._portString;
    this->_serverNames = from._serverNames;
    this->_location = from._location;
    this->_hostValue = from._hostValue;
    return (*this);
}


ConfigurationServer::ConfigurationServer(const ConfigurationServer &from):
Atributes(from)
{
    // std::cout << "ConfigurationServer copy constructor called from port : " << from._portString;
    this->_host = from._host;
    this->_port = from._port;
    this->_serverNames = from._serverNames;
    this->_location = from._location;
    this->_hostValue = from._hostValue;
    this->_portString = from._portString;
}

int ConfigurationServer::getPort() const {
    return _port;
}

 std::string &ConfigurationServer::getHost() {
    return _host;
}

unsigned int  ConfigurationServer::gethostValue() {
    return this->_hostValue;
}

void ConfigurationServer::setHost(const std::string &host) {
    this->_host = host;
}

std::ostream &operator<<(std::ostream &os, ConfigurationServer &server) {
    os << "server port: " << server.getPort() <<  std::endl;
    os << "server name: " << std::endl;
    for (std::vector<std::string>::const_iterator it = server.getServerNames().begin(); it != server.getServerNames().end(); ++it) {
        os <<  *it << " ";
    }
    for (std::map<std::string, Location>::iterator it = server.getLocation().begin(); it != server.getLocation().end(); ++it) {
        std::cout << std::endl  << "location : " << it->first << std::endl;
        os <<  it->first << " location Name " << it->second << " ";
    }
    os << static_cast<Atributes>(server);
    return os;
}
