#include "Epoll.hpp"

#include <iostream>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Epoll::Epoll(void)
{
	// std::cout << GREY << "Epoll constructor called" << RESET << std::endl;
	this->_fd = epoll_create(1);
	if (this->_fd == -1)
	{
		perror("epoll_create");
		return;
	}
	this->_events = new epoll_event[MAXEVENT];
}

Epoll::Epoll(const Epoll &from)
{
	// std::cout << GREY << "Epoll copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
Epoll::~Epoll(void)
{
	// std::cout << GREY << "Epoll destructor called" << RESET << std::endl;
	close(this->_fd);
	delete [] this->_events;
	return;
}

// Overloaded operators
Epoll& Epoll::operator=(const Epoll &from)
{
	// std::cout << GREY << "Epoll '=' overload called" << RESET << std::endl;
	if (this == &from)
		return (*this);
	this->_fd = from._fd;
	if (!this->_events)
		this->_events = new epoll_event[MAXEVENT];
	for (unsigned int i = 0; i < MAXEVENT; i++)
		this->_events[i] = from._events[i];
	return (*this);
}

// Getters
int Epoll::getFd(void) const
{
	return (this->_fd);
}

epoll_event* Epoll::getEventsPtr(void) const
{
	return (this->_events);
}

// Setters

// Public member functions
void Epoll::routine(Server &serv)
{
	int event_quant;

	event_quant = epoll_wait(this->_fd, this->_events, MAXEVENT, -1);
	if (event_quant == -1)
		perror("epoll_wait");

	for (int i = 0; i < event_quant; i++)
	{
		if (!(this->_events[i].events & EPOLLIN))
			close(this->_events[i].data.fd);
		else
			this->handleEvents(this->_events[i], serv);
	}
}

void Epoll::handleEvents(epoll_event event, Server& serv)
{
	int sock = event.data.fd;

	if (serv.isServSocket(sock) == true)
		this->handleNewClients(sock, serv);
	else
	{
		char buff[512];
		int readed;

		readed = read(sock, buff, sizeof(buff));
		buff[readed] = '\0';
		std::cout << buff << std::endl;

		// std::string req = buff;
		// if (req.find("STOP") != std::string::npos)
		// 	running = false;

		std::string resp = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nContent-Type: text/plain\r\n\r\nHello, World\n";
		write(sock, resp.c_str(), resp.length());

		this->delAndCloseSocket(sock, serv);
	}
}

void Epoll::handleNewClients(int sock, Server &serv) const
{
	int client_sock = serv.newClient(sock);

	while (client_sock != -1)
	{
		this->addFd(client_sock);
		client_sock = serv.newClient(sock);
	}
}

void Epoll::addFd(int fd) const
{
	struct epoll_event event;

	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		perror("epoll_ctl");
		return;
	}
}

void Epoll::delAndCloseSocket(int sock, Server &serv) const
{
	struct epoll_event event;

	event.data.fd = sock;

	if (epoll_ctl(this->_fd, EPOLL_CTL_DEL, sock, NULL) == -1)
	{
		perror("epoll_ctl");
		return;
	}
	close(sock);
	serv.delClient(sock);
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Epoll& instance)
{
	std::cout << instance.getFd();
	return (stream);
}
