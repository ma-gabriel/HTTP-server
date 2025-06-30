#include "Parser.hpp"

#include <fstream>
#include <unistd.h>
#include <algorithm>

Parser::~Parser()
{

}

std::map<long, std::vector<ConfigurationServer> > Parser::ParseFile(const std::string &config_file)
{
    std::ifstream file(config_file.c_str());
    if (!file.is_open())
        throw (std::runtime_error("Error: could not open " + config_file));
    std::string fileContent = this->readFile(file);
    std::vector<std::string> allTokens = getAllToken(fileContent);
    std::map<long, std::vector<ConfigurationServer> > config = createAllServeur(allTokens);
    refactorAllServer(config);
    return config;
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


std::map<long, std::vector<ConfigurationServer> > Parser::createAllServeur(std::vector<std::string> &allTokens){
    std::vector<std::string>::iterator it;
    std::vector<std::string>::iterator end = allTokens.end();
    Atributes atributes;
    std::map <long, std::vector<ConfigurationServer> > config;
    for (it = allTokens.begin(); it != end; ++it) {
        if (*it == "server"){
            ConfigurationServer server(it, allTokens.end());
            std::vector<ConfigurationServer> findConfig = config[server.getPort() + server.gethostValue() * 65536];
            findConfig.push_back(server);
            config[server.getPort() + server.gethostValue() * 65536] = findConfig;
        }
        else if (!atributes.addAttributes(it, end))
            throw std::runtime_error("Unknow Attributes " + *it);
    }
    addInfoChildren(config, atributes);
    if (config.empty())
        throw std::runtime_error("No server configuration found in the file.");
    return config;
}


void Parser::addInfoChildren(std::map<long, std::vector<ConfigurationServer> > &config, Atributes &atributes) {
    std::map<long, std::vector<ConfigurationServer> >::iterator serverIterator;
    for (serverIterator = config.begin(); serverIterator != config.end(); ++serverIterator) {
        std::vector<ConfigurationServer>::iterator serverConfigIterator = serverIterator->second.begin();
        for (; serverConfigIterator != serverIterator->second.end(); ++serverConfigIterator) {
            serverConfigIterator->fillAtributes(atributes);
            std::map<std::string, Location>::iterator locationIterator = serverConfigIterator->getLocation().begin();
            for (; locationIterator != serverConfigIterator->getLocation().end(); ++locationIterator) {
                Location &location = locationIterator->second;
                location.fillAtributes(*serverConfigIterator);
            }
        }
    }
}

Parser::Parser() {}

Parser &Parser::instance() {
    static Parser instance;
    return instance;
}

void Parser::refactorAllServer(std::map<long, std::vector<ConfigurationServer> > &config) {
    std::map<long, std::vector<ConfigurationServer> >::iterator serverIterator;

    std::vector<int> removedKeys;
    for (serverIterator = config.begin(); serverIterator != config.end(); ++serverIterator) {
        if (serverIterator->first >= 65536) {
            std::map<long, std::vector<ConfigurationServer> >::iterator  findVectorAllIp = config.find(serverIterator->first % 65536);
            if (findVectorAllIp != config.end()) {
                std::vector<ConfigurationServer>::iterator serverConfigIterator = serverIterator->second.begin();
                for (; serverConfigIterator != serverIterator->second.end(); ++serverConfigIterator) {
                    serverConfigIterator->setHost("0.0.0.0");
                    config[serverIterator->first % 65536].push_back(*serverConfigIterator);
                }
                removedKeys.push_back(serverIterator->first);
            }
        }
    }
    for (std::vector<int>::iterator it = removedKeys.begin(); it != removedKeys.end(); ++it) {
        config.erase(*it);
    }
    std::cout << config.size() << " servers found" << std::endl;
    for (std::map<long, std::vector<ConfigurationServer> >::iterator it = config.begin(); it != config.end(); ++it) {
        std::vector<std::string> serverNames;
        bool emppty = false;
        for (std::vector<ConfigurationServer>::iterator serverConfigIterator = it->second.begin(); serverConfigIterator != it->second.end(); ++serverConfigIterator) {
            std::vector<std::string> names = serverConfigIterator->getServerNames();
            if (names.empty()) {
                if (emppty)
                    throw std::runtime_error("two server with same port and ip are not server name.");
                emppty = true;
                continue;
            }
            for (std::vector<std::string>::iterator nameIt = names.begin(); nameIt != names.end(); ++nameIt) {
                if (std::find(serverNames.begin(), serverNames.end(), *nameIt) != serverNames.end()) {
                    throw std::runtime_error("Server name " + *nameIt + " already exists in another server configuration.");
                }
                serverNames.push_back(*nameIt);
            }
        }
    }
}
