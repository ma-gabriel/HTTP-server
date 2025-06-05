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
            this->_location.insert(std::make_pair(location.getPath(), location));
        }
        else if  (*begin == "}")
            return;
    }
    if (begin == end || *begin != "}") {
        throw std::runtime_error("after server is not right brace ");
    }
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
