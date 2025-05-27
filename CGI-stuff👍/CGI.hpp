
#include <string>
#include <map>

class CGI
{
    private:
        std::string _query;
        std::string _binPath;
        std::string _binName;
        std::string _fileLocation;
        std::string _fileName;

        int _toCGI[2];
        int _fromCGI[2];

        char    **createEnvCGI(std::map<std::string, std::string> headers);
        char    **createArgvCGI(const std::string &bin, const std::string &file);
        static void deleteEnvCGI(char ** env);
        static void deleteArgvCGI(char** argv);
        void childExec();
        void parentTreating();

    public:
        CGI();
        ~CGI();


        static std::string getActualPath(const std::string &path, const std::string &root = "");
        static bool launch(const std::string &binPath, const std::string &filePath, const std::string &query);
        static std::string getQuery(const std::string &address);
        static bool checkfile(const std::string &path);
        static std::string checkExtensions(std::map<std::string,std::string> extensions, const std::string &path);

};