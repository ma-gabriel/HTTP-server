
#ifndef PARSER_HPP
# define PARSER_HPP

# include <iostream>
# include <vector>
# include "Server.hpp"
# include "AAtributes.hpp"
# include "utils.hpp"
# include "ConfigurationServer.hpp"

class Parser : private AAtributes
{
private:
    std::string _configFile;
    std::map<int, ConfigurationServer> _allServeur;

    void ParseFile(std::string &fileContent);
    void readFile(std::ifstream &file);
    void    createAllServeur(std::vector<std::string> &allTokens);

public:
    const std::string &getConfigFile() const;
    void setConfigFile(const std::string &configFile);
    std::vector<std::string> getAllToken(std::string &str);
    ~Parser();
    Parser(const std::string &config_file);
};

#endif