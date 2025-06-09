#include "Server.hpp"
#include "Epoll.hpp"
#include <signal.h>

#include "ConfigurationServer.hpp"
#include "Parser.hpp"
#include "utils.hpp"


void sigint_handler(int signum)
{
    #ifdef LINUX
    if (signum == SIGINT)
        Epoll::isRunning = false;
    #endif
    (void)signum;
}

int main(int argc, char **argv) // no variable for -Werror=unused-parameter
{
    // remove comments + add variable names when configuration file present
    if (check_args(argc) == false)
    	return (1);
    try {
        Parser config = Parser(argv[1]);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }
    signal(SIGINT, sigint_handler);
    Epoll epoll;
    Server serv;

    epoll.addFd(serv.newInstance(8080));
    epoll.addFd(serv.newInstance(8081));
    while (Epoll::isRunning)
        epoll.routine(serv);
}