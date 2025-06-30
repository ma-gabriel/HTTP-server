
#include <iostream>

bool check_args(int argc, char **argv) {
    if (argc != 2) {
        std::cerr << "Program must be used as ./webserv [configuration file]" << std::endl;
        return false;
    }
    if (std::string(argv[1]).size() < 5 || std::string(argv[1]).substr(std::string(argv[1]).size() - 5) != ".conf") {
        std::cerr << "Configuration file must end with .conf" << std::endl;
        return false;
    }
    //that's basically it
    return true;
}