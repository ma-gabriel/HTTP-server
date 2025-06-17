
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <cstring>
#include <algorithm>
#include <signal.h>
#include <sstream>
#include <fcntl.h>
#include <sys/wait.h>

#include "CGI.hpp"
#include "Epoll.hpp"
#include "Request.hpp"
#include "Server.hpp"

static inline bool endsWith(const std::string &, const std::string &);
static std::string to_upper(std::string src);


CGI::CGI(){
	_toCGI[0] = _toCGI[1] = \
	_fromCGI[0] = _fromCGI[1] = -1;
}
CGI::~CGI(){}


#define ERROR_500(socket) write(socket, "HTTP/1.1 500 Internal Server Error\r\nContent-Type: text/html;\r\n\r\n<!doctype html>\n<head>\n  <title>500 Internal Server Error</title>\n</head>\n<body>\n  <h1>Internal Server Error</h1>\n  <p>a function failed due to kernel limitations</p>\n</body></html>", 246)	
#define ERROR_404(socket) write(socket, "HTTP/1.1 404 Not Found\r\nContent-Type: text/html;\r\n\r\n<!doctype html>\n<head>\n  <title>404 Not Found</title>\n</head>\n<body>\n  <h1>Not Found</h1>\n  <p>file requested not found</p>\n</body></html>", 191)
#define ERROR_403(socket) write(socket, "HTTP/1.1 403 Forbidden\r\nContent-Type: text/html;\r\n\r\n<!doctype html>\n<head>\n  <title>403 Forbidden </title>\n</head>\n<body>\n  <h1>Forbidden</h1>\n  <p>file requested not executable</p>\n</body></html>", 197)

bool doCGI(const Request &req)
{
	//TODO need to be in the config files
	// subject : "Execute CGI based on certain file extension (for example .php)."
	std::map<std::string, std::string> extensions;
	extensions[".py"] = "/usr/bin/python3";
	extensions[".php"] = "/usr/bin/php";
	extensions[".js"] = "/usr/bin/node";

	// TODO the second argument is from the config file
	std::string filePath = CGI::getActualPath(req.getPath(), "/Users/raphaelperrot/webserv2/CGI-scripts");
	std::string bin = CGI::checkExtensions(extensions, filePath);
	if (bin.empty())
		return false;

	if (!CGI::checkfilepresence(bin) || !CGI::checkfilepresence(filePath)){
		// TODO change macro to better error 404
		ERROR_404(req.getSock());
		return true;
	}

	if (!CGI::checkfileexec(bin) || !CGI::checkfileexec(filePath)){
		// TODO change macro to better error 403
		std::cerr << filePath << std::endl;
		ERROR_403(req.getSock());
		return true;
	}
	CGI::launch(req, bin, filePath);
	return true;
}

void CGI::launch(const Request &req, const std::string &binPath, std::string filePath)
{
	CGI handle;

	handle._body = req.getBody();
	handle._query = CGI::getQuery(req.getPath());
	handle._binPath = binPath;
	handle._socket = dup(req.getSock());
	if (handle._socket == -1) {
		ERROR_500(handle._socket);
		return ;
	}
	handle._fileLocation = filePath.substr(0, filePath.find_last_of('/'));
	handle._binName = binPath.find('/') ? binPath.substr(binPath.find_last_of('/')) : binPath;
	handle._fileName = filePath.substr(filePath.find_last_of('/') + 1);
	if (pipe(handle._toCGI) || pipe(handle._fromCGI)) {
		close(handle._toCGI[0]); close(handle._toCGI[1]);
		// TODO change macro to better error 500
		ERROR_500(handle._socket);
		close(handle._socket);
		return ;
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(handle._toCGI[0]); close(handle._toCGI[1]);
		close(handle._fromCGI[0]); close(handle._fromCGI[1]);
		// TODO change macro to better error 500
		ERROR_500(handle._socket);
		close(handle._socket);
		return ;
	}
	if (pid == 0)
		handle.childExec(req.getHeaders());
	else
		handle.parentHandling(pid);
}

static CGI::infos generate(std::time_t ts, pid_t pid, int fd, std::string body)
{
	CGI::infos buff;
	buff.timestamp = ts;
	buff.pid = pid;
	buff.output_fd = fd;
	buff.body = body;
	return buff;
}

void CGI::parentHandling(pid_t pid)
{
	close(_toCGI[0]);
	close(_fromCGI[1]);
	int flags;
	if ((flags = fcntl(_toCGI[1], F_GETFL, 0)) == -1 || fcntl(_toCGI[1], F_SETFL, flags | O_NONBLOCK) == -1
		|| (flags = fcntl(_fromCGI[0], F_GETFL, 0)) == -1 || fcntl(_fromCGI[0], F_SETFL, flags | O_NONBLOCK) == -1){
		close(_fromCGI[0]);
		close(_toCGI[1]);
		kill(pid, SIGKILL);
		//TODO change macro to error 500
		ERROR_500(_socket);
		close(_socket);
		return ;
	}
	//if this one fails (like kernel error) the CGI won't have a body, at worst, time-out
	Server::instance().addCGI(_toCGI[1], generate(std::time(NULL), -1, -1, _body),  false);
	//TODO change macro to error 500
	if (!Server::instance().addCGI(_fromCGI[0], generate(std::time(NULL), pid, _socket, ""), true)){
		kill(pid, SIGKILL);
		ERROR_500(_socket);
		close(_socket);
		return ;
	}
}

void CGI::childExec(std::map<std::string,std::string> envRaw)
{
	bool tmp = dup2(_toCGI[0], 0) == -1 || dup2(_fromCGI[1], 1) == -1;
	close(_toCGI[0]);
	close(_toCGI[1]);
	close(_fromCGI[0]);
	if (tmp) {
		ERROR_500(_fromCGI[1]);
		close(_fromCGI[1]);
		std::exit(1);
	}
	close(_fromCGI[1]);

	// not to remove, according to https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.7
	// the server need this flag even if empty
	envRaw["QUERY_STRING"] = _query;
	char **env = createEnvCGI(envRaw);
	char **argv = createArgvCGI(_binName, _fileName);

	// https://datatracker.ietf.org/doc/html/rfc3875#section-7.1
	// we need to chdir into the directory of the executable
	if (chdir(_fileLocation.c_str()) != -1){
		execve(_binPath.c_str(), argv, env);
	}
	ERROR_500(1);
	deleteEnvCGI(env);
	deleteArgvCGI(argv);
	std::exit(1);
}

ssize_t CGI::flush(int fd, std::string text){
	size_t body = text.find("\r\n\r\n");
	if (body == std::string::npos) {
		text = static_cast< std::ostringstream & >(( std::ostringstream() << std::dec << \
			"HTTP/1.1 200 OK\r\nContent-Type: text/html\r\nContent-Length: " << text.length() << "\r\n\r\n")).str() + text;

	} else {
		size_t status = text.find("Status: ");
		size_t length = text.find("Content-Length: ");
		size_t type = text.find("Content-Type: ");
		if (status != std::string::npos && status < body){
			std::string status_line = "HTTP/1.1 " + text.substr(status + 8, text.find("\r\n", status) - 8);
			text.erase(status, text.find("\r\n", status));
			text = status_line + text;
		}
		else text = "HTTP/1.1 200 OK\r\n" + text;
		if (type == std::string::npos || type > body){
			text.insert(text.find("\r\n\r\n") + 2, "Content-Type: text/html\r\n");
		}
		if (length == std::string::npos || body == std::string::npos || length > body){
			text.insert(text.find("\r\n\r\n") + 2, static_cast< std::ostringstream & >((std::ostringstream() << std::dec \
			<< "Content-Length: " << text.length() - text.find("\r\n\r\n") - 4 << "\r\n")).str());
		}
	}
	ssize_t total = 0;
	while (text.length()) {
		ssize_t len = send(fd, text.c_str(), text.length(), 0);
		if (len == -1) return -1;
		text = text.substr(len);
		total += len;
	}
	return total;
}

/**
 * remove anything from the after any ? character found 
 * 
 * @param path the path found in the first line of the HTTP request
 * @param root (optionnal) the root from the config file
 * @return root (if present) followed by shortened path
 */
std::string CGI::getActualPath(const std::string &path, const std::string &root){
	return root + (path.find('?') != std::string::npos ? std::string(path).erase(path.find('?')) : path);
}

/** 
 * check if the file at path exist and can be executed
 * 
 * @param  file the file (with path) in question
 * @return true if yes, false if not
 */
bool CGI::checkfileexec(const std::string &file){
	struct  stat data;

	return	stat(file.c_str(), &data) == 0		// check if exists
			&& S_ISREG(data.st_mode) != 0		// check if is a file
			&& access(file.c_str(), X_OK) == 0;	// check if executable
}

bool CGI::checkfilepresence(const std::string &file){
	return access(file.c_str(), F_OK) == 0;
}

std::string CGI::getQuery(const std::string &address){
	return address.find("?") == std::string::npos ? std::string() : address.substr(0, address.find("?"));
}

/** 
 * @param fullString the string to check
 * @param ending the potential end of fullString
 * @return true if ends with, false if not
 */
static inline bool endsWith(const std::string& fullString, const std::string& ending) {
    return ending.size() > fullString.size() ? false : !fullString.compare(fullString.size() - ending.size(), ending.size(), ending);
}

/**
 * find the binary corresponding of the extension of the file in path 
 * 
 * @param extensions map of, for instance {{".py", "/usr/bin/py"}}
 * @param path path or file to check 
 * @return second value from the map if found, empty string if not
 */
std::string CGI::checkExtensions(std::map<std::string,std::string> extensions, const std::string &path){
	for (std::map<std::string,std::string>::iterator it = extensions.begin(); it != extensions.end(); it++)
		if (endsWith(path, it->first) && path != it->first) return it->second;
	return std::string();
}

/**
 * create a new string (not allocated) with all capital
 * 
 * @param src original string
 * @return new string
 */
static std::string to_upper(std::string src) {
    std::transform(src.begin(), src.end(), src.begin(), static_cast<int(*)(int)>(std::toupper));
    return src;
}

// execve("/usr/bin/php-cgi", { "php-cgi", "/index.php", NULL}, envp);

/**
 * create the argv of the CGI 
 * 
 * @param bin should be the binary name, without /usr/bin/ before
 * @param bin should be the file name, without /CGI-stuff/ before
 * @return the argv
 */
char **CGI::createArgvCGI(const std::string &bin, const std::string &file)
{
	char **argv = new char *[3];
	argv[0] = new char[bin.length() + 1];
	std::memcpy(argv[0], bin.c_str(), bin.length() + 1);
	argv[1] = new char[file.length() + 1];
	std::memcpy(argv[1], file.c_str(), file.length() + 1);
	argv[2] = NULL;
	return argv;
}


/**
 * create the environnement of the CGI 
 * 
 * @param headers headers from the HTTP request
 * @return environnement to give to the CGI
 */
char	**CGI::createEnvCGI(std::map<std::string, std::string> headers){
	char **env = new char*[headers.size() + 1];
	size_t i = 0;
	for (std::map<std::string, std::string>::iterator it = headers.begin(); it != headers.end(); it++){
		std::string line = to_upper(it->first) + "=" + it->second;
		env[i] = new char[line.length() + 1];
		std::memcpy(env[i++], line.c_str(), line.length() + 1);
	}
	env[i] = NULL;
	return env;
}


/**
 * delete the environnement of the CGI 
 * 
 * @param env the CGI environnement
 */
void CGI::deleteEnvCGI(char **env){
	for (size_t i = 0; env[i]; i++)
		delete[] env[i];
	delete[] env;
}

/**
 * delete the argv of the CGI 
 * 
 * @param env the CGI argv
 */
void CGI::deleteArgvCGI(char **env){
	deleteEnvCGI(env);
}
