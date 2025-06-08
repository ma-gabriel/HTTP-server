#include "Server.hpp"
#include "Epoll.hpp"
#include "webserv.hpp"
#include <cstring>
#include <iostream>
#include <signal.h>

void sigint_handler(int signum)
{
    #ifdef LINUX
    if (signum == SIGINT)
        Epoll::isRunning = false;
    #endif
    (void)signum;
}

int main(int , char **) // no variable for -Werror=unused-parameter
{
    // remove comments + add variable names when configuration file present
    // if (check_args(argc) == false)
    // 	return (1);

    signal(SIGINT, sigint_handler);
    Epoll epoll;
    Server serv;

    epoll.addFd(serv.newInstance(8080));
    epoll.addFd(serv.newInstance(8081));
    while (Epoll::isRunning)
        epoll.routine(serv);
}