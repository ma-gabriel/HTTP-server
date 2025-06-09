#include "Server.hpp"
#include "Epoll.hpp"
#include <signal.h>

#include "ConfigurationServer.hpp"
#include "Parser.hpp"
#include "utils.hpp"


void sigint_handler(int signum)
{
    Epoll::isRunning = false;
    (void)signum;
}

int main(int argc, char **argv) // no variable for -Werror=unused-parameter
{
    // remove comments + add variable names when configuration file present
    if (check_args(argc) == false)
    	return (1);
    try {
        Parser config = Parser(argv[1]);
        signal(SIGINT, sigint_handler);
        Epoll epoll;
        Server serv;
        for (std::map<int, ConfigurationServer>::const_iterator it = config.getAllServeur().begin();
             it != config.getAllServeur().end(); ++it) {
            epoll.addFd(serv.newInstance(it->second));
        }
        while (Epoll::isRunning)
            epoll.routine(serv);
    }
    catch (const std::exception &e) {
        std::cerr << e.what() << std::endl;
        return (1);
    }

    return (0);
}