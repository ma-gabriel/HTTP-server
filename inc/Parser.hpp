
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
    void addInfoChildren(std::map<long, std::vector<ConfigurationServer> > &config, Atributes &atributes);
    std::map<long, std::vector<ConfigurationServer> > createAllServeur(std::vector<std::string> &allTokens);
    void refactorAllServer(std::map<long, std::vector<ConfigurationServer> > &config);
        Parser();

public:
    std::map<long, std::vector<ConfigurationServer> > ParseFile(const std::string &fileContent);
    std::vector<std::string> getAllToken(std::string &str);
    static Parser &instance();
    ~Parser();
};

#endif