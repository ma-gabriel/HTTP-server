cmake_minimum_required(VERSION 3.30)
project(webserv)

set(CMAKE_CXX_STANDARD 14)

include_directories(inc)
include_directories(src)

add_executable(webserv
    inc/AAtributes.hpp
    inc/ARequest.hpp
    inc/ConfigurationServer.hpp
        src/ConfigurationServer.cpp
    inc/Epoll.hpp
        inc/Location.hpp
    inc/CGI.hpp
        src/CGI.cpp
    inc/Parser.hpp
    inc/Request.hpp
    inc/Response.hpp
    inc/Server.hpp
    inc/utils.hpp
    inc/webserv.hpp
    src/exceptions/LocationException.cpp
    src/utils/isSeparator.cpp
    src/utils/strIsdigit.cpp
    src/utils/strNoCase.cpp
    src/AAtributes.cpp
    src/ARequest.cpp
    src/args.cpp
        src/Epoll.cpp
        src/Location.cpp
    src/main.cpp
    src/Parser.cpp
    src/Request.cpp
    src/Response.cpp
    src/Server.cpp)
