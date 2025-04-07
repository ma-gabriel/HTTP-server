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
			if (!(events[i].events & EPOLLIN))
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

				std::string resp = "HTTP/1.1 200 OK\r\nContent-Length: 12\r\nContent-Type: text/plain\r\n\r\nHello, World\n";
				write(events[i].data.fd, resp.c_str(), resp.length());
				epoll.delFd(events[i].data.fd);
				close(events[i].data.fd);
			}
	   }
	}
}
