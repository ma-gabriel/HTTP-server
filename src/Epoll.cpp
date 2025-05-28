#include "Epoll.hpp"

#include <iostream>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

#include "Request.hpp"

bool Epoll::isRunning = true;

Epoll &Epoll::instance()
{
	static Epoll instance;
	return instance;
}

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

	event_quant = epoll_wait(this->_fd, this->_events, MAXEVENT, 1000);
								// once a sec, checks that CGI don't timeout (err 504)
	if (event_quant == -1 && errno != EINTR){	// won't write when close by ^C,
		perror("epoll_wait"); 					// due to same errno value than timeout
	}
	for (int i = 0; i < event_quant; i++)
	{
		if (serv.isCGI(this->_events[i].data.fd) == true)
			serv.handleCGI(this->_events[i]);
		else if (!(this->_events[i].events & EPOLLIN))
			this->delAndCloseSocket(this->_events[i].data.fd, serv);
		else
			this->handleEvents(this->_events[i].data.fd, serv);
	}
	if (event_quant != -1 || errno == EINTR)
		serv.routineCGI();
}

void Epoll::handleEvents(int sock, Server& serv)
{
	if (serv.isServSocket(sock) == true)
		this->handleNewClients(sock, serv);
	else
	{
#ifdef DEBUG
		std::cout << "Receiving new request from " << serv.getClientAddress(sock) << std::endl;
#endif
		serv.handleRequest(sock);
		this->delAndCloseSocket(sock, serv);
	}
}

void Epoll::handleNewClients(int sock, Server &serv) const
{
	int client_sock = serv.newClient(sock);

	if (client_sock != -1)
		this->addFd(client_sock);
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
	if (epoll_ctl(this->_fd, EPOLL_CTL_DEL, sock, NULL) == -1)
	{
		perror("epoll_ctl");
		return;
	}
	close(sock);
	serv.delClient(sock);
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Epoll& epoll)
{
	std::cout << epoll.getFd();
	return (stream);
}
