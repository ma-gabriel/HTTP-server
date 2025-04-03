#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <ostream>
# include <sys/socket.h>
# include <sys/types.h>
# include <netinet/in.h>
# include <errno.h>

class Epoll;

class Socket
{
public:
// Constructors
	Socket(short port);
	Socket(const Socket &from);
// Destructors
	~Socket(void);
// Overloaded operators
	Socket &operator=(const Socket &from);
// Getters
	int getFd(void) const;
// Setters
// Public member functions
	void handleNewClients(Epoll& epoll) const;
	static void setSocketNonBlocking(int sfd);

private:
	int _fd;
};

std::ostream& operator<<(std::ostream& stream, const Socket& instance);

#endif
