#include "Location.hpp"

Location::DoubleSlashExceptionLocation::DoubleSlashExceptionLocation(const std::string &path): std::runtime_error("Path of this location " + path + " must not contain //")
{

}

Location::StartNoWithSlashLocationException::StartNoWithSlashLocationException(const std::string &path):
    std::runtime_error("path of this location " + path + " must start with /"){}