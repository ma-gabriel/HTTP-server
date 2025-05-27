#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define PROJECT_NAME "webSAH"

# include <sstream>

template <typename T>
std::string stoi(T num)
{
	std::ostringstream ss;
	ss << num;
	return ss.str();
}

#endif
