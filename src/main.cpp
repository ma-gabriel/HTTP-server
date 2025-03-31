#include "Server.hpp"

#include <iostream>

int main(void)
{
	Server serv(9000);

	char recvBuf = '\0';
	while (recvBuf != 'a')
	{
		recv(serv.getFd(), &recvBuf, sizeof(recvBuf), 0);
		if (recvBuf)
			std::cout << recvBuf << std::endl;
	}
}
