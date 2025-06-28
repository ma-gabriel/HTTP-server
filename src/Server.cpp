#include "Server.hpp"

#include <iostream>
#include <cstdio>
#include <unistd.h>
#include <arpa/inet.h>
#include <algorithm>
#include <sys/wait.h>
#include <cstring>
#include <signal.h>


#include "Request.hpp"
#include "Response.hpp"
#include "Epoll.hpp"
#include "CGI.hpp"
#include "ConfigurationServer.hpp"

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

Server &Server::instance()
{
	static Server instance;
	return instance;
}

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
	{
		std::map<int, ConfigurationServer>::iterator it;
		for (it = this->_instances.begin(); it != this->_instances.end(); it++)
			close(it->first);
	}
	{
		std::map<int, CGI::infos>::iterator it;
		for (it = this->_CGIs.begin(); it != this->_CGIs.end(); it++) {
			close(it->first);
			if (it->second.pid) {
				close(it->second.output_fd);
				kill(it->second.pid, SIGKILL);
				waitpid(it->second.pid, NULL, 0);
			}
		}
	}
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


std::map<int, std::string> &Server::getResponses()
{
	return Server::instance()._responses;
}

std::map<int, Request> &Server::getRequests()
{
	return Server::instance()._requests;
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
	this->_clients.erase(std::find(this->_clients.begin(), this->_clients.end(), sock));

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

bool Server::addCGI(int fd, CGI::infos infos, bool in)
{
	Epoll::instance().addFd(fd, in);
	_CGIs[fd] = infos;
	return true;
}

static void delFD(int fd)
{
	#ifdef LINUX
	epoll_ctl(Epoll::instance().getFd(), EPOLL_CTL_DEL, fd, NULL);
	#else
	struct kevent change;
	EV_SET(&change, fd,  EVFILT_READ, EV_DELETE, 0, 0, NULL);
	kevent(Epoll::instance().getFd(), &change, 1,  NULL, 0, NULL);
	# endif
	close(fd);
}

#ifdef LINUX
void Server::handleCGI(epoll_event event)
{
	int fd = event.data.fd;
	CGI::infos &infos = _CGIs[fd];
	if (infos.pid == -1)
	{
		if (event.events & EPOLLOUT){
			ssize_t len = write(fd, infos.body.c_str(), infos.body.length());
			if (len == -1 || static_cast<size_t>(len) == infos.body.length()){
				delFD(fd);
				_CGIs.erase(fd);
				return ;
			}
			infos.body = infos.body.substr(len);
		}
		if (event.events & EPOLLHUP || event.events & EPOLLERR){
			delFD(fd);
			_CGIs.erase(fd);
			return ;
		}
	}
	else
	{
		if (event.events & EPOLLIN){
			char buffer[65537];
			ssize_t len = read(fd, buffer, 65536);
			if (len != -1)
				infos.body.append(buffer, len);
		}
		if (event.events & EPOLLHUP || event.events & EPOLLERR){
			CGI::flush(infos.output_fd, infos.body);
			delFD(fd);
			kill(infos.pid, SIGKILL);
			waitpid(infos.pid, NULL, 0);
			_CGIs.erase(fd);
		}
	}
}

#else
void Server::handleCGI(struct kevent kev)
{
	int fd = kev.ident;
	CGI::infos &infos = _CGIs[fd];

	if (infos.pid == -1)
	{
		if (kev.filter == EVFILT_WRITE) {
			ssize_t len = write(fd, infos.body.c_str(), infos.body.length());
			if (len == -1 || static_cast<size_t>(len) == infos.body.length()) {
				delFD(fd);
				_CGIs.erase(fd);
				return;
			}
			infos.body = infos.body.substr(len);
		}
		if ((kev.flags & EV_EOF) || (kev.flags & EV_ERROR)) {
			delFD(fd);
			_CGIs.erase(fd);
			return;
		}
	}
	else
	{
		if (kev.filter == EVFILT_READ) {
			char buffer[65537];
			ssize_t len = read(fd, buffer, 65536);
			if (len != -1)
				infos.body.append(buffer, len);
		}
		if ((kev.flags & EV_EOF) || (kev.flags & EV_ERROR)) {
			CGI::flush(infos.output_fd, infos.body);
			delFD(fd);
			kill(infos.pid, SIGKILL);
			waitpid(infos.pid, NULL, 0);
			_CGIs.erase(fd);
		}
	}
}
#endif

bool Server::isCGI(int fd) const
{
	if (this->_CGIs.find(fd) != this->_CGIs.end())
		return (true);
	return (false);
}

void Server::routineReq()
{
	std::vector <int> to_remove;
	for (std::map<int, Request>::iterator it = _requests.begin(); it != _requests.end(); it++){
		if (it->second.getTime() + 3 < std::time(NULL)){
			to_remove.push_back(it->first);
		}
	}
	for (std::vector<int>::iterator it = to_remove.begin(); it != to_remove.end(); ++it){
		// TODO the usual, probably gonna need to add the ConfigurationServer associated in the map
		Response::sendResponse(*it, Response::error(504, "Gateway Timeout", _requests.at(*it).getConfig().getErrorPages() ));
		Epoll::instance().delAndCloseSocket(*it);
		_requests.erase(*it);
	}
}

void Server::routineCGI()
{
	std::vector <int> to_remove;
	for (std::map<int, CGI::infos>::iterator it = _CGIs.begin(); it != _CGIs.end(); it++){
		if (it->second.timestamp + 15 < std::time(NULL)){
			if (it->second.pid != -1) {
				Response::sendResponse(it->second.output_fd, Response::error(504, "Gateway Timeout", it->second.config.getErrorPages()));
				kill(it->second.pid, SIGKILL);
				waitpid(it->second.pid, NULL, 0);
			}
			to_remove.push_back(it->first);
		}
	}
	for (std::vector<int>::iterator it = to_remove.begin(); it != to_remove.end(); ++it){
		delFD(*it);
		_CGIs.erase(*it);
	}
}

bool Server::createRequests(int sock)
{
	try {
		std::map<int, Request>::iterator it = _requests.find(sock);
		if (it == _requests.end())
			_requests.insert(std::make_pair(sock, Request(sock)));
		else
			it->second.read();
		return (_requests.at(sock).isValid());
	}
	catch (std::string &e)
	{
		if (e == "ERROR400")
			Response::sendResponse(sock, Response::error(400, "Bad Request", _requests.at(sock).getConfig().getErrorPages()));
		if (e == "ERROR413")
			Response::sendResponse(sock, Response::error(413, "Request Entity Too Large", _requests.at(sock).getConfig().getErrorPages()));
		Server::getRequests().erase(sock);
		return false;
	}
	catch (...)
	{
		Response::sendResponse(sock, Response::error(400, "Bad Request", _requests.at(sock).getConfig().getErrorPages()));
		Server::getRequests().erase(sock);
		return false;
	}
}

// Public member functions
int Server::newInstance(ConfigurationServer server)
{
	// Oppening socket for IPv4 communication (AF_INET),
	// using TCP protocol (SOCK_STREAM)
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1)
	{
		perror("socket");
		return(-1);
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
	struct addrinfo hints, *result;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;    // IPv4 ou IPv6
	hints.ai_socktype = SOCK_STREAM; // TCP
	if (getaddrinfo("0.0.0.0", server.getPortString().c_str(), &hints, &result) != 0) {
		perror("bind");
		close(sock);
	}
	// Assinging the newly created socket to the address and port
	int bindStatus = bind(sock,  result->ai_addr, result->ai_addrlen);
	freeaddrinfo(result);
	if (bindStatus == -1)
	{
		perror("bind");
		close(sock);
		return(-1);
	}
	// Set the newly created and binded socket to listen,
	// and set max client listening queue to maximum (SOMAXCONN)
	if (listen(sock, SOMAXCONN) == -1)
	{
		perror("listen");
		close(sock);
		return(-1);
	}
#ifdef DEBUG
	std::cout << "Server now listing on " << inet_ntoa(addr.sin_addr) << " port " << port << std::endl;
#endif

	this->_instances[sock] = server;
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

void Server::handleRequest(int sock)
{
	Request &req = _requests.at(sock);
	try {
		req.parseRequest();
	} catch (Request::BadRequestException &e) {
		Response::sendResponse(sock, Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
#ifdef DEBUG
		std::cout << "Exception: Bad request: ";
		std::cout << e.what() << std::endl;
#endif
		return ;
	}

	if (doCGI(req) == true){
		return ;
	}

	Response::sendResponse(sock, Response::createResponse(req));
}

std::map<int, ConfigurationServer> Server::getInstances() {
	return Server::instance()._instances;
}

// Overloaded print operator
std::ostream& operator<<(std::ostream& stream, Server& server)
{
	stream << "Server: ";
	stream << "nbClients -> " << server.getClientNumber();

	return (stream);
}


void Server::writeResponses(int sock)
{
	std::string &str = Server::instance()._responses[sock];

	ssize_t len = send(sock, str.c_str(), str.length(), MSG_DONTWAIT);
	if (len == -1)
		return ;
	str = str.substr(len);
	if (str.length())
		return ;
	Epoll::instance().delAndCloseSocket(sock);
}