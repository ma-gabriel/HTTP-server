#ifndef SERVER_HPP
# define SERVER_HPP

# include <ostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <errno.h>
# include <deque>

class Epoll;

class Server
{
public:
// Constructors
	Server(short port);
	Server(const Server &from);
// Destructors
	~Server(void);
// Overloaded operators
	Server &operator=(const Server &from);
// Getters
	int getSocketFd(void) const;
// Setters
// Public member functions
	void handleNewClients(Epoll& epoll);
	static void setServerNonBlocking(int sfd);

private:
	int _socket;
	std::deque<int> _clients;
};

std::ostream& operator<<(std::ostream& stream, const Server& instance);

#endif
