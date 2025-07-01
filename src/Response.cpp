
#include "webserv.hpp"
#include "Response.hpp"

//#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sstream>
#include <cstdlib>
#include <dirent.h>
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

 std::string Response::createListingFile(std::string pathFile, std::string pathUrl)
{
   std::multimap<std::string, unsigned char> filesInfo;
   DIR *dir;
    struct dirent *entry;
    std::string header = createHeaderHtml("Index of " + pathUrl);
   dir = opendir(pathFile.c_str());
    if (!dir)
         return error(404, "Not Found", std::map<int, std::string>());
    while ((entry = readdir(dir))) {
        filesInfo.insert(std::make_pair(entry->d_name, entry->d_type));
    }
    std::string headerHtml = createHeaderHtml(pathFile);
    std::string body = "<body>\n<h1>\nIndex of " + pathUrl + "\n</h1>\n<ul>\n";
    for (std::multimap<std::string, unsigned char>::const_iterator it = filesInfo.begin();
         it != filesInfo.end(); ++it) {
        if (it->first == "." || it->first == "..")
            continue;
        body += "<li>" + it->first + " ";
        if (it->second == DT_DIR)
            body += "\xF0\x9F\x93\x81</li>\n";
        else
            body += "\xF0\x9F\x93\x84</li>\n";
    }
    body += "</ul>\n</body>\n";
    return createResponsePage(200, "OK", header + body);
}

std::string Response::createResponsePage(size_t code, std::string infoCode, std::string body)
{
    return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
    "HTTP/1.1 " << code << " " << infoCode << "\r\nContent-Type: text/html\r\nContent-Length: " \
    << body.length() << "\r\n\r\n")).str() + body;
}

std::string Response::createHeaderHtml(std::string title)
{
    return "<!doctype html>\n<head>\n<meta charset=\"UTF-8\">  <title>" + title + "</title>\n<></head>\n";
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
        if (req.getConfig().getIndex().empty() && req.getConfig().isAutoIndex() == TRUE)
            return createListingFile(file, req.getPath());
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

bool HandleUpload(Request &req)
{
    if (req.getMethod() != "POST" && req.getMethod() != "PUT")
        return false;
    std::map<std::string, std::string>::const_iterator type_it = req.getHeaders().find("Content-Type");
    if (type_it == req.getHeaders().end())
        return false;
    std::string type = type_it->second;
    size_t i = type_it->second.find(";");
    if (i != std::string::npos) type.erase(i);
    if (type != "multipart/form-data" && type != "form-data")
        return false;
    size_t b = type_it->second.find("boundary=");
    if (b == std::string::npos){
        Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
        return true;
    }
    i = type_it->second.find(";", b);
    if (i == std::string::npos) i = type_it->second.length();
    std::string boundary = type_it->second.substr(b + 9, i - (b + 9));
    if (boundary.empty()){
        Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
        return true;
    }

    std::map<std::string, std::string> files;
    return true;
}
// Private member functions

void Response::sendResponse(int sock, std::string content)
{
    Epoll::instance().modFd(sock);
    Server::getResponses()[sock] = content;
    Server::getRequests().erase(sock);
}