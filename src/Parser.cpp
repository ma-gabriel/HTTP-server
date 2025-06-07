/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Parser.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ibjean-b <ibjean-b@student.42.fr>          #+#  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025-03-19 16:59:30 by ibjean-b          #+#    #+#             */
/*   Updated: 2025-03-19 16:59:30 by ibjean-b         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Parser.hpp"

#include <fstream>

Parser::~Parser()
{

}

Parser::Parser(const std::string &config_file): _configFile(config_file)
{
    std::ifstream file(config_file.c_str());
    try
    {;
        if (!file.is_open())
            throw (std::runtime_error("Error: could not open " + config_file));
        this->readFile(file);
        file.close();
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
}

void Parser::readFile(std::ifstream &file)
{
    std::string line;
    std::string fileContent;
    while (std::getline(file, line)) {
        size_t comment = line.find('#');
        if (comment != std::string::npos)
            line = line.substr(0, comment);
        fileContent += line + "\n";
    }
    std::vector<std::string> allTokens = getAllToken(fileContent);
    try {
        createAllServeur(allTokens);
    }
    catch (const std::runtime_error &e) {
        std::cerr << "Error while parsing the configuration file: " << e.what() << std::endl;
        return;
    }
}

std::vector<std::string> Parser::getAllToken(std::string &str)
{
    std::vector<std::string> tokenList;
    size_t i = 0;

    while (i < str.length()) {
        while (i < str.length() && isSeparator(str[i]))
            i++;
        int start = i;
        if (i < str.length()) {
            while (i < str.length() && !isSeparator(str[i])) {
                i++;
                if (i == str.length() || str[i] == '{' || str[i] == '}' || str[i] == ';')
                    break;
            }
            tokenList.push_back(str.substr(start, i - start));
        }
    }
    return tokenList;
}

void Parser::createAllServeur(std::vector<std::string> &allTokens){
    std::vector<std::string>::iterator it;

    for (it = allTokens.begin(); it != allTokens.end(); ++it) {
        if (*it == "server"){
            ConfigurationServer server = ConfigurationServer(it, allTokens.end());
        }
    }

}

void Parser::ParseFile(std::string &fileContent)
{
    (void)fileContent;
}

const std::vector<Server> &Parser::getConfig() const
{
    return _config;
}

void Parser::setConfig(const std::vector<Server> &config)
{
    _config = config;
}

const std::string &Parser::getConfigFile() const
{
    return _configFile;
}

void Parser::setConfigFile(const std::string &configFile)
{
    _configFile = configFile;
}