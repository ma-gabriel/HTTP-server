
#ifndef CGI_HPP
# define CGI_HPP
# include <string>
# include <map>
# include "Request.hpp"
# include "Location.hpp"

class CGI
{
    private:
        std::string _query;
        std::string _binPath;
        std::string _binName;
        std::string _fileLocation;
        std::string _fileName;
        std::string _body;
        std::string _pathInfo;

        Location _config;

        int _toCGI[2];
        int _fromCGI[2];
        int _socket;

        char    **createEnvCGI(std::map<std::string, std::string> headers);
        char    **createArgvCGI(const std::string &bin, const std::string &file);
        static void deleteEnvCGI(char ** env);
        static void deleteArgvCGI(char** argv);
        void childExec(std::map<std::string,std::string> &headers);
        void parentHandling(pid_t pid);
        void add_headers(std::map<std::string,std::string> &env, const Request &req);

    public:
        CGI();
        ~CGI();
        struct infos {
            time_t timestamp;
            pid_t pid;
            int output_fd;
            std::string body;
        };

        static std::string getActualPath(const std::string &path, const std::string &root);
        static std::string getQuery(const std::string &address);
        static std::string getPathInfo(std::string address, std::map<std::string,std::string> extensions);
        static bool checkfilepresence(const std::string &file);
        static bool checkfileexec(const std::string &file);
        static std::string checkExtensions(std::map<std::string,std::string> extensions, std::string &path);

        static void launch(const Request &req, const std::string &binPath, std::string filePath, Location &config);
        static ssize_t flush(int fd, std::string text);
};

bool doCGI(const Request &req);

#endif