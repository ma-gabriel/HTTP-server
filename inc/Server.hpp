#ifndef SERVER_HPP
# define SERVER_HPP
# include <ostream>

#ifdef LINUX
# include <sys/epoll.h>
#else
# include <sys/fcntl.h>
# include <sys/event.h>
# include <sys/time.h>
#endif
# include <ostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <errno.h>
# include <deque>
# include <map>
# include <ctime>
# include "CGI.hpp"

class ConfigurationServer;
class Epoll;

class Server
{
public:
	// Instance Generator
	static Server &instance();
	// Destructors
	~Server(void);
	// Getters
	int getClientNumber(void) const;
	int getSocketFromPort(short port);

#ifdef DEBUG
	char* getClientAddress(int sock) const;
#endif

	// Setters
	void delClient(int sock);
	// Checkers
	bool isServSocket(int fd) const;
	// Public member functions
	int newInstance(ConfigurationServer server);
	int newClient(int sock);
	void handleRequest(int sock);
	void handleNewClients(Epoll& epoll, int socket);

	// CGI handlers
#ifdef LINUX
	void handleCGI(epoll_event event);
#else
	void handleCGI(strcut kevent event);
#endif

	bool isCGI(int fd) const;
	bool addCGI(int fd, CGI::infos infos,bool in);
	void routineCGI();
private:
	// Constructors
	Server(void);
	Server(const Server &from);
	// Overloaded operators
	Server &operator=(const Server &from);
	// data members
	std::map<int, CGI::infos> _CGIs;
	std::map<int, ConfigurationServer> _instances;
	std::deque<int> _clients;

#ifdef DEBUG
	std::map<int, char*> _clients_debug;
#endif

};

std::ostream& operator<<(std::ostream& stream, const Server& instance);

#endif