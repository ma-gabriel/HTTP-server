#include "ARequest.hpp"

#include <iostream>
#include <sstream>
#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <sys/socket.h>

#ifndef COLORS

# define GREY "\033[1;30m"
# define RESET "\033[0m"

#endif

// Constructors
ARequest::ARequest(void)
{
	// std::cout << GREY << "ARequest constructor called" << RESET << std::endl;
	return;
}

ARequest::ARequest(const ARequest &from)
{
	// std::cout << GREY << "ARequest copy constructor called" << RESET << std::endl;
	*this = from;
	return;
}

// Destructors
ARequest::~ARequest(void)
{
	// std::cout << GREY << "ARequest destructor called" << RESET << std::endl;
	return;
}

// Overloaded operators
ARequest& ARequest::operator=(const ARequest &from)
{
	// std::cout << GREY << "ARequest '=' overload called" << RESET << std::endl;
	if (this != &from)
	{
	}
	return (*this);
}

// Getters

// Setters

// Public member functions
void ARequest::handleRequest(int sock)
{
	std::string request;
	char buff[8192];
	int readed;

	while ((readed = recv(sock, buff, sizeof(buff), MSG_DONTWAIT)) > 0)
		request.append(buff, readed);
	if (readed == -1 && request.length() == 0) {
		perror("read");
		return ;
	}

#ifdef DEBUG
	std::cout << buff << std::endl;
#endif

	// std::string req = buff;
	// if (req.find("STOP") != std::string::npos)
	// 	running = false;

	
	std::string resp = treatRequest(request);
	write(sock, resp.c_str(), resp.length());
}



#define HTML_START "<!DOCTYPE html>\n<html>\n<head>\n  <title>webserv</title>\n  <link rel=\"icon\" href=\"https://42.fr/wp-content/uploads/2021/07/cropped-42-favicon-acs-32x32.png\" sizes=\"32x32\" />\n</head>\n<body>"
#define HTML_END "</body>\n</html>"
#define HEADERS "Content-Type: text/html; charset=UTF-8\n\rContent-len: "

static size_t count_words(std::string line, std::string *buffer);
static void handle_errors(int status, std::string &body);

ARequest::myRequest ARequest::decomposeRequest(const std::string &request)
{
	myRequest res;

	std::size_t pos = request.find("\r\n");
	std::size_t pos2 = request.find("\r\n\r\n");
	if (pos2 == std::string::npos || pos == pos2){
		res.status = (pos2 == std::string::npos ? myRequest::NO_RNRN : myRequest::NO_HEADERS);
		return res;
	}

	std::string buffer[3];
	size_t nb_words = count_words(request.substr(0, pos), buffer);
	if (nb_words != 3){
		res.status = nb_words < 3 ? myRequest::UNDER_3 : myRequest::OVER_3;
		return res;
	}

	res.status = myRequest::IS_WORKING;
	res.method = buffer[0];
	res.target = buffer[1];
	res.http = buffer[2];
	res.headers = request.substr(pos + 2, pos2 - pos - 2);
	res.body = request.substr(pos2 + 4);
	return res;
}

/*
	Cette abomination est signée geymat

	J'aurai besoin d'avoir une version traitée du fichier de config avant de 
	pouvoir rendre une version plus proche de la definitive
	(ca n'est pas du tout le cas actuellement)
	C'est juste histoire d'avoir un brin de semblant de taritement de donnée

	TODO :
		-redirection
		-root
		-accessoirement toutes les regles de config)
		-traitement des POST (et PUT possiblement)
		-changer HTML_START qui contient <title> et <favicon>
		-voir pour enlever la struct de la classe ARequest (je trouvais que ca rendait bien bref)
		-...

	also:
		possibilité de tester avec nc pour une requete HTTP foireuse

	si il y a des questions message discord
*/
std::string ARequest::treatRequest(std::string str_request)
{
	std::string start_line, body, headers = HEADERS;

	myRequest request = decomposeRequest(str_request);

	// Ce serait bien de mettre les if qui suivent dans une fonction statique a part
	// mais ca necessite de sortir la struct de la classe (ce qui est loin d'etre exclut)
	if (request.status != 0) {
		start_line = "HTTP/1.1 400 Bad Request";
		handle_errors(request.status, body);
	} else if (request.method != "GET" && request.method != "POST") {
		start_line = "HTTP/1.1 501 Not Implemented";
		body = HTML_START "<h1> " + start_line.substr(9) + "</h1>\n\nMethod Not Implemented/recognized (for now only \"GET\" and \"POST\"\n\n" HTML_END;
	} else if (request.http != "HTTP/1.1") {
		start_line = "HTTP/1.1 505 HTTP Version Not Supported";
		body = HTML_START "<h1> " + start_line.substr(9) + "</h1>\n\nWe only work with HTTP/1.1\n\n" HTML_END;
	} else if (request.headers.find("Host:") == std::string::npos) {
		start_line = "HTTP/1.1 418 I'm a teapot"; // we'll need to change that one error
		body = HTML_START "<h1> " + start_line.substr(9) + "</h1>\n\nAccording to google we need \"Host:\" in the header\n\n" HTML_END;
	} else if (request.target != "/" && request.target != "/index.html") {
		start_line = "HTTP/1.1 404 Not found"; // we'll need to change that one too
		body = HTML_START "<h1> " + start_line.substr(9) + "</h1>\n\nYou are supposed to only go in specified website for now\n\n" HTML_END;
	} else {
		start_line = "HTTP/1.1 200 OK";
		body = HTML_START "\n\nHello, World\n\n" HTML_END;
	}

	{
		std::stringstream convert;
		convert << body.length();
		headers += convert.str();
	}
	std::string resp = start_line + "\n\r" + headers + "\n\r\n\r" + body;
	return resp;
}

// Overloaded print operator
// std::ostream& operator<<(std::ostream& stream, const ARequest& instance)
// {
// 	return (stream);
// }


static size_t count_words(std::string line, std::string *buffer) {
    std::istringstream iss(line);
    std::string word;
    size_t count = 0;
    while (iss >> word) {
		if (count < 3)
			buffer[count] = word;
		count++;
	}
	return count;
}

static void handle_errors(int status, std::string &body)
{
	// enum is_working { IS_WORKING = 0, OVER_3 = 1, UNDER_3 = 2,
	// 	NO_HEADERS = 3, NO_RNRN = 4};
	switch (status){
		case 1:
			body = HTML_START "<h1>400 Bad Request</h1>\n\nrequest line has too much data, supposed to be in the form GET / HTTP/1.1\n\n" HTML_END;
			break;
		case 2:
			body = HTML_START "<h1>400 Bad Request</h1>\n\nrequest line has not enough data, supposed to be in the form GET / HTTP/1.1\n\n" HTML_END;
			break;
		case 3:
			body = HTML_START "<h1>400 Bad Request</h1>\n\nHTTP Request has no headers... According to google we must have at least \"Host:\"\n\n" HTML_END;
			break;
		case 4:
			body = HTML_START "<h1>400 Bad Request</h1>\n\nHTTP Request doesn't have the famous \"\\r\\n\\r\\n\"\n\n" HTML_END;
			break;
	}
}
