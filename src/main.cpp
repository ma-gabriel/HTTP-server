#include "Socket.hpp"
#include "Epoll.hpp"

#include <iostream>
#include <string>

int main(void)
{
	Socket serv_sock(9080);
	// TODO: set serv_sock to reuse port

	Epoll epoll;
	epoll.addFd(serv_sock.getFd());

	while (1)
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
			else if (events[i].data.fd == serv_sock.getFd())
				serv_sock.handleNewClients(epoll);
			else
			{
				char buff[512];
				int readed;

				readed = read(events[i].data.fd, buff, sizeof(buff));
				buff[readed] = '\0';
				std::cout << buff << std::endl;
			}
	   }
	}
}
