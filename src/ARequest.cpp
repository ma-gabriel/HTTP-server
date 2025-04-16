#include "ARequest.hpp"

#include <iostream>
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

	std::string resp = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nContent-Type: text/plain\r\n\r\nHello, World\n";
	write(sock, resp.c_str(), resp.length());
}

// Overloaded print operator
// std::ostream& operator<<(std::ostream& stream, const ARequest& instance)
// {
// 	return (stream);
// }
