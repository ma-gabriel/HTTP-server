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

std::map<int, ConfigurationServer> Parser::ParseFile(const std::string &config_file)
{
    std::ifstream file(config_file.c_str());
    if (!file.is_open())
        throw (std::runtime_error("Error: could not open " + config_file));
    std::string fileContent = this->readFile(file);
    std::vector<std::string> allTokens = getAllToken(fileContent);
    return createAllServeur(allTokens);
}

std::string Parser::readFile(std::ifstream &file)
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
    return fileContent;

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


std::map<int, ConfigurationServer> Parser::createAllServeur(std::vector<std::string> &allTokens){
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator end = allTokens.end();
    Atributes atributes;
    std::map <int, ConfigurationServer> config;
    for (it = allTokens.begin(); it != end; ++it) {
        if (*it == "server"){
            ConfigurationServer server(it, allTokens.end());
            if (config.find(server.getPort()) != config.end()) {
                throw std::runtime_error("Server with this port : " + server.getPortString() + " already exists.");
            }
            config[server.getPort()] = server;
        }
        else if (!atributes.addAttributes(it, end))
            throw std::runtime_error("Unknow Attributes " + *it);
    }
    addInfoChildren(config, atributes);
    if (config.empty())
        throw std::runtime_error("No server configuration found in the file.");
    return config;
}


void Parser::addInfoChildren(std::map<int, ConfigurationServer> &config, Atributes &atributes) {
    std::map<int, ConfigurationServer>::iterator serverIterator;
    for (serverIterator = config.begin(); serverIterator != config.end(); ++serverIterator) {
        serverIterator->second.fillAtributes(atributes);
        std::map<std::string, Location>::iterator locationIterator = serverIterator->second.getLocation().begin();
        for (; locationIterator != serverIterator->second.getLocation().end(); ++locationIterator) {
            Location &location = locationIterator->second;
            location.fillAtributes(serverIterator->second);

        }
    }

}

Parser::Parser() {}

Parser &Parser::instance() {
    static Parser instance;
    return instance;
}
