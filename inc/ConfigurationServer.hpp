//
// Created by rperrot on 5/22/25.
//

#ifndef CONFIGURATIONSERVER_HPP
# define CONFIGURATIONSERVER_HPP

#include "Atributes.hpp"
#include <netdb.h>
#include "Location.hpp"

class ConfigurationServer : public Atributes
{
public:
    int getPort() const;

public:
    ConfigurationServer();
    ~ConfigurationServer();
    ConfigurationServer(const ConfigurationServer &from);
    ConfigurationServer(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    const addrinfo *getAddr() const;
    const std::vector<std::string> &getServerNames() const;
     std::map<std::string, Location> &getLocation();
    ConfigurationServer &operator=(const ConfigurationServer &from);
     std::string &getPortString();
     std::string &getHost();

private:
    std::map<std::string, Location>  _location;
    std::vector<std::string>         _serverNames;
    std::string                      _portString;
    std::string                      _host;
    int                              _port;
    bool addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    void addServerName(std::vector<std::string>::iterator &it, int n);
    void addListen(std::vector<std::string>::iterator &it, int n);
};

std::ostream &operator<<(std::ostream &os,  ConfigurationServer &server);
#endif
