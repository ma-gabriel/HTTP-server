#include "Response.hpp"

#include <sstream>

 std::string Response::createResponseRedirect(Request &request){
    std::string  path = request.getPath();
    std::string  locationPath = request.getConfig().getPath();
    if (request.getHeaders().find("HOST") == request.getHeaders().end()) {
        return Response::error(400, "Bad Request", request.getConfig().getErrorPages());
    }
    std::string newRedirection = "http://" + request.getHeaders().at("HOST") + request.getConfig().getRedirection().getPath();
     std::ostringstream oss;
     oss << std::dec
         << "HTTP/1.1 " << request.getConfig().getRedirection().getCode() << " "
         << request.getConfig().getRedirection().getMessageCodeHttp() << "\r\n"
         << "Location: " << newRedirection << "\r\n"
         << "Content-Type: text/html\r\n"
         << "Content-Length: 0\r\n"
         << "\r\n";
     return oss.str();
}