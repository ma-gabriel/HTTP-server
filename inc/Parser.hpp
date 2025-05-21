
#ifndef PARSER_HPP
#define PARSER_HPP

#include <iostream>
#include <vector>
#include "AttributesParser.hpp"
#include "Server.hpp"

class Parser
{
private:
    std::string _configFile;
    std::vector<Server> _config;
    void ParseFile(std::string &fileContent);
    void readFile(std::ifstream &file);
public:
    const std::vector<Server> &getConfig() const;
    void setConfig(const std::vector<Server> &config);
    const std::string &getConfigFile() const;
    void setConfigFile(const std::string &configFile);
    std::string getNextToken(std::string &str);
    ~Parser();
    Parser(const std::string &config_file);
};

#endif