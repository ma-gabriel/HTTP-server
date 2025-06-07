//
// Created by rperrot on 5/22/25.
//

#ifndef CONFIGURATIONSERVER_HPP
# define CONFIGURATIONSERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include "AAtributes.hpp"
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include "Location.hpp"

class ConfigurationServer : public AAtributes
{
public:
    ConfigurationServer(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    const addrinfo *getAddr() const;
    const std::vector<std::string> &getServerNames() const;
    const std::map<std::string, Location> &getLocation() const;

private:
    std::map<std::string, Location>  _location;
    std::vector<std::string>         _serverNames;
    struct addrinfo               *_addr;
    bool addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    void addServerName(std::vector<std::string>::iterator &it, int n);
    void addListen(std::vector<std::string>::iterator &it, int n);
};

std::ostream &operator<<(std::ostream &os, const ConfigurationServer &server);
#endif
