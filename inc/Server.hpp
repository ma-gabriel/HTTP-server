#ifndef SOCKET_HPP
# define SOCKET_HPP

# include <ostream>
# include <sys/socket.h>
# include <netinet/in.h>

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
	int getFd(void) const;
// Setters
// Public member functions

private:
	int _fd;
};

std::ostream& operator<<(std::ostream& stream, const Server& instance);

#endif
