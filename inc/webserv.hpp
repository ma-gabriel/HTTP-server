#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# define PROJECT_NAME "webSAH"
#if defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || defined(__bsdi__) || defined(__DragonFly__)
    #define OS_IS_BSD 1
#else
    #define OS_IS_BSD 0
#endif

# include <sstream>

template <typename T>
std::string stoi(T num)
{
    std::ostringstream ss;
    ss << num;
    return ss.str();
}

#endif