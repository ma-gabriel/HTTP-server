//
// Created by rperrot on 5/22/25.
//

#ifndef CONFIGURATIONSERVER_HPP
# define CONFIGURATIONSERVER_HPP

#include <netinet/in.h>
#include <sys/socket.h>
#include "AAtributes.hpp"
#include "Location.hpp"

class ConfigurationServer : public AAtributes
{
public:
    ConfigurationServer(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    const sockaddr_in &getAddr() const;
    void setAddr(const sockaddr_in &addr);
    const std::string &getHostName() const;
    void setHostName(const std::string &hostName);

private:
    std::string                      _hostName;
    struct sockaddr_in               _addr;
    std::map<std::string, Location>  _location;

};

#endif
