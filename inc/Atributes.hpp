//
// Created by rperrot on 5/20/25.
//
/* ************************************************************************** */

#ifndef	AATTRIBUTES
# define AATTRIBUTES

# include <iostream>
# include <vector>
# include "Request.hpp"
# include <map>
# include "utils.hpp"

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

protected:
    std::string					_root;
    std::map<int, std::string> 	_errorPages;
    std::vector<EHttpMethode> 	_httpMethode;
    Boolean						_autoIndex;
    std::vector <std::string>	_index;
    std::vector <std::string>	_cgi;
    int                         _maxBodySize;
    int getSizeLine(std::vector<std::string>::iterator it, const std::vector<std::string>::iterator &end);

public:
    Atributes();
    Atributes(const Atributes &a);
    Atributes &operator=(const Atributes &a);
    virtual ~Atributes();
    virtual bool addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    const std::string &getRoot() const;
    const std::map<int, std::string> &getErrorPages() const;
    const std::vector<EHttpMethode> &getHttpMethode() const;
    const std::vector<std::string> &getIndex() const;
    bool isAutoIndex() const;
    const std::vector<std::string> &getCgi() const;
    int getMaxBodySize() const;
};

std::ostream &operator<<(std::ostream &os, const Atributes &attributes);
#endif