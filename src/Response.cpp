
#include "webserv.hpp"
#include "Response.hpp"

//#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sstream>
#include <cstdlib>
#include "Epoll.hpp"

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
Response::Response(void)
{
    // std::cout << GREY << "Response constructor called" << RESET << std::endl;
    return;
}

Response::Response(const Response &from)
{
    // std::cout << GREY << "Response copy constructor called" << RESET << std::endl;
    *this = from;
    return;
}

// Destructors
Response::~Response(void)
{
    // std::cout << GREY << "Response destructor called" << RESET << std::endl;
    return;
}

// Overloaded operators
Response& Response::operator=(const Response &from)
{
    // std::cout << GREY << "Response '=' overload called" << RESET << std::endl;
    if (this != &from)
    {
    }
    return (*this);
}

// Getters

// Setters

// Public member functions

std::string Response::error(int error, std::string name, std::map<int, std::string> pages)
{
    std::string res;
    std::ifstream infile;
    std::map<int, std::string>::const_iterator file = pages.find(error);

    if (file != pages.end())
        infile.open(("." + file->second).c_str());
    
    if (!infile.is_open())
        res = static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
        "<!doctype html>\n<head>\n  <title>" << error << " " <<  name << \
        "</title>\n</head>\n<body>\n  <h1>" << name << "</h1>\n</body></html>")).str();
    else
        res.assign(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());


    return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
    "HTTP/1.1 " << error << " " << name << "\r\nContent-Type: text/html\r\nContent-Length: " \
    << res.length() << "\r\n\r\n")).str() + res;
}

std::string Response::createResponse(Request &req)
{
    struct  stat data;
    std::string res;
    std::string file = req.getPath();

    size_t query = file.find('?');
    if (query != std::string::npos) file.erase(query);
    file = file.substr(req.getConfig().getPath().length());
    std::string root = req.getConfig().getRoot().length() ? req.getConfig().getRoot() : "/";
    file = "." + root + file;

    if (access(file.c_str(), F_OK) != 0)
        return error(404, "Not Found", req.getConfig().getErrorPages());
    if (access(file.c_str(), R_OK) != 0)
        return error(403, "Forbidden", req.getConfig().getErrorPages());

    // NOT WORKING BELOW
    // _index is always empty
    if (stat(file.c_str(), &data) == 0
		&& S_ISDIR(data.st_mode) != 0)
    {
        std::vector<std::string>::const_iterator it = req.getConfig().getIndex().begin();
        for (; it != req.getConfig().getIndex().end(); it++){
            file = "." + root + "/" + *it;
            if (stat(file.c_str(), &data) == 0
                && S_ISREG(data.st_mode) != 0
                && access(file.c_str(), R_OK) == 0)
                break;
        }
        if (it == req.getConfig().getIndex().end())
            return error(404, "Not Found", req.getConfig().getErrorPages());
    }
    std::ifstream infile(file.c_str());
    if (!infile.is_open())
        return error(403, "Forbidden", req.getConfig().getErrorPages());
    else
        res.assign(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());

    return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " \
    << res.length() << "\r\n\r\n")).str() + res;
}

// Private member functions

void Response::sendResponse(int sock, std::string content)
{
    Epoll::instance().modFd(sock);
    Server::getResponses()[sock] = content;
}