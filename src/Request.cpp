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

static std::map<std::string, Location>::iterator decide_location(std::map<std::string, Location> &dict, std::string path);

// Constructors
Request::Request(void)
{
	// std::cout << GREY << "Request constructor called" << RESET << std::endl;
	return;
}

Request::Request(int sock) : _sock(sock), _time(std::time(NULL))
{
	_config = Location(Epoll::instance().getFdClientConfigs()[sock][0]);

#ifdef DEBUG
	std::cout << this->_raw << std::endl;
#endif

}

void Request::read()
{
	char buff[8192];
	int val = recv(this->_sock, buff, sizeof(buff) - 1, MSG_DONTWAIT);
	if (val == -1)
		return ;
	buff[val] = 0;
	_raw += buff;
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

std::string Request::getBody(void) const
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

std::map<std::string, std::string> Request::getHeaders(void) const
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
	if (this->checkMethod() == false)
		throw 405;
	if (this->_path.empty())
		throw Request::BadRequestException("No path have been provided.");
	if (this->_version.empty() || this->_version != "HTTP/1.1")
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
	if (_method == "Put" && std::find(methods.begin(), methods.end(), Put) != methods.end())
		return (true);
	return false;

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
    size_t iHost = _raw.find("Host: ");
    if (iHost == std::string::npos)
        throw 400;
     size_t endHost = _raw.find(this->_raw, iHost);
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
    return  servers[0];
}

bool Request::isValid()
{

	if (_raw.find("\r\n\r\n") == std::string::npos)
		return false; //because headers not finished
	if (_path.empty())
	{
		parseFirstLine();
		checkFirstLine();
		ConfigurationServer config = getConfigurationServer(Epoll::instance().getFdClientConfigs()[_sock]);
        std::map<std::string, Location> &dict = config.getLocation();
		std::map<std::string, Location>::iterator dict_iterator = dict.end();

		dict_iterator = decide_location(dict, _path);
		if (dict_iterator != dict.end())
			_config = dict_iterator->second;
		else
			_config = Location(config);
	}
	if (((long) (_raw.length() - _raw.find("\r\n\r\n") - 4) > _config.getMaxBodySize()))
		throw 413;
	if (_raw.find("Content-Length: ") == std::string::npos)
		return true; //because nothing useful after \r\n\r\n
	if ((long) (_raw.length() - _raw.find("\r\n\r\n") - 4) == std::atol(_raw.c_str() + _raw.find("Content-Length: ") + 16))
		return true;
	if ((long) (_raw.length() - _raw.find("\r\n\r\n") - 4) > std::atol(_raw.c_str() + _raw.find("Content-Length: ") + 16))
		throw 400;
	return false;
}

static std::map<std::string, Location>::iterator decide_location(std::map<std::string, Location> &dict, std::string path)
{
	if (path.find('/', 1) == std::string::npos)
		return dict.end();
	path = path.substr(0, path.find('/', 1));
	return dict.find(path);
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
