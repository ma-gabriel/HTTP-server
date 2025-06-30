#include "Server.hpp"
#include <Epoll.hpp>
#include "Parser.hpp"

#include <cstring>
#include <signal.h>

void sigint_handler(int signum)
{
	if (signum == SIGINT)
	{
		Epoll::isRunning = false;
		std::cerr << std::endl;
	}
}

int main(int argc, char **argv)
{
	if (!check_args(argc, argv))
		return (1);
	try {
		Epoll &epoll = Epoll::instance();
		Parser &parser = Parser::instance();
		Server &server = Server::instance();
		std::map<int, ConfigurationServer> allServers = parser.ParseFile(argv[1]);
		signal(SIGINT, sigint_handler);
		for (std::map<int, ConfigurationServer>::const_iterator it = allServers.begin();
		     it != allServers.end(); ++it) {
			std::cout << it->second.getPort() << std::endl;
			epoll.addFd(server.newInstance(it->second));
		}
		while (Epoll::isRunning)
				epoll.routine();
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}
	return (0);
}
