#ifndef SERVER_HPP
# define SERVER_HPP

# include <ostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <errno.h>
# include <deque>
# include <map>
#include "ConfigurationServer.hpp"

class Epoll;

class Server
{
public:
// Constructors
	Server(void);
	Server(const Server &from);
// Destructors
	~Server(void);
// Overloaded operators
	Server &operator=(const Server &from);
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
	void handleNewClients(Epoll& epoll, int socket);

private:
	std::map<int, ConfigurationServer> _instances;
	std::deque<int> _clients;

#ifdef DEBUG
	std::map<int, char*> _clients_debug;
#endif

};

std::ostream& operator<<(std::ostream& stream, const Server& instance);

#endif
