#ifndef REQUEST_HPP
# define REQUEST_HPP

# include <ostream>

# include <string>
# include <map>
# include <ctime>

#ifdef DEBUG
#endif

# include <iostream>
# include "EHttpMethodeEnum.hpp"
# include "Location.hpp"

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
    int getSock(void) const;
    std::string getVersion(void) const;
    std::string getPath(void) const;
    std::string getBody(void) const;
    time_t getTime(void) const;
    std::string getMethod(void) const;
    Location getConfig(void) const;
    std::map<std::string, std::string> getHeaders(void) const;
    // Setters
    // Public member functions
    void parseRequest(void);
    void read(void);
    bool isValid(void);
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
    Request(void);
    // Private constructors
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
    std::time_t _time;
    Location _config;
};

std::ostream& operator<<(std::ostream& stream, const Request& instance);

#endif