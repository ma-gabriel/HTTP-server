#ifndef EPOLL_HPP
# define EPOLL_HPP

# include <ostream>
# ifdef LINUX
#  include <sys/epoll.h>
# else
#  include <sys/fcntl.h>
#  include <sys/event.h>
#  include <sys/time.h>
# endif
# include <sys/types.h>
# include <unistd.h>
# include <vector>
# include <stdio.h>
# include <map>
# include <utility>
# include <ctime>
# include <ConfigurationServer.hpp>

# include "Server.hpp"

# define MAXEVENT 64

class Epoll
{
public:
// Destructors
    ~Epoll(void);
// Instance Generator
	static Epoll &instance();
// Getters
    int getFd(void) const;
#ifdef LINUX
    struct epoll_event* getEventsPtr(void) const;
#else
    struct kevent *getKevents(void) const;
#endif
// Public member functions
    void routine(Server& serv);
    void handleEvents(int sock);
    void handleNewClients(int sock, Server& serv) ;
    void addFd(int fd, bool in = true);
    void modFd(int fd, bool in = false);
    void delAndCloseSocket(int sock);
    std::map<int, ConfigurationServer> &getFdClientConfigs();

// Static Variable
    static bool isRunning;

private:
    std::map<int, ConfigurationServer> _fdClientsConfigs;
    int _fd;
#ifdef LINUX
    struct epoll_event *_events;
#else
    std::vector<int> _epollWrite;
    struct kevent *_events;
#endif
// Constructors private
	Epoll(void);
	Epoll(const Epoll &from);
// Overloaded operators
	Epoll &operator=(const Epoll &from);
};

std::ostream& operator<<(std::ostream& stream, const Epoll& epoll);

#endif