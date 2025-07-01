#include "Redirection.hpp"

Redirection::Redirection(const std::string &path, int code) : _path(path), _code(code) {
    if (code < 300 || code >= 309) {
        throw std::runtime_error("Redirection code must be in the range [300, 309]");
    }
    switch (code) {
        case 300:
            _messageCodeHttp = "Multiple Choices";
            break;
        case 301:
            _messageCodeHttp = "Moved Permanently";
            break;
        case 302:
            _messageCodeHttp = "Found";
            break;
        case 303:
            _messageCodeHttp = "See Other";
            break;
        case 304:
            _messageCodeHttp = "Not Modified";
            break;
        case 305:
            _messageCodeHttp = "Use Proxy";
            break;
        case 306:
            _messageCodeHttp = "Switch Proxy";
            break;
        case 307:
            _messageCodeHttp = "Temporary Redirect";
            break;
        case 308:
            _messageCodeHttp = "Permanent Redirect";
            break;
        default:
            throw std::runtime_error("Invalid redirection code");
    }
}

Redirection::Redirection(): _path(""), _code(-1) {}

const std::string &Redirection::getPath() const {
    return _path;
}

int Redirection::getCode() const {
    return _code;
}

Redirection &Redirection::operator=(const Redirection &rhs)  {
    if (this != &rhs) {
       this->_path = rhs._path;
       this->_code = rhs._code;
       this->_messageCodeHttp = rhs._messageCodeHttp;
    }
    return *this;
}

Redirection::Redirection(const Redirection &rhs): _path(rhs._path), _code(rhs._code), _messageCodeHttp(rhs._messageCodeHttp){}

Redirection::~Redirection() {

}

const std::string &Redirection::getMessageCodeHttp() const {
    return _messageCodeHttp;
}

void Redirection::setMessageCodeHttp(const std::string &messageCodeHttp) {
    Redirection::_messageCodeHttp = messageCodeHttp;
}
