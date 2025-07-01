
#ifndef	AATTRIBUTES
# define AATTRIBUTES

# include <iostream>
# include <vector>
# include <map>
# include "utils.hpp"
# include "EHttpMethodeEnum.hpp"
# include "Redirection.hpp"

enum Boolean {
    FALSE, TRUE, Null
};

class Atributes
{
private:
    void addRoot(std::vector<std::string>::iterator &it, int n);
    void addAutoIndex(std::vector<std::string>::iterator &it, int n);
    void addIndex(std::vector<std::string>::iterator &it, int n);
    void addCgi(std::vector<std::string>::iterator &it, int n);
    void addErrorPages(std::vector<std::string>::iterator &it, int n);
    void addHttpIndex(std::vector<std::string>::iterator &it, int n);
    void addClientMaxBodySize(std::vector<std::string>::iterator &it, int n);
    void addReturn(std::vector<std::string>::iterator &it, int n);

protected:
    std::string					_root;
    std::map<int, std::string> 	_errorPages;
    std::vector<EHttpMethode> 	_httpMethode;
    Boolean						_autoIndex;
    std::vector <std::string>	_index;
    std::vector <std::string>	_cgi;
    int                         _maxBodySize;
    Redirection                 _redirection;
    int getSizeLine(std::vector<std::string>::iterator it, const std::vector<std::string>::iterator &end);

public:
    Atributes();
    Atributes(const Atributes &attributes);
    Atributes &operator=(const Atributes &atributes);
    virtual ~Atributes();
    virtual bool addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    void fillAtributes(Atributes &atributes);
    const std::string &getRoot() const;
    const std::map<int, std::string> &getErrorPages() const;
    const std::vector<EHttpMethode> &getHttpMethode() const;
    const std::vector<std::string> &getIndex() const;
    bool isAutoIndex() const;
    Boolean getAutoIndex() const;
    const Redirection &getRedirection() const;
    const std::vector<std::string> &getCgi() const;
    int getMaxBodySize() const;
};

std::ostream &operator<<(std::ostream &os, const Atributes &attributes);
#endif