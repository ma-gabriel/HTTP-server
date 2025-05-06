#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <ostream>

# include <string>
# include <map>

#ifdef DEBUG
#endif

#include <iostream>

class Request
{
public:
// Constructors
	Request(int sock);
	Request(const Request &from);
// Destructors
	~Request(void);
// Overloaded operators
	Request &operator=(const Request &from);
// Getters
// Setters
// Public member functions
	void parseRequest(void);
// Public exception
	class BadRequestException : public std::exception
	{
	public:
		BadRequestException(const char* why) : _why(why) {};
		virtual const char* what() const throw() {
			return (this->_why);
		}
	private:
		const char* _why;
	};

private:
// Private constructors
	Request(void);
// Private member functions
	void parseFirstLine(void);
	void checkFirstLine(void);
	bool checkMethod(void);
	void extractHeaders(void);
	std::string extractHeaderKey(std::string& line);
	std::string extractOneLine(void);
// Private attributs
	int			_sock;
	std::string	_raw;
	std::string	_method; 
	std::string	_path;
	std::string	_version;
	std::map<std::string, std::string> _headers;
	std::string	_body;

};

std::ostream& operator<<(std::ostream& stream, const Request& instance);

#endif
