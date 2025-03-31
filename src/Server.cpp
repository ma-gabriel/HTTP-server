#include "Server.hpp"

//#include <iostream>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Server::Server(void)
{
	// std::cout << GREY << "Server constructor called" << RESET << std::endl;
	this->_fd = socket(AF_INET, SOCK_STREAM, 0);
	bind(this->_fd, (struct sockaddr*) INADDR_ANY, 0);
	listen(this->_fd, 0);
	return;
}

Server::Server(const Server &from)
{
	// std::cout << GREY << "Server copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
Server::~Server(void)
{
	// std::cout << GREY << "Server destructor called" << RESET << std::endl;
	return;
}

// Overloaded operators
Server& Server::operator=(const Server &from)
{
	// std::cout << GREY << "Server '=' overload called" << RESET << std::endl;
	if (this != &from)
	{
	}
	return (*this);
}

// Getters
int Server::getFd(void) const
{
	return (this->_fd);
}

// Setters

// Public member functions

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Server& instance)
{
	stream << instance.getFd();
	return (stream);
}
