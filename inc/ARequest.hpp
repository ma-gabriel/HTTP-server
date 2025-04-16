#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <ostream>

# include <string>
# include <map>

#ifdef DEBUG
#endif

class ARequest
{
public:
// Constructors
	ARequest(void);
	ARequest(const ARequest &from);
// Destructors
	~ARequest(void);
// Overloaded operators
	ARequest &operator=(const ARequest &from);
// Getters
// Setters
// Public member functions
	static void handleRequest(int sock);
	virtual void execRequest(void) = 0;

protected:
	std::map<std::string, std::string> _headers;

};

std::ostream& operator<<(std::ostream& stream, const ARequest& instance);

#endif
