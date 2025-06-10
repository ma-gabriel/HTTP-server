#include "Epoll.hpp"

#include <iostream>
#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif


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
#ifdef LINUX
	this->_fd = epoll_create(1);
	if (this->_fd == -1)
	{
		perror("epoll_create");
		return;
	}
	this->_events = new epoll_event[MAXEVENT];
#else
    this->_fd = kqueue();
	if (this->_fd == -1)
	{
		perror("kqueue_create");
		return;
	}
	this->_events = new struct kevent[MAXEVENT];
#endif
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

}

// Overloaded operators
Epoll& Epoll::operator=(const Epoll &from)
{

	// std::cout << GREY << "Epoll '=' overload called" << RESET << std::endl;
	if (this == &from)
		return (*this);
	this->_fd = from._fd;
	if (!this->_events)
#ifdef LINUX
		this->_events = new epoll_event[MAXEVENT];
#else
		this->_events = new struct kevent[MAXEVENT];
#endif
	for (unsigned int i = 0; i < MAXEVENT; i++)
		this->_events[i] = from._events[i];
	return (*this);
}

// Getters
int Epoll::getFd(void) const
{
	return (this->_fd);
}

#ifdef LINUX
epoll_event* Epoll::getEventsPtr(void) const
{
	return (this->_events);
}
#else
struct kevent *Epoll::getKevents(void) const {
	return this->_events;
}
#endif

void Epoll::routine(Server &serv)
{
    #ifdef LINUX
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
    #else
	int eventKqueue;
	eventKqueue = kevent(this->_fd, NULL, 0, this->_events, MAXEVENT, NULL);
	if (eventKqueue == -1){
		perror("epoll_wait");
	}
	for (int i = 0; i < eventKqueue; i++)
	{
		// if (serv.isCGI(this->_events[i].ident) == true)
		// 	serv.handleCGI(this->_events[i]);
		if (this->_events[i].flags & EV_EOF) {
			this->delAndCloseSocket(this->_events[i].ident, serv);
		}
		else {
			this->handleEvents(this->_events[i].ident, serv);
		}
	}
	#endif
	// serv.routineCGI();
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
	#ifdef LINUX
	struct epoll_event event;

	event.data.fd = fd;
	event.events = EPOLLIN | EPOLLET;

	if (epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		perror("epoll_ctl");
		return;
	}
	#else
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	struct kevent change;
	EV_SET(&change, fd, EVFILT_READ, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(this->_fd, &change, 1,  NULL, 0, NULL) == -1) {
		perror("kevent add client");
		close(fd);
		return;
	}
	#endif
}

void Epoll::delAndCloseSocket(int sock, Server &serv) const
{
	#ifdef LINUX
	struct epoll_event event;
	(void) event; // -Werror=unused-but-set-variable

	event.data.fd = sock;

	if (epoll_ctl(this->_fd, EPOLL_CTL_DEL, sock, NULL) == -1)
	{
		perror("epoll_ctl");
		return;
	}
	#else
	struct kevent change;
	EV_SET(&change, sock,  EVFILT_READ, EV_DELETE, 0, 0, NULL);

	if (kevent(this->_fd, &change, 1,  NULL, 0, NULL) == -1){
		perror("kevent add client");
		close(sock);
		return;
	}

	#endif
	close(sock);
	serv.delClient(sock);
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Epoll& epoll)
{
	std::cout << epoll.getFd();
	return (stream);
}
