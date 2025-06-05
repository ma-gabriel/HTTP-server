#include "Request.hpp"
#include <sstream>


// Getters
int Request::getSock(void) const
{
    return(this->_sock);
}

std::string Request::getVersion(void) const
{
    return(this->_version);
}

std::string Request::getPath(void) const
{
    return(this->_path);
}

// Setters

// Public member functions
void Request::parseRequest()
{
    this->parseFirstLine();
    this->checkFirstLine();

    this->extractHeaders();
    this->_body = this->_raw;
}

void Request::parseFirstLine()
{
    std::string first_line = extractOneLine();

    std::string buff;
    std::istringstream stream(first_line);

    getline(stream, this->_method, ' ');
    getline(stream, this->_path, ' ');
    getline(stream, this->_version, '\r');
}

void Request::checkFirstLine()
{
    if (this->checkMethod() == false)
        throw Request::BadRequestException("Bad HTTP method.");
    if (this->_path.empty())
        throw Request::BadRequestException("No path have been provided.");
    if (this->_version.empty() || this->_version != "HTTP/1.1")
        throw Request::BadRequestException("Bad HTTP version");
}

bool Request::checkMethod()
{
    if (this->_method.empty())
        return (false);
    if (this->_method == "GET")
        return (true);
    if (this->_method == "POST")
        return (true);
    if (this->_method == "DELETE")
        return (true);
    return (false);
}

void Request::extractHeaders()
{
    std::string curr_line = extractOneLine();
    while (curr_line.empty() == false)
    {
        std::string key = extractHeaderKey(curr_line);
        this->_headers[key] = curr_line;
        curr_line = extractOneLine();
    }
}

std::string Request::extractHeaderKey(std::string& line)
{
    std::string::size_type colon_pos = line.find(":");
    if (colon_pos == std::string::npos)
        throw Request::BadRequestException("Bad header formating");

    std::string key = line.substr(0, colon_pos);
    line.erase(0, colon_pos + 2);

    return (key);
}

std::string Request::extractOneLine()
{
    std::string::size_type line_end = this->_raw.find("\r\n");
    if (line_end == std::string::npos)
        throw Request::BadRequestException("End-of-line error");

    std::string line = this->_raw.substr(0, line_end);
    this->_raw.erase(0, line_end + 2);

    return (line);
}
