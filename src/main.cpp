#include "Server.hpp"
#include "Epoll.hpp"

#include <cstring>
#include <signal.h>

bool running = true;

void sigint_handler(int signum)
{
	if (signum == SIGINT)
		running = false;

}

int main(void)
{
	signal(SIGINT, sigint_handler);
	Epoll epoll;
	Server serv;

	epoll.addFd(serv.newInstance(8080));
	epoll.addFd(serv.newInstance(8081));

	while (running)
		epoll.routine(serv);
}
