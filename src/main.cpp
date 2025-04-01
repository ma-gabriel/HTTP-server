#include "Server.hpp"

#include <iostream>
#include <string>

int main(void)
{
	Server serv(9000);

	std::string recvBuf;
	while (recvBuf.find('\n') != std::string::npos)
		recv(serv.getFd(), &recvBuf, sizeof(recvBuf), 0);

	std::cout << recvBuf << std::endl;
}
