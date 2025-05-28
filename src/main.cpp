#include "Server.hpp"
#include "Epoll.hpp"

#include <iostream>
#include <cstring>
#include <signal.h>
#include <sys/wait.h>

void sigint_handler(int signum)
{
	if (signum == SIGINT)
	{
		Epoll::isRunning = false;
		std::cerr << std::endl;
	}
}
void wait_handler(int){ wait(NULL);}

int main(int , char **) // no variable for -Werror=unused-parameter
{
	// remove comments + add variable names when configuration file present
	// if (check_args(argc) == false)
	// 	return (1);

	signal(SIGCHLD, wait_handler);
	signal(SIGINT, sigint_handler);
	Epoll &epoll = Epoll::instance();
	Server &serv = Server::instance();

	epoll.addFd(serv.newInstance(8080));
	epoll.addFd(serv.newInstance(8081));

	while (Epoll::isRunning)
		epoll.routine(serv);
}
