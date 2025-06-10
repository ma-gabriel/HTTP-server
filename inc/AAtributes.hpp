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

class AAtributes
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
    virtual bool addAttributes(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    int getSizeLine(std::vector<std::string>::iterator it, const std::vector<std::string>::iterator &end);

public:
    AAtributes();
    AAtributes(const AAtributes &a);
    virtual ~AAtributes();
    const std::string &getRoot() const;
    const std::map<int, std::string> &getErrorPages() const;
    const std::vector<EHttpMethode> &getHttpMethode() const;
    const std::vector<std::string> &getIndex() const;
    bool isAutoIndex() const;
    void setRoot(const std::string &root);
    void setHttpMethode(const std::vector<EHttpMethode> &httpMethode);
    void setErrorPages(const std::map<int, std::string> &errorPages);
    void setIndex(const std::vector<std::string> &index);
    void setAutoIndex(Boolean autoIndex);
    const std::vector<std::string> &getCgi() const;
    int getMaxBodySize() const;
};

std::ostream &operator<<(std::ostream &os, const AAtributes &attributes);
#endif