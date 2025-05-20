#include "webserv.hpp"
#include "Response.hpp"

//#include <iostream>
#include <fstream>
#include <sys/socket.h>

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

Response::Response(Request* req) : _req(req)
{
	this->_version = req->getVersion();
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
void Response::sendBadRequest(int sock, std::string what)
{
	std::string resp;
	resp.append("HTTP/1.1 ");
	resp.append("400 Bad Request\r\n");

	resp.append("Content-Type: application/json\r\n");

	/*
	* 47 comes here from the skel of empty Bad Request response:
	*
	* {
	*	 "error": "Bad request",
	*	 "message": "",
	* }
	*/
	int content_lenght = what.length() + 47;
	resp.append("Content-Lenght: " + stoi(content_lenght) + "\r\n");

	resp.append("{\r\n");
	resp.append("\t\"error\": \"Bad request\",\r\n");
	resp.append("\t\"message\": \"" + what + "\",\r\n");
	resp.append("}\r\n");

	send(sock, resp.c_str(), resp.length(), 0);
}

std::string Response::createResponse(void)
{
	this->_status = "200 OK";
	this->_headers["Server"] = PROJECT_NAME;
	this->_body = openFile();
	this->_headers["Content-Lenght"] = stoi(this->_body.length());

	return (concatenateResponse());
}

std::string Response::openFile()
{
	std::string file_content;

	std::string file_path = this->_req->getPath();
	file_path.erase(file_path.begin());

	std::ifstream fstream(file_path.c_str(), std::ios::in);
	if (fstream.is_open())
	{
		std::string line;
		while (getline(fstream, line))
		{
			file_content.append(line);
			file_content.append("\n");
		}
	}
#ifdef DEBUG
	else
		std::cout << "Error opening file: " << this->_req->getPath() << std::endl;
#endif
	fstream.close();
	return (file_content);
}

// Private member functions
std::string Response::concatenateResponse(void)
{
	this->_raw.append(this->_version);
	this->_raw.append(" ");
	this->_raw.append(this->_status);
	this->_raw.append("\r\n");

	std::map<std::string, std::string>::iterator it;
	for (it = this->_headers.begin(); it != this->_headers.end(); it++)
	{
		this->_raw.append(it->first + ": " + it->second);
		this->_raw.append("\r\n");
	}

	this->_raw.append("\r\n");
	this->_raw.append(this->_body);

#ifdef DEBUG
	std::cout << "Response: " << std::endl;
	std::cout << this->_raw << std::endl;
#endif

	return (this->_raw);
}

// Overloaded print operator
// std::ostream& operator<<(std::ostream& stream, const Response& instance)
// {
// 	return (stream);
// }
