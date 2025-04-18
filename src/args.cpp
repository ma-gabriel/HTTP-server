
#include <iostream>

bool check_args(int argc)
{
    if (argc != 2){
        std::cerr << "Program must be used as ./webserv [configuration file]" << std::endl;
        return false;
    }
    //that's basically it
    return true;
}