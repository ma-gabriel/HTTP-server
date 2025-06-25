#include "Epoll.hpp"
#include <string.h> //strerror
#include <iostream>
#include <time.h>
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
		throw std::runtime_error("epoll:" + std::string(strerror(errno)));
	this->_events = new epoll_event[MAXEVENT];
#else
    this->_fd = kqueue();
	if (this->_fd == -1)
		throw std::runtime_error("kqueue:" + std::string(strerror(errno)));
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
	// once a sec, checks thatno timeout (err 504)
	if (event_quant == -1 && errno != EINTR){
		perror("epoll_wait");
	}
	for (int i = 0; i < event_quant; i++)
	{
		if (serv.isCGI(this->_events[i].data.fd) == true) {
			serv.handleCGI(this->_events[i]);
			continue;
		}
		if (!(this->_events[i].events & EPOLLIN) && !(this->_events[i].events & EPOLLOUT))
			this->delAndCloseSocket(this->_events[i].data.fd);
		else
			this->handleEvents(this->_events[i].data.fd);
	}
    #else
	int eventKqueue;
	struct timespec timeout;
	timeout.tv_sec = 1;
	timeout.tv_nsec = 0;
	eventKqueue = kevent(this->_fd, NULL, 0, this->_events, MAXEVENT, &timeout);
	if (eventKqueue == -1){
		perror("epoll_wait");
	}
	for (int i = 0; i < eventKqueue; i++)
	{
		if (serv.isCGI(this->_events[i].ident) == true)
		 	serv.handleCGI(this->_events[i]);
		else if (this->_events[i].flags & EV_EOF) {
			this->delAndCloseSocket(this->_events[i].ident);
		}
		else {
			this->handleEvents(this->_events[i].ident, serv);
		}
	}
	#endif
	serv.routineReq();
	serv.routineCGI();
}

void Epoll::handleEvents(int sock)
{
	Server &serv = Server::instance();
	if (serv.isServSocket(sock) == true)
		this->handleNewClients(sock, serv);
	else if (serv.createRequests(sock))		
	{
#ifdef DEBUG
		std::cout << "Receiving new request from " << serv.getClientAddress(sock) << std::endl;
#endif
		serv.handleRequest(sock);
		serv.getRequests().erase(sock);
		delAndCloseSocket(sock);
	}
}


void Epoll::handleNewClients(int sock, Server &serv) {
	int client_sock = serv.newClient(sock);
	if (client_sock != -1) {
		this->addFd(client_sock);
		this->_fdClientsConfigs[client_sock] = serv.getInstances()[sock];
#if DEBUG
		std::cout << "New client connected on socket: " << client_sock << std::endl << std::endl;
		std::cout << "server client: " << this->_fdClientsConfigs[client_sock] << std::endl << std::endl;
#endif
	}
}

void Epoll::addFd(int fd, bool in)
{
	if (fd == -1)
		return;
	#ifdef LINUX
	struct epoll_event event;

	event.data.fd = fd;
	event.events = (EPOLLIN * in) | (EPOLLOUT * !in) | EPOLLET;

	if (epoll_ctl(this->_fd, EPOLL_CTL_ADD, fd, &event) == -1)
	{
		perror("epoll_ctl (add)");
		return;
	}
	#else
	int flags = fcntl(fd, F_GETFL, 0);
	fcntl(fd, F_SETFL, flags | O_NONBLOCK);

	struct kevent change;
	if (!in) {
		this->_epollWrite.push_back(fd);
	}
	EV_SET(&change, fd, in ? EVFILT_READ : EVFILT_WRITE, EV_ADD | EV_ENABLE, 0, 0, NULL);
	if (kevent(this->_fd, &change, 1,  NULL, 0, NULL) == -1) {
		close(fd);
		return;
	}
	#endif
}

void Epoll::delAndCloseSocket(int sock)
{
	#ifdef LINUX
	if (epoll_ctl(this->_fd, EPOLL_CTL_DEL, sock, NULL) == -1)
	{
		perror("epoll_ctl (del)");
		return;
	}
	#else
	struct kevent change;
	bool write = std::find(this->_epollWrite.begin(), this->_epollWrite.end(), sock) != this->_epollWrite.end();
	EV_SET(&change, sock, (write ? EVFILT_WRITE : EVFILT_READ), EV_DELETE, 0, 0, NULL);

	if (kevent(this->_fd, &change, 1,  NULL, 0, NULL) == -1){
		perror("kevent add client3");
		close(sock);
		return;
	}
	if (write)
		this->_epollWrite.erase(std::remove(this->_epollWrite.begin(), this->_epollWrite.end(), sock), this->_epollWrite.end());
	
	#endif
	close(sock);
	Server::instance().delClient(sock);
	this->_fdClientsConfigs.erase(sock);
}

std::map<int, ConfigurationServer> &Epoll::getFdClientConfigs() {
	return this->_fdClientsConfigs;
}
// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, const Epoll& epoll)
{
	std::cout << epoll.getFd();
	return (stream);
}
