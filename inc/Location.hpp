
#ifndef LOCATION_HPP
#define LOCATION_HPP

#include <iostream>
#include "Atributes.hpp"

class Location : public Atributes {
private:
    std::string _path; // Path of the location
    void correctPath();

public:
    const std::string &getPath() const;
    void setPath(const std::string &path);
    Location(std::vector<std::string>::iterator &it, const std::vector<std::string>::iterator &end);
    Location(const Location &location);
    Location &operator=(const Location &location);
    ~Location();
    class DoubleSlashExceptionLocation : public std::runtime_error {
    public:
        DoubleSlashExceptionLocation(const std::string &path);
    };

    class StartNoWithSlashLocationException : public std::runtime_error {
    public:
        StartNoWithSlashLocationException(const std::string &path);
    };
};

#endif