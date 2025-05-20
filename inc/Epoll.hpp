#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <ostream>

#include <sys/epoll.h>
#include <sys/types.h>
#include <unistd.h>
#include <vector>
#include <stdio.h>

#include "Server.hpp"

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
    void routine(Server& serv);
    void static handleEvents(int sock, Server& serv);
    void handleNewClients(int sock, Server& serv) const;
    void addFd(int fd) const;
    void delAndCloseSocket(int sock, Server& serv) const;

// Static Variable
    static bool isRunning;

private:
    int _fd;
    struct epoll_event *_events;
};

std::ostream& operator<<(std::ostream& stream, const Epoll& instance);

#endif