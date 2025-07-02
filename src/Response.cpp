
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
#include <climits>
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

 std::string Response::createListingFile(std::string pathFile, std::string pathUrl, Request &req)
{
   std::multimap<std::string, unsigned char> filesInfo;
   DIR *dir;
    struct dirent *entry;
    std::string header = createHeaderHtml("Index of " + pathUrl);
   dir = opendir(pathFile.c_str());
   std::cout << "Listing directory: " << pathFile << std::endl;
    if (!dir)
         return error(404, "Not Found", std::map<int, std::string>());
    while ((entry = readdir(dir))) {
        filesInfo.insert(std::make_pair(entry->d_name, entry->d_type));
    }
    std::string headerHtml = createHeaderHtml(pathFile);
    if (req.getHeaders().find("Host") == req.getHeaders().end()) {
        return Response::error(400, "Bad Request", req.getConfig().getErrorPages());
    }
    if (!pathUrl.empty() && pathUrl[pathUrl.size() - 1] != '/')
        pathUrl += '/';
    std::string body = "<body>\n<h1>\nIndex of " + pathUrl + "\n</h1>\n<ul>\n";
    for (std::multimap<std::string, unsigned char>::const_iterator it = filesInfo.begin();
         it != filesInfo.end(); ++it) {
        if (it->first == "." || it->first == "..")
            continue;
        body += "<li>\n<a href=\"http://" + req.getHeaders().at("Host") + pathUrl  + it->first +  "\">" + it->first + " ";
        if (it->second == DT_DIR)
            body += "\xF0\x9F\x93\x81</a>\n</li>\n";
        else
            body += "\xF0\x9F\x93\x84</a>\n</li>\n";
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
    return "<!doctype html>\n<head>\n<meta charset=\"UTF-8\">  <title>" + title + "</title>\n</head>\n";
}

static std::string extract_cookies(const std::map<std::string, std::string> &headers){

    std::map <std::string, std::string>::const_iterator it = headers.find("Cookie");
    if (it == headers.end())
        return "Set-Cookie: nb_static_visits=1; Max-Age=3600; Path=/";
    size_t i = it->second.find("nb_static_visits=");
    if (i == std::string::npos)
        return "Set-Cookie: nb_static_visits=1; Max-Age=3600; Path=/";
    size_t val = std::strtoul (it->second.substr(i + 17).c_str(), NULL, 0);
    if (val == ULONG_MAX)
        return "Set-Cookie: nb_static_visits=1; Max-Age=3600; Path=/";
    return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
    "Set-Cookie: nb_static_visits=" << val + 1 << "; Max-Age=3600; Path=/")).str();
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
        std::string fileTemp;
        std::vector<std::string>::const_iterator it = req.getConfig().getIndex().begin();
        for (; it != req.getConfig().getIndex().end(); it++){
            fileTemp = "." + root + "/" + *it;
            if (stat(fileTemp.c_str(), &data) == 0
                && S_ISREG(data.st_mode) != 0
                && access(fileTemp.c_str(), R_OK) == 0)
                break;
        }
        if (it == req.getConfig().getIndex().end() && req.getConfig().isAutoIndex() == FALSE)
            return error(404, "Not Found", req.getConfig().getErrorPages());
        if (req.getConfig().isAutoIndex() == TRUE)
            return createListingFile(file, req.getPath(), req);
    }
    std::ifstream infile(file.c_str());
    if (!infile.is_open())
        return error(403, "Forbidden", req.getConfig().getErrorPages());
    else
        res.assign(std::istreambuf_iterator<char>(infile), std::istreambuf_iterator<char>());

    std::string cookies = extract_cookies(req.getHeaders());

    return static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
    "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " \
    << res.length() << "\r\n" << cookies << "\r\n\r\n")).str() + res;
}

bool Response::handleUpload(Request &req)
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
    std::string delim = "--" + boundary;
    std::string end_delim = delim + "--";
    size_t pos = 0;
    while (true) {
        size_t start = req.getBody().find(delim + "\r\n", pos);
        if (start == std::string::npos)
            break;
        start += delim.length() + 2;
        size_t next = req.getBody().find(delim, start);
        if (next == std::string::npos)
            break;
        std::string part = req.getBody().substr(start, next - start);
        size_t header_end = part.find("\r\n\r\n");
        if (header_end == std::string::npos){
            Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
            return true;
        }
        std::string headers = part.substr(0, header_end);
        std::string content = part.substr(header_end + 4); // skip \r\n\r\n

        size_t cd_pos = headers.find("Content-Disposition:");
        if (cd_pos == std::string::npos){
            Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
            return true;
        }
        size_t name_pos = headers.find("filename=\"", cd_pos);
        if (name_pos == std::string::npos){
            Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
            return true;
        }

        name_pos += 10; // move past filename="
        size_t name_end = headers.find("\"", name_pos);
        if (name_end == std::string::npos){
            Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
            return true;
        }

        files[headers.substr(name_pos, name_end - name_pos)] = content;

        pos = next;
    }
    if (req.getBody().compare(pos, end_delim.length() + 2, end_delim + "\r\n")){
        Response::sendResponse(req.getSock(), Response::error(400, "Bad Request", req.getConfig().getErrorPages()));
        return true;
    }

    for (std::map<std::string, std::string>::iterator it = files.begin(); it != files.end(); it++){
        if (!access(("./static/uploads/" + it->first).c_str(), F_OK)){
            Response::sendResponse(req.getSock(), Response::error(409, "Conflict", req.getConfig().getErrorPages()));
            return true;
        }
    }
    for (std::map<std::string, std::string>::iterator it = files.begin(); it != files.end(); it++){
        std::ofstream outfile(("./static/uploads/" + it->first).c_str());
        if (outfile)
            outfile << it->second;
        outfile.close();
    }
    Response::sendResponse(req.getSock(), Response::error(201, "Created", req.getConfig().getErrorPages()));
    return true;
}


bool Response::removeUpload(Request &req)
{
    if (req.getMethod() != "DELETE")
        return false;
    std::string file = req.getPath();
    size_t i = req.getPath().find_last_of("/");
    if (i != std::string::npos)
        file = "static/uploads/" + req.getPath().substr(i + 1);
    size_t query = file.find('?');
    if (query != std::string::npos)
        file.erase(query);
    if (access(file.c_str(), F_OK)){
        Response::sendResponse(req.getSock(), Response::error(404, "Not Found", req.getConfig().getErrorPages()));
        return true;
    }
    std::remove(file.c_str());
    if (access(file.c_str(), F_OK))
        Response::sendResponse(req.getSock(), Response::error(204, "No Content", req.getConfig().getErrorPages()));
    else    
        Response::sendResponse(req.getSock(), Response::error(500, "Internal Server Error", req.getConfig().getErrorPages()));
    return true;
}


void Response::sendResponse(int sock, std::string content)
{
    std::cout  << content << std::endl;
    Epoll::instance().modFd(sock);
    Server::getResponses()[sock] = content;
    Server::getRequests().erase(sock);
	Server::instance().killCGIsock(sock);
}