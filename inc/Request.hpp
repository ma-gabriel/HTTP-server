#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <ostream>

# include <string>
# include <map>

#ifdef DEBUG
#endif

class Request
{
public:
// Constructors
	Request(void);
	Request(int sock);
	Request(const Request &from);
// Destructors
	~Request(void);
// Overloaded operators
	Request &operator=(const Request &from);
// Getters
// Setters
// Public member functions
	void parseFirstLine(void);
	void checkFirstLine(void);
	void parseRequest(const std::string& request);
	bool checkMethod(void);
	void handleRequest(void);
	static std::string treatRequest(std::string request);
	// virtual void execRequest(void) = 0;
	std::string extractOneLine(void);

protected:
	int			_sock;
	std::string	_raw;
	std::string	_method; 
	std::string	_path;
	std::string	_version;
	std::map<std::string, std::string> _headers;
	std::string	_body;

	class BadRequestExeception : public std::exception
	{
	public:
		virtual const char* what() const throw() {
			return ("Exception: Bad request");
		}
	};
};

std::ostream& operator<<(std::ostream& stream, const Request& instance);

#endif
