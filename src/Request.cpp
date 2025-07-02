#include "Request.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <algorithm>
#include <stdio.h>
#include <sstream>
#include <sys/socket.h>

#include "Epoll.hpp"

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

static Location * decide_location(std::map<std::string, Location> &dict, std::string path);

// Constructors
Request::Request(void)
{
	// std::cout << GREY << "Request constructor called" << RESET << std::endl;
	return;
}

Request::Request(int sock) : _sock(sock), _time(std::time(NULL))
{
//	_config = Location(Epoll::instance().getFdClientConfigs()[sock][0]);

#ifdef DEBUG
	std::cout << this->_raw << std::endl;
#endif

}

void Request::read()
{
	char buff[65536];
	int val = recv(this->_sock, buff, sizeof(buff) - 1, MSG_DONTWAIT);
	if (val == -1)
		return ;
	_raw.append(buff, val);
}

Request::Request(const Request &from)
{
	// std::cout << GREY << "Request copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
Request::~Request(void)
{
	// std::cout << GREY << "Request destructor called" << RESET << std::endl;
	return;
}

// Overloaded operators
Request& Request::operator=(const Request &from)
{
	// std::cout << GREY << "Request '=' overload called" << RESET << std::endl;
	if (this != &from)
	{
		_sock = from._sock;
		_raw = from._raw;
		_method = from._method;
		_path = from._path;
		_version = from._version;
		_headers = from._headers;
		_body = from._body;
		_time = from._time;
		_config = from._config;
	}
	return (*this);
}

// Getters
int Request::getSock(void) const
{
	return(this->_sock);
}

std::time_t Request::getTime(void) const
{
	return(this->_time);
}

const std::string &Request::getBody(void) const
{
	return (this->_body);
}

std::string Request::getVersion(void) const
{
	return(this->_version);
}

std::string Request::getPath(void) const
{
	return(this->_path);
}

const Location &Request::getConfig(void) const
{
	return (_config);
}

std::string Request::getMethod(void) const
{
	return(this->_method);
}

const std::map<std::string, std::string> &Request::getHeaders(void) const
{
	return(this->_headers);
}

// Setters

// Public member functions
void Request::parseRequest()
{
	this->extractHeaders();
	this->_body = this->_raw;
}

void Request::parseFirstLine()
{
	std::string first_line = extractOneLine();

	std::string buff;
	std::istringstream stream(first_line);

	getline(stream, this->_method, ' ');
	getline(stream, this->_path, ' ');
	getline(stream, this->_version, '\r');
}

void Request::checkFirstLine()
{
	if (!checkMethod())
		throw 405;
	if (!checkPath())
		throw 400;
	if (_version != "HTTP/1.1")
		throw 505;
}

bool Request::checkMethod()
{
	const std::vector<EHttpMethode> &methods = _config.getHttpMethode();
	if (_method == "GET" && std::find(methods.begin(), methods.end(), Get) != methods.end())
		return (true);
	if (_method == "DELETE" && std::find(methods.begin(), methods.end(), Delete) != methods.end())
		return (true);
	if (_method == "POST" && std::find(methods.begin(), methods.end(), Post) != methods.end())
		return (true);
	if (_method == "PUT" && std::find(methods.begin(), methods.end(), Put) != methods.end())
		return (true);
	return false;
}

bool Request::checkPath()
{
	if (_path.find("//") != std::string::npos)
		return false;
	if (_path[0] != '/')
		return false;
	if (_path.length() >= 3 && !_path.compare(_path.length() - 3, 3, "/.."))
		return false;
	if (_path.length() >= 2 && !_path.compare(_path.length() - 2, 2, "/."))
		return false;
	if (_path.find("/../") != std::string::npos)
		return false;
	if (_path.find("/./") != std::string::npos)
		return false;
	return true;
}

void Request::extractHeaders()
{
	std::string curr_line = extractOneLine();
	while (curr_line.empty() == false)
	{
		std::string key = extractHeaderKey(curr_line);
		this->_headers[key] = curr_line;
		curr_line = extractOneLine();
	}
}

std::string Request::extractHeaderKey(std::string& line)
{
	std::string::size_type colon_pos = line.find(":");
	if (colon_pos == std::string::npos)
		throw Request::BadRequestException("Bad header formating");
	std::string key = line.substr(0, colon_pos);
	line.erase(0, colon_pos + 2);
	return (key);
}

std::string Request::extractOneLine()
{
	std::string::size_type line_end = this->_raw.find("\r\n");
	if (line_end == std::string::npos)
		throw Request::BadRequestException("End-of-line error");
	std::string line = this->_raw.substr(0, line_end);
	this->_raw.erase(0, line_end + 2);

	return (line);
}

ConfigurationServer &Request::getConfigurationServer(std::vector<ConfigurationServer> &servers)
{
	// https://datatracker.ietf.org/doc/html/rfc7230#section-5.4 
	// "A client MUST send a Host header field in all HTTP/1.1 request message"
    size_t iHost = _raw.find("Host: ");
    if (iHost == std::string::npos)
        throw 400;
     size_t endHost = _raw.find("\r\n", iHost);
     if (endHost == std::string::npos)
         throw 400;
    std::string host = _raw.substr(iHost + 6, endHost - iHost - 6);
    if (host.find(':') != std::string::npos)
        host = host.substr(0, host.find(':'));
    for (std::vector<ConfigurationServer>::iterator it = servers.begin(); it != servers.end(); ++it)
    {
        for (std::vector<std::string>::const_iterator serverName = it->getServerNames().begin(); serverName != it->getServerNames().end(); ++serverName)
        {
            if (*serverName == host || (host.empty() && *serverName == ""))
                return *it;
        }
    }
    return servers[0];
}

bool Request::isValid()
{
	if (_raw.find("\r\n\r\n") == std::string::npos)
		return false; //because headers not finished
	if (_method.empty())
	{
		parseFirstLine();
		ConfigurationServer config = getConfigurationServer(Epoll::instance().getFdClientConfigs()[_sock]);
        std::map<std::string, Location> &dict = config.getLocation();
		Location *location = decide_location(dict, _path);
		if (location != NULL)
			_config = *location;
		else
			_config = Location(config);
        checkFirstLine();
	}
	if (_raw.length() - _raw.find("\r\n\r\n") - 4 > _config.getMaxBodySize())
		throw 413;
	if (_raw.find("Content-Length: ") == std::string::npos){
		if (_raw.find("\r\n\r\n") == _raw.length() - 4)
			return true;
		throw 400;
	}
	if ((long) (_raw.length() - _raw.find("\r\n\r\n") - 4) == std::atol(_raw.c_str() + _raw.find("Content-Length: ") + 16))
		return true;
	if ((long) (_raw.length() - _raw.find("\r\n\r\n") - 4) > std::atol(_raw.c_str() + _raw.find("Content-Length: ") + 16))
		throw 400;
	return false;
}

static Location *decide_location(std::map<std::string, Location> &dict, std::string path)
{
    for (std::map<std::string, Location>::iterator it = dict.begin(); it != dict.end(); ++it)
    {
        if (path.find(it->first) == 0) // Check if path starts with the location path
        {
            std::string pathLocation = it->first;
            if (it->first[it->first.length() - 1] != '/') // If location path ends with '/', it matches any subdirectory
                pathLocation += '/';
            if (path[path.length() - 1] != '/') // If request path ends with '/', it matches the exact location
                path += '/';
            std::string pathAdapted = path.substr(0, pathLocation.length());
            if (pathAdapted == pathLocation) // Exact match or subdirectory
            {
                return &(it->second);
            }
        }
    }
    return NULL;
}

std::string getMethodString(EHttpMethode method) {
	switch (method) {
		case Post: return "POST";
		case Get: return "GET";
		case Delete: return "DELETE";
		case Put: return "PUT";
		default: return "UNKNOWN";
	}
}
