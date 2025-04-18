#include "ARequest.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
ARequest::ARequest(void)
{
	// std::cout << GREY << "ARequest constructor called" << RESET << std::endl;
	return;
}

ARequest::ARequest(const ARequest &from)
{
	// std::cout << GREY << "ARequest copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
ARequest::~ARequest(void)
{
	// std::cout << GREY << "ARequest destructor called" << RESET << std::endl;
	return;
}

// Overloaded operators
ARequest& ARequest::operator=(const ARequest &from)
{
	// std::cout << GREY << "ARequest '=' overload called" << RESET << std::endl;
	if (this != &from)
	{
	}
	return (*this);
}

// Getters

// Setters

// Public member functions
void ARequest::handleRequest(int sock)
{
	char buff[4096];
	int readed;

	readed = read(sock, buff, sizeof(buff) - 1);
	buff[readed] = '\0';

#ifdef DEBUG
	std::cout << buff << std::endl;
#endif

	// std::string req = buff;
	// if (req.find("STOP") != std::string::npos)
	// 	running = false;

	std::string start_line = "HTTP/1.1 200 OK";
	std::string body = "<!DOCTYPE html>\n<html>\n<head>\n  <title>webserv</title>\n  <link rel=\"icon\" href=\"https://42.fr/wp-content/uploads/2021/07/cropped-42-favicon-acs-32x32.png\" sizes=\"32x32\" />\n</head>\n<body>\n\nHello, World\n\n</body>\n</html>";
	std::string headers = "Content-Type: text/html; charset=UTF-8\n\rContent-len: ";
	{
		std::stringstream convert;
		convert << body.length();
		headers += convert.str();
	}
	std::string resp = start_line + "\n\r" + headers + "\n\r\n\r" + body;
	write(sock, resp.c_str(), resp.length());
}

// Overloaded print operator
// std::ostream& operator<<(std::ostream& stream, const ARequest& instance)
// {
// 	return (stream);
// }
