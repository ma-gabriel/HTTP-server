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
Server::Server(void)
{
	// std::cout << GREY << "Server constructor called" << RESET << std::endl;
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
	std::map<int, short>::iterator it;
	for (it = this->_instances.begin(); it != this->_instances.end(); it++)
		close(it->first);

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
int Server::getSocketFromPort(short port)
{
	std::map<int, short>::iterator it;
	for (it = this->_instances.begin(); it != this->_instances.end(); it++)
		if (it->second == port)
			return (it->first);
	return (0);
}

int Server::getClientNumber(void) const
{
	return (this->_clients.size());
}

// Setters
void Server::delClient(int sock)
{
	this->_clients.erase(std::find(this->_clients.begin(), this->_clients.end(), sock));
}

// Checkers
bool Server::isServSocket(int fd) const
{
	if (this->_instances.find(fd) != this->_instances.end())
		return (true);
	return (false);
}

// Public member functions
int Server::newInstance(short port)
{
	// Oppening socket for IPv4 communication (AF_INET),
	// using TCP protocol (SOCK_STREAM), and non-blocking fd (SOCK_NONBLOCK)
	int sock = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (sock == -1)
	{
		perror("socket");
		return(0);
	}

	// Enabling (opt_value = 1) different options on socket,
	// to reuse same addresss multiple time (SO_REUSEADDR), and
	// to reuse same port multiple time (SO_REUSEPORT),
	// to prevent port/address lock after innatended program end
	int opt_value = 1;
	if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt_value, sizeof(int)) == -1)
		perror("setsockopt");

	// Creating IPv4 (AF_INET) structure,
	// to listen on 0.0.0.0 address (INADDR_ANY)
	// on a certain port, on Network Bytes Order (htons(port))
	sockaddr_in	addr;
	addr.sin_port = htons(port);
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = (INADDR_ANY);

	// Assinging the newly created socket to the address and port
	if (bind(sock, (sockaddr*)&addr, sizeof(addr)) == -1)
	{
		perror("bind");
		close(sock);
		return(0);
	}

	// Set the newly created and binded socket to listen,
	// and set max client listening queue to maximum (SOMAXCONN)
	if (listen(sock, SOMAXCONN) == -1)
	{
		perror("listen");
		close(sock);
		return(0);
	}

#ifdef DEBUG
	std::cout << "Server now listing on " << inet_ntoa(addr.sin_addr) << " port " << port << std::endl;
#endif

	this->_instances.insert(std::make_pair(sock, port));
	return(sock);
}

int Server::newClient(int sock)
{
	int client_sock = accept(sock, NULL, NULL);
	if (client_sock == -1)
		return (-1);

	this->_clients.push_back(client_sock);
	Server::setSocketNonBlocking(client_sock);

#ifdef DEBUG
		struct sockaddr_in addr;
		socklen_t addr_len = sizeof(addr);
		getpeername(client_sock, (sockaddr*)&addr, &addr_len);
		std::cout << "New client : " << inet_ntoa(addr.sin_addr) << std::endl;
#endif

	return (client_sock);
}

void Server::setSocketNonBlocking(int sfd)
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
std::ostream& operator<<(std::ostream& stream, Server& instance)
{
	stream << "Server: ";
	stream << "nbClients -> " << instance.getClientNumber();
	return (stream);
}
