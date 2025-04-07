#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <ostream>

#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>

#define MAXEVENT 64

class Epoll
{
public:
// Constructors
	Epoll(void);
	Epoll(const Epoll &from);
// Destructors
	~Epoll(void);
// Overloaded operators
	Epoll &operator=(const Epoll &from);
// Getters
	int getFd(void) const;
	struct epoll_event* getEventsPtr(void) const;
// Setters
// Public member functions
	void addFd(int fd) const;
	void delFd(int fd) const;

private:
	int _fd;
	struct epoll_event *events;
};

std::ostream& operator<<(std::ostream& stream, const Epoll& instance);

#endif
