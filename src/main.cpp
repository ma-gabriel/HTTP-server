#include "Server.hpp"
#include "Epoll.hpp"

#include <iostream>
#include <cstring>
#include <string>

int main(void)
{
	Server serv(8080);

	Epoll epoll;
	epoll.addFd(serv.getSocketFd());

	bool running = true;
	while (running)
	{
		int event_quant;
		epoll_event* events = epoll.getEventsPtr();

		event_quant = epoll_wait(epoll.getFd(), events, MAXEVENT, -1);
		if (event_quant == -1)
			perror("epoll_wait");
		for (int i = 0; i < event_quant; i++)
		{
			if (!(events[i].data.fd & EPOLLIN))
			{
				close (events[i].data.fd);
				continue;
			}
			else if (events[i].data.fd == serv.getSocketFd())
				serv.handleNewClients(epoll);
			else
			{
				char buff[512];
				int readed;

				readed = read(events[i].data.fd, buff, sizeof(buff));
				buff[readed] = '\0';
				std::cout << buff << std::endl;
				if (strcmp(buff, "STOP\n") == 0)
					running = false;
			}
	   }
	}
}
