#include "Request.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sstream>

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

Request::Request(int sock) : _sock(sock)
{
	char buff[8192];
	int readed;

	while ((readed = read(this->_sock, buff, sizeof(buff) - 1)) != -1){
		buff[readed] = '\0';
		this->_raw += buff;
	}

#ifdef DEBUG
	std::cout << this->_raw << std::endl;
#endif

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
	}
	return (*this);
}

// Getters

// Setters

// Public member functions
void Request::handleRequest()
{
	this->parseFirstLine();
	this->checkFirstLine();
	std::cout << "First line ok" << std::endl;
}


std::string Request::extractOneLine()
{
	std::string::size_type line_end = this->_raw.find("\r\n");
	if (line_end == std::string::npos)
		throw Request::BadRequestExeception();

	std::string line = this->_raw.substr(0, line_end);
	this->_raw.erase(0, line_end + 2);

	return (line);
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
		throw Request::BadRequestExeception();
	if (this->_path.empty())
		throw Request::BadRequestExeception();
	if (this->_version.empty() || this->_version != "HTTP/1.1")
		throw Request::BadRequestExeception();
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
