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
#include <unistd.h>

Parser::~Parser()
{

}

Parser::Parser(const std::string &config_file): _configFile(config_file)
{
    std::ifstream file(config_file.c_str());
    if (!file.is_open())
        throw (std::runtime_error("Error: could not open " + config_file));
    this->readFile(file);
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
    file.close();
    std::vector<std::string> allTokens = getAllToken(fileContent);
    createAllServeur(allTokens);
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
            ConfigurationServer server(it, allTokens.end());
            this->_allServeur[server.getPort()] = server;
        }
    }
}

void Parser::ParseFile(std::string &fileContent)
{
    (void)fileContent;
}

const std::string &Parser::getConfigFile() const
{
    return _configFile;
}

void Parser::setConfigFile(const std::string &configFile)
{
    _configFile = configFile;
}

const std::map<int, ConfigurationServer> &Parser::getAllServeur() const {
    return _allServeur;
}
