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

int main(int argc, char **argv) // no variable for -Werror=unused-parameter
{
	// remove comments + add variable names when configuration file present
	if (check_args(argc) == false)
		return (1);
	try {
		Epoll &epoll = Epoll::instance();
		Parser config = Parser(argv[1]);
		Server &server = Server::instance();
		signal(SIGINT, sigint_handler);
		for (std::map<int, ConfigurationServer>::const_iterator it = config.getAllServeur().begin();
			it != config.getAllServeur().end(); ++it) {
			std::cout << argv[0] << "1" << std::endl;
			epoll.addFd(server.newInstance(it->second), true);
			std::cout << argv[0] << "2"<< std::endl;
		}
		while (Epoll::isRunning)
				epoll.routine(Server::instance());
	}
	catch (const std::exception &e) {
		std::cerr << e.what() << std::endl;
		return (1);
	}

	return (0);
}
