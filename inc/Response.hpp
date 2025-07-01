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
    static std::string createResponseRedirect(Request &request);
    static bool handleUpload(Request &req);
    static bool removeUpload(Request &req);

private:
// Private constructor
// Constructors
    static std::string createListingFile(std::string pathFile, std::string pathUrl);
    Response(const Response &from);
    Response &operator=(const Response &from);
// Destructors
    virtual ~Response(void) = 0;
    Response(void);
    static std::string createHeaderHtml(std::string title);
    static std::string createResponsePage(size_t code, std::string infoCode, std::string body);
};

std::ostream& operator<<(std::ostream& stream, const Response& instance);

#endif