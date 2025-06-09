#include "Server.hpp"

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <algorithm>
#include <cstring>

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
	std::map<int, ConfigurationServer>::iterator it;
	for (it = this->_instances.begin(); it != this->_instances.end(); it++)
		close(it->first);
	std::for_each(this->_clients.begin(), this->_clients.end(), close);
#ifdef DEBUG
	std::map<int, char*>::iterator it2;
	for (it2 = this->_clients_debug.begin(); it2 != this->_clients_debug.end(); it2++)
		free(it2->second);
#endif
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
	std::map<int, ConfigurationServer>::iterator it;
	for (it = this->_instances.begin(); it != this->_instances.end(); it++)
		if (it->second.getPort() == port)
			return (it->first);
	return (0);
}

int Server::getClientNumber(void) const
{
	return (this->_clients.size());
}

#ifdef DEBUG
char* Server::getClientAddress(int sock) const
{
	std::map<int, char*>::const_iterator it;
	it = this->_clients_debug.find(sock);

	if (it == this->_clients_debug.end())
		return (NULL);
	else
		return(it->second);
}
#endif

// Setters
void Server::delClient(int sock)
{
	std::deque<int>::iterator it = std::find(this->_clients.begin(), this->_clients.end(), sock);
	if (it != this->_clients.end())
		this->_clients.erase(it);
	else
		std::cout << "Client not found" << std::endl;
#if DEBUG
	std::map<int, char*>::iterator client_ptr = this->_clients_debug.find(sock);
	free(client_ptr->second);
	this->_clients_debug.erase(client_ptr);
#endif

}

// Checkers
bool Server::isServSocket(int fd) const
{
	if (this->_instances.find(fd) != this->_instances.end())
		return (true);
	return (false);
}

// Public member functions
int Server::newInstance(ConfigurationServer server)
{
	// Oppening socket for IPv4 communication (AF_INET),
	// using TCP protocol (SOCK_STREAM)
    struct addrinfo hints, *res;
    std::memset(&hints, 0, sizeof(hints));
	int sock = socket(AF_INET, SOCK_STREAM, 0);
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
	// Assinging the newly created socket to the address and port
    if (getaddrinfo(server.getHost(), server.getPortString(), &hints, &res) != 0){
        perror("getaddrinfo");
        close(sock);
        return(0);
    }

	if (bind(sock, res->ai_addr, res->ai_addrlen) == -1)
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

	this->_instances.insert(std::make_pair(sock, server));
	return(sock);
}

int Server::newClient(int sock)
{
	int client_sock = accept(sock, NULL, NULL);
	if (client_sock == -1)
	{
		perror("accept");
		return (-1);
	}
	this->_clients.push_back(client_sock);

#ifdef DEBUG
	struct sockaddr_in addr;
	socklen_t addr_len = sizeof(addr);
	getpeername(client_sock, (sockaddr*)&addr, &addr_len);
	char client_addr[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &addr.sin_addr, client_addr, INET_ADDRSTRLEN);
	std::cout << "New client : " << client_addr << std::endl;
	this->_clients_debug.insert(std::make_pair(client_sock, strdup(client_addr)));
#endif

	return (client_sock);
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, Server& instance)
{
	stream << "Server: ";
	stream << "nbClients -> " << instance.getClientNumber();
	return (stream);
}
