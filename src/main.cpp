#include "Server.hpp"
#include "Epoll.hpp"

#include <cstring>
#include <signal.h>

void sigint_handler(int signum)
{
    if (signum == SIGINT)
        Epoll::isRunning = false;
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