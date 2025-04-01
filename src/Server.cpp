#include "Server.hpp"

#include <iostream>
#include <errno.h>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Server::Server(short port)
{
	// std::cout << GREY << "Server constructor called" << RESET << std::endl;

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
		std::cerr << "Server error: cannot open socket" << std::endl;

	sockaddr_in	addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (INADDR_ANY);
	if (bind(fd, (sockaddr*)&addr, sizeof(addr)) == -1)
		std::cerr << "Server error: cannot assining name to socket" << std::endl;

	if (listen(fd, 10) == -1)
		std::cerr << "Server error: cannot set socket to listening state" << std::endl;

	this->_fd = accept(fd, NULL, NULL);
	if (this->_fd == -1)
	{
		if (errno == EAGAIN)
			std::cerr << "Server accept error: EAGAIN" << std::endl;
		if (errno == EBADF)
			std::cerr << "Server accept error: EBADF" << std::endl;
		if (errno == ECONNABORTED)
			std::cerr << "Server accept error: ECONNABORTED" << std::endl;
		if (errno == EFAULT)
			std::cerr << "Server accept error: EFAULT" << std::endl;
		if (errno == EINTR)
			std::cerr << "Server accept error: EINTR" << std::endl;
		if (errno == EINVAL)
			std::cerr << "Server accept error: EINVAL" << std::endl;
		if (errno == EMFILE)
			std::cerr << "Server accept error: EMFILE" << std::endl;
		if (errno == ENFILE)
			std::cerr << "Server accept error: ENFILE" << std::endl;
		if (errno == ENOTSOCK)
			std::cerr << "Server accept error: ENOTSOCK" << std::endl;
		if (errno == EOPNOTSUPP)
			std::cerr << "Server accept error: EOPNOTSUPP" << std::endl;
		if (errno == EPROTO)
			std::cerr << "Server accept error: EPROTO" << std::endl;
	}

	// 	std::cerr << "Server error: cannot set socket to accept connection" << std::endl;

	std::cout << "Server logging: Now listening for incomming connections" << std::endl;

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
