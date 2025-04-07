#include "Server.hpp"

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <algorithm>

#include "Epoll.hpp"

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Server::Server(short port) : _port(port)
{
	// std::cout << GREY << "Server constructor called" << RESET << std::endl;

	// Oppening socket for IPv4 communication (AF_INET),
	// using TCP protocol (SOCK_STREAM), and non-blocking fd (SOCK_NONBLOCK)
	this->_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (this->_socket == -1)
	{
		perror("socket");
		return ;
	}

	// Enabling (opt_value = 1) different options on socket,
	// to reuse same addresss multiple time (SO_REUSEADDR), and
	// to reuse same port multiple time (SO_REUSEPORT),
	// to prevent port/address lock after innatended program end
	int opt_value = 1;
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(int)) == -1)
		perror("setsockopt");
	if (setsockopt(this->_socket, SOL_SOCKET, SO_REUSEPORT, &opt_value, sizeof(int)) == -1)
		perror("setsockopt");

	// Creating IPv4 (AF_INET) structure,
	// to listen on 0.0.0.0 address (INADDR_ANY)
	// on a certain port, on Network Bytes Order (htons(port))
	sockaddr_in	addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (INADDR_ANY);

	// Assinging the newly created socket to the address and port
	if (bind(this->_socket, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		close(this->_socket);
		return ;
	}

	// Set the newly created and binded socket to listen,
	// and set max client listening queue to maximum (SOMAXCONN)
	if (listen(this->_socket, SOMAXCONN) == -1)
	{
		perror("bind");
		close(this->_socket);
		return ;
	}

#ifdef DEBUG
	std::cout << "Server now listing on " << inet_ntoa(addr.sin_addr) << " port " << port << std::endl;
#endif

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
	close(this->_socket);
	std::for_each(this->_clients.begin(), this->_clients.end(), close);
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
short Server::getPort(void) const
{
	return (this->_port);
}

int Server::getSocketFd(void) const
{
	return (this->_socket);
}

int Server::getClientNumber(void) const
{
	return (this->_clients.size());
}

// Setters

// Public member functions
void Server::handleNewClients(Epoll& epoll)
{
	int client_socket = accept(this->_socket, NULL, NULL);

	while (client_socket != -1)
	{
		this->_clients.push_back(client_socket);
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		getpeername(client_socket, (sockaddr*)&addr, &addr_len);
		std::cout << "New client : " << inet_ntoa(addr.sin_addr) << std::endl;
		Server::setServerNonBlocking(client_socket);
		epoll.addFd(client_socket);
		client_socket = accept(this->_socket, NULL, NULL);
	}

	if (client_socket == -1 && !(errno == EAGAIN || errno == EWOULDBLOCK))
		perror("accept");
}

void Server::setServerNonBlocking(int sfd)
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
std::ostream& operator<<(std::ostream& stream, const Server& instance)
{
	stream << "Server: ";
	stream << "fdSocket -> " << instance.getSocketFd();
	stream << "port -> " << instance.getSocketFd();
	stream << "nbClients -> " << instance.getClientNumber();
	return (stream);
}
