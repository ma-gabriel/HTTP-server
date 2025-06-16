
#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <vector>
# include "Server.hpp"
# include "Atributes.hpp"
# include "ConfigurationServer.hpp"

class Parser
{

private:
    std::string readFile(std::ifstream &file);
    void addInfoChildren(std::map<int, ConfigurationServer> config, Atributes &atributes);
    std::map<int, ConfigurationServer> createAllServeur(std::vector<std::string> &allTokens);
    Parser();

public:
    std::map<int, ConfigurationServer> ParseFile(const std::string &fileContent);
    std::vector<std::string> getAllToken(std::string &str);
    static Parser &instance();
    ~Parser();
};

#endif