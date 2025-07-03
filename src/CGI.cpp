
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

#include "Response.hpp"
#include "CGI.hpp"
#include "Parser.hpp"
#include "Epoll.hpp"
#include "Request.hpp"
#include "Server.hpp"

static inline bool endsWith(const std::string &, const std::string &);
static std::string to_upper(std::string src);
static std::map<std::string, std::string> decompose_cgi(std::vector<std::string> cgis);

CGI::CGI(){
	_toCGI[0] = _toCGI[1] = \
	_fromCGI[0] = _fromCGI[1] = -1;
}
CGI::~CGI(){}

bool doCGI(const Request &req)
{
	std::map<std::string, std::string> extensions;
	Location config = req.getConfig();

	std::string filePath = req.getPath().substr(config.getPath().length());
	filePath = CGI::getActualPath(filePath, config.getRoot());

	try {
		extensions = decompose_cgi(config.getCgi());
	} catch(...){
		return false;
	}


	std::string bin = CGI::checkExtensions(extensions, filePath);
	if (bin.empty())
		return false;
	if (!CGI::checkfilepresence(bin) || bin[0] != '/' || !CGI::checkfilepresence(filePath)){
		Response::sendResponse(req.getSock(), Response::error(404, "Not found", config.getErrorPages()));
		return true;
	}

	if (!CGI::checkfileexec(bin) || !CGI::checkfileexec(filePath)){
		Response::sendResponse(req.getSock(), Response::error(403, "Forbidden", config.getErrorPages()));
		return true;
	}
	CGI::launch(req, bin, filePath, config);
	return true;
}

void CGI::launch(const Request &req, const std::string &binPath, std::string filePath, Location &config)
{
	CGI handle;

	handle._pathInfo = CGI::getPathInfo(req.getPath(), decompose_cgi(config.getCgi()));
	handle._body = req.getBody();
	handle._query = CGI::getQuery(req.getPath());
	handle._binPath = binPath;
	handle._config = config;
	handle._socket = req.getSock();
	handle._fileLocation = filePath.substr(0, filePath.find_last_of('/'));
	handle._binName = binPath.find('/') != std::string::npos ? binPath.substr(binPath.find_last_of('/')) : binPath;
	handle._fileName = filePath.substr(filePath.find_last_of('/') + 1);
	if (pipe(handle._toCGI) || pipe(handle._fromCGI)) {
		close(handle._toCGI[0]); close(handle._toCGI[1]);
		Response::sendResponse(req.getSock(), Response::error(500, "Internal Server Error", config.getErrorPages()));
		return ;
	}

	pid_t pid = fork();
	if (pid == -1) {
		close(handle._toCGI[0]); close(handle._toCGI[1]);
		close(handle._fromCGI[0]); close(handle._fromCGI[1]);
		Response::sendResponse(req.getSock(), Response::error(500, "Internal Server Error", config.getErrorPages()));
		return ;
	}
	if (pid == 0){
		std::map<std::string,std::string> env = req.getHeaders();
		handle.add_headers(env, req);
		handle.childExec(env);
	}
	else
		handle.parentHandling(pid);
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
		waitpid(pid, NULL, 0);
		Response::sendResponse(_socket, Response::error(500, "Internal Server Error", _config.getErrorPages()));
		return ;
	}
	if (_body.length() > 0)
		Server::instance().addCGI(_toCGI[1], CGI::infos(std::time(NULL), -1, _socket, _body, _config), false);
	else
		close(_toCGI[1]);
	Server::instance().addCGI(_fromCGI[0], CGI::infos(std::time(NULL), pid, _socket, "", _config), true);
}

void CGI::childExec(std::map<std::string,std::string> &envRaw)
{
	bool tmp = dup2(_toCGI[0], 0) == -1 || dup2(_fromCGI[1], 1) == -1;
	close(_toCGI[0]);
	close(_toCGI[1]);
	close(_fromCGI[0]);
	if (tmp) {
		std::string res = Response::error(500, "Internal Server Error", _config.getErrorPages());
		write(_fromCGI[1], res.c_str(), res.length());
		close(_fromCGI[1]);
		std::exit(1);
	}
	close(_fromCGI[1]);

	char **env = createEnvCGI(envRaw);
	char **argv = createArgvCGI(_binName, _fileName);

	// https://datatracker.ietf.org/doc/html/rfc3875#section-7.1
	// we need to chdir into the directory of the executable
	if (chdir(_fileLocation.c_str()) != -1){
		execve(_binPath.c_str(), argv, env);
	}

	std::string res = Response::error(500, "Internal Server Error", _config.getErrorPages());
	write(_fromCGI[1], res.c_str(), res.length());
	deleteEnvCGI(env);
	deleteArgvCGI(argv);
	std::exit(1);
}

void CGI::flush(int fd, std::string text){
    try {
        Request request(text);
        std::map<std::string, std::string>::const_iterator status = request.getHeaders().find("STATUS");
        std::map<std::string, std::string>::const_iterator  length = request.getHeaders().find("CONTENT-LENGTH");
        std::map<std::string, std::string>::const_iterator  type = request.getHeaders().find("CONTENT-TYPE");
        if (status != request.getHeaders().end()){
            std::string status_line = status->second;
            size_t i = findInsensitive(text, "\r\nStatus:");
            if (i == std::string::npos)
                i = findInsensitive(text, "Status:");
            text.erase(i, text.find("\r\n", i) - i + 2);
            text = "HTTP/1.1 " + status_line + text ;
        }
        else
            text = "HTTP/1.1 200 OK\r\n" + text;
        if (length == request.getHeaders().end()){
            text.insert(text.find("\r\n\r\n") + 2, static_cast< std::ostringstream & >((std::ostringstream() << std::dec \
        << "Content-Length: " << text.length() - text.find("\r\n\r\n") - 4 << "\r\n")).str());
        }
        if (type == request.getHeaders().end())
            text = Response::error(502, "Bad Gateway", Server::getRequests().at(fd).getConfig().getErrorPages());
    }
    catch (...) {
        text = Response::error(502, "Bad Gateway", Server::getRequests().at(fd).getConfig().getErrorPages());
    }
    std::cout << text;
	Response::sendResponse(fd, text);
}


/**
 * remove anything from the after any ? character found 
 * 
 * @param path the path found in the first line of the HTTP request
 * @param root (optionnal) the root from the config file
 * @return root (if present) followed by shortened path
 */
std::string CGI::getActualPath(const std::string &path, const std::string &root){
	return "." + root + (path.find('?') != std::string::npos ? std::string(path).erase(path.find('?')) : path);
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
	return address.find("?") == std::string::npos ? std::string() : address.substr(address.find("?") + 1);
}


std::string CGI::getPathInfo(std::string address, std::map<std::string,std::string> extensions){
	size_t len = address.find("?");
		if (len != std::string::npos) address.erase(len);
	for (std::map<std::string,std::string>::iterator it = extensions.begin(); it != extensions.end(); it++)
	{
		size_t ext = 0;
		do ext = address.find(it->first + "/", ext);
		while (ext != std::string::npos && address[ext - 1] == '/');
		if (ext != std::string::npos) return address.substr(ext + it->first.length() + 1);
		if (endsWith(address, it->first) && address != it->first) return std::string();
	}
	return std::string();
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
std::string CGI::checkExtensions(std::map<std::string,std::string> extensions, std::string &path){
	for (std::map<std::string,std::string>::iterator it = extensions.begin(); it != extensions.end(); it++)
	{
		if (endsWith(path, it->first) && path != it->first) return it->second;
		size_t ext = 0;
		do ext = path.find(it->first + "/", ext);
		while (ext != std::string::npos && path[ext - 1] == '/');
		if (ext != std::string::npos) {
			path.erase(ext + it->first.length());
			return it->second;
		}
	}
	return std::string();
}

static char bettertoupper(char c){
	return c == '-' ? '_' : std::toupper(c);
}

/**
 * create a new string (not allocated) with all capital
 * 
 * @param src original string
 * @return new string
 */
static std::string to_upper(std::string src) {
    std::transform(src.begin(), src.end(), src.begin(), bettertoupper);
    return src;
}

void CGI::add_headers(std::map<std::string,std::string> &env, const Request &req)
{
	std::map<std::string,std::string> new_env;
    for (std::map<std::string, std::string>::iterator it = env.begin(); it != env.end(); ++it) {
        std::string first = to_upper(it->first);
		// following "if" is according to the CGI 1.1 norm
		if (first != "CONTENT_TYPE" && first != "CONTENT_LENGTH")
        	first.insert(0, "HTTP_");
        new_env[first] = it->second;
    }
    env = new_env;
	env["SERVER_PROTOCOL"] = req.getVersion();
	env["REQUEST_METHOD"] = req.getMethod();
	env["QUERY_STRING"] = _query;
	env["PATH_INFO"] = _pathInfo;
	env["SCRIPT_NAME"] = _fileLocation + "/" + _fileName;
	env["GATEWAY_INTERFACE"] = "CGI/1.1";
};


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
		std::string line = it->first + "=" + it->second;
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

static std::map<std::string, std::string> decompose_cgi(std::vector<std::string> cgis){
	std::map<std::string, std::string> res;
	for (std::vector<std::string>::iterator it = cgis.begin(); it != cgis.end(); it++){
		size_t sep = (*it).find(':');
		if (sep == std::string::npos)
			throw ("random bullshit let's gooooooo");
		res[(*it).substr(0, sep)] = (*it).substr(sep + 1);
	}
	return res;
}