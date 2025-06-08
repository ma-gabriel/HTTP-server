#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <ostream>
# include <map>

# include "Request.hpp"

class Response
{
public:
// Constructors
    Response(Request* req);
    Response(const Response &from);
// Destructors
    ~Response(void);
// Overloaded operators
    Response &operator=(const Response &from);
// Getters
// Setters
// Public member functions
    static void sendBadRequest(int sock, std::string what);
    std::string createResponse(void);

private:
// Private constructor
    Response(void);
// Private member functions
    std::string concatenateResponse(void);
    std::string openFile(void);
// Private attributs
    Request*	_req;
    std::string	_raw;
    std::string	_version;
    std::string _status;
    std::map<std::string, std::string> _headers;
    std::string	_body;
};

std::ostream& operator<<(std::ostream& stream, const Response& instance);

#endif