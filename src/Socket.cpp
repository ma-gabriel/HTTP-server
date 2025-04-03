#include "Socket.hpp"

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>

#include "Epoll.hpp"

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Socket::Socket(short port)
{
	// std::cout << GREY << "Socket constructor called" << RESET << std::endl;

	// Oppening socket for IPv4 communication (AF_INET),
	// using TCP protocol (SOCK_STREAM), and non-blocking fd (SOCK_NONBLOCK)
	this->_fd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_fd == -1)
	{
		perror("socket");
		return ;
	}

	// Creating IPv4 (AF_INET) structure,
	// to listen on 0.0.0.0 address (INADDR_ANY)
	// on a certain port, on Network Bytes Order (htons(port))
	sockaddr_in	addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (INADDR_ANY);

	// Assinging the newly created socket to the address and port
	if (bind(this->_fd, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		close(this->_fd);
		return ;
	}

	// Set the newly created and binded socket to listen,
	// and set max client listening queue to maximum (SOMAXCONN)
	if (listen(this->_fd, SOMAXCONN) == -1)
	{
		perror("bind");
		close(this->_fd);
		return ;
	}
}

Socket::Socket(const Socket &from)
{
	// std::cout << GREY << "Socket copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
Socket::~Socket(void)
{
	// std::cout << GREY << "Socket destructor called" << RESET << std::endl;
	close(this->_fd);
	return;
}

// Overloaded operators
Socket& Socket::operator=(const Socket &from)
{
	// std::cout << GREY << "Socket '=' overload called" << RESET << std::endl;
	if (this != &from)
	{
	}
	return (*this);
}

// Getters
int Socket::getFd(void) const
{
	return (this->_fd);
}

// Setters

// Public member functions
void Socket::handleNewClients(Epoll& epoll) const
{
	int client_fd = accept(this->_fd, NULL, NULL);

	while (client_fd != -1)
	{
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		getpeername(client_fd, (sockaddr*)&addr, &addr_len);
		std::cout << "New client : " << inet_ntoa(addr.sin_addr) << std::endl;
		Socket::setSocketNonBlocking(client_fd);
		epoll.addFd(client_fd);
		client_fd = accept(this->_fd, NULL, NULL);
	}

	if (client_fd == -1 && !(errno == EAGAIN || errno == EWOULDBLOCK))
		perror("accept");
}

void Socket::setSocketNonBlocking(int sfd)
{
	int flags;

	flags = fcntl(sfd, F_GETFL, 0);
	if (flags == -1)
	{
		perror("fcntl");
		return;
	}

	if (fcntl(sfd, F_SETFL, flags | O_NONBLOCK) == -1)
	{
		perror("fcntl");
		return;
	}
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Socket& instance)
{
	stream << instance.getFd();
	return (stream);
}
