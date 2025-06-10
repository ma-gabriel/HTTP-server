
#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <vector>
# include "Server.hpp"
# include "AAtributes.hpp"
# include "ConfigurationServer.hpp"

class Parser : public AAtributes
{

private:
    void    readFile(std::ifstream &file);
    void    createAllServeur(std::vector<std::string> &allTokens);
    Parser();

public:
    std::map<int, ConfigurationServer> ParseFile(std::string &fileContent);
    const std::map<int, ConfigurationServer> &getAllServeur() const;
    const std::string &getConfigFile() const;
    void setConfigFile(const std::string &configFile);
    std::vector<std::string> getAllToken(std::string &str);
    ~Parser();
};

#endif