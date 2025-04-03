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
	this->events = new epoll_event[MAXEVENT];
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
	delete [] this->events;
	return;
}

// Overloaded operators
Epoll& Epoll::operator=(const Epoll &from)
{
	// std::cout << GREY << "Epoll '=' overload called" << RESET << std::endl;
	if (this == &from)
		return (*this);
	this->_fd = from._fd;
	if (!this->events)
		this->events = new epoll_event[MAXEVENT];
	for (unsigned int i = 0; i < MAXEVENT; i++)
		this->events[i] = from.events[i];
	return (*this);
}

// Getters
int Epoll::getFd(void) const
{
	return (this->_fd);
}

epoll_event* Epoll::getEventsPtr(void) const
{
	return (this->events);
}

// Setters

// Public member functions
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

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Epoll& instance)
{
	std::cout << instance.getFd();
	return (stream);
}
