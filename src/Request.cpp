#include "Request.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <cstdlib>
#include <errno.h>
#include <stdio.h>
#include <sstream>
#include <sys/socket.h>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Request::Request(void)
{
	// std::cout << GREY << "Request constructor called" << RESET << std::endl;
	return;
}

Request::Request(int sock) : _sock(sock), _time(std::time(NULL))
{
	char buff[8192];
	int val = recv(this->_sock, buff, sizeof(buff) - 1, MSG_DONTWAIT);
	if (val == -1)
		return ;
	buff[val] = 0;
	_raw += buff;

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
	this->parseFirstLine();
	this->checkFirstLine();

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
		throw Request::BadRequestException("Bad HTTP method.");
	if (this->_path.empty())
		throw Request::BadRequestException("No path have been provided.");
	if (this->_version.empty() || this->_version != "HTTP/1.1")
		throw Request::BadRequestException("Bad HTTP version");
}

bool Request::checkMethod()
{
	if (this->_method.empty())
		return (false);
	if (this->_method == "GET")
		return (true);
	if (this->_method == "POST")
		return (true);
	if (this->_method == "DELETE")
		return (true);
	return (false);
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

std::string getMethodString(EHttpMethode method) {
	switch (method) {
		case Post: return "POST";
		case Get: return "GET";
		case Delete: return "DELETE";
		case Put: return "PUT";
		default: return "UNKNOWN";
	}
};

bool Request::isValid()
{
	if (_raw.find("\r\n\r\n") == std::string::npos)
		return false; //because headers not finished
	if (_raw.find("Content-Length: ") == std::string::npos)
		return true; //because nothing useful after \r\n\r\n
	if ((long) (_raw.length() - _raw.find("\r\n\r\n")) >= std::atol(_raw.c_str() + _raw.find("Content-Length: ") + 17))
		return true;
	return false;

}