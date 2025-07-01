//
// Created by rperrot on 7/1/25.
//

#ifndef WEBSERV_REDIRECTION_HPP
#define WEBSERV_REDIRECTION_HPP

#include <iostream>

class Redirection {
    private :
        std::string _path;
        int _code;
        std::string _messageCodeHttp;
public:
    const std::string &getMessageCodeHttp() const;

    void setMessageCodeHttp(const std::string &messageCodeHttp);

public:
    virtual ~Redirection();
    Redirection();
    Redirection(const std::string &path, int code);
    const std::string &getPath() const;
    Redirection &operator=(const Redirection &rhs);
    Redirection(const Redirection &rhs);
    void setPath(const std::string &path);
    int getCode() const;
    void setCode(int code);

};


#endif //WEBSERV_REDIRECTION_H
