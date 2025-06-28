#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <ostream>
# include <map>

# include "Request.hpp"

class Response
{
public:
// Public member functions
    static std::string error(int error, std::string name, std::map<int, std::string> pages);
    static std::string createResponse(Request &req);
    static void sendResponse(int sock, std::string content);

private:
// Private constructor
// Constructors
    Response(const Response &from);
    Response &operator=(const Response &from);
// Destructors
    virtual ~Response(void) = 0;
    Response(void);
};

std::ostream& operator<<(std::ostream& stream, const Response& instance);

#endif