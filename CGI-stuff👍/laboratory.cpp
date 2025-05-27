
#include <sys/stat.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>
#include <map>
#include <vector>
#include <string.h>
#include <cstring>
#include <algorithm>
#include <fcntl.h>

#include "CGI.hpp"

static std::string getActualPath(const std::string &, const std::string &);
static inline bool endsWith(const std::string &, const std::string &);
static std::string to_upper(std::string src);


CGI::CGI(){
	_toCGI[0] = _toCGI[1] = _fromCGI[0] = _fromCGI[1] = -1;
}
CGI::~CGI(){}




bool doCGI(std::string &address)
{
	//hardcoding what is in the config file
	std::map <std::string,std::string> extensions {{".py", "/usr/bin/python3.10"}, {".php", "/usr/bin/php"}};


	//second argument from the config file
	std::string filePath = CGI::getActualPath(address, "./CGI-stuff/");
	std::string bin = CGI::checkExtensions(extensions, filePath);
	if (bin.empty())
		return false;
	if (!CGI::checkfile(filePath) || !CGI::checkfile(bin))
		return false; // more like a 404 not found or 403 not authorized
	
	CGI::launch(filePath, bin, CGI::getQuery(address));
	return true;
}

bool CGI::launch(const std::string &binPath, const std::string &filePath, const std::string &query)
{
	CGI handle;

	handle._query = query;
	handle._binPath = binPath;
	if (pipe(handle._toCGI) || pipe(handle._fromCGI)) {
		close(handle._toCGI[0]); close(handle._toCGI[1]);
		return false; //more like error 500
	}
	
	int pid = fork();
	if (pid == -1)
		return false; //more like error 500
	if (pid == 0)
		handle.childExec();
	//parenting_stuff();
	// set _toCGI[1] to epoll to write body

	return true;
}
/*

TO DO list:
	get a way to check if the fd from epoll are CGI and not basic request
	get a small function to just send the header "HTTP/1.1 200 OK" + few headers + the body
	and celebrate
*/

void CGI::parentTreating()
{
	close(_toCGI[0]);
	close(_fromCGI[1]);

	//hardcoded but it will be the request's body
	std::string body = "yay i am a body";
	write(_toCGI[1], body.c_str(), body.length());
    if (fcntl(_toCGI[1], F_SETFL, O_NONBLOCK) == -1){
		//sad, the CGI won't have any body
		close(_toCGI[1]);
		close(_fromCGI[0]);
		return ;
	}
	



}

void CGI::childExec()
{

	bool tmp = dup2(_toCGI[0], 0) == -1 || dup2(_fromCGI[1], 1) == -1;
	close(_toCGI[0]);
	close(_toCGI[1]);
	close(_fromCGI[0]);
	close(_fromCGI[1]);
	if (tmp) return;

	//hardcoded but gonna be Request::_headers
	std::map<std::string,std::string> envRaw =
		{{"Host","en.wikipedia.org"},
		{"User-Agent","Mozilla/5.0 (X11; Ubuntu; Linux x86_64; rv:138.0) Gecko/20100101 Firefox/138.0"},
		{"Accept","text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"}};
	
	// not to remove, according to https://datatracker.ietf.org/doc/html/rfc3875#section-4.1.7
	// the server need this flag even if empty
	envRaw.insert({"QUERY_STRING", _query});
	char **env = createEnvCGI(envRaw);
	char **argv = createArgvCGI(_binName, _fileName);

	// https://datatracker.ietf.org/doc/html/rfc3875#section-7.1
	// we need to chdir into the directory of the executable
	// TODO chdir 
	if (chdir(_fileLocation.c_str()) != -1);
		execve(_binPath.c_str(), argv, env);
	deleteEnvCGI(env);
	deleteArgvCGI(argv);
	std::exit(1);
}


/**
 * remove anything from the after any ? character found 
 * 
 * @param path the path found in the first line of the HTTP request
 * @param root (optionnal) the root from the config file
 * @return root (if present) followed by shortened path
 */
std::string CGI::getActualPath(const std::string &path, const std::string &root = ""){
	return root + (path.find('?') != std::string::npos ? std::string(path).erase(path.find('?')) : path);
}

/** 
 * check if the file at path exist and can be executed
 * 
 * @param  file the file (with path) in question
 * @return true if yes, false if not
 */
bool CGI::checkfile(const std::string &file){
	struct  stat data;

	return	stat(file.c_str(), &data) == 0		// check if exists
			&& S_ISREG(data.st_mode) != 0		// check if is a file
			&& access(file.c_str(), X_OK) != 0;	// check if executable
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

// execve("/usr/bin/php-cgi", { "php-cgi", "/index.php", nullptr}, envp);

/**
 * create the argv of the CGI 
 * 
 * @param bin should be the binary name, without /usr/bin/ before
 * @param bin should be the file name, without /CGI-stuff/ before
 * @return the argv
 */
char **CGI::createArgvCGI(const std::string &bin, const std::string &file)
{
	char **argv = new char *[2];
	argv[0] = new char[bin.length() + 1];
	std::memcpy(argv[0], bin.c_str(), bin.length() + 1);
	argv[1] = new char[file.length() + 1];
	std::memcpy(argv[1], file.c_str(), file.length() + 1);
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
