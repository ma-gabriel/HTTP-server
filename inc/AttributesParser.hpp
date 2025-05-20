//
// Created by rperrot on 3/25/25.
//

#ifndef WEBSERV_ATTRIBUTESPARSER_HPP
#define WEBSERV_ATTRIBUTESPARSER_HPP
# include <iostream>
# include <vector>

class AttributesParser
{
public:
    AttributesParser(const std::string &key, const std::string &value, const std::vector<AttributesParser> &values);
    AttributesParser();
    const std::string &getValue() const;
    void setValue(const std::string &value);
    const std::vector<AttributesParser> &getChildren() const;
    void setChildren(const std::vector<AttributesParser> &values);
    const std::string &getKey() const;
    void setKey(const std::string &key);
    void addChildren(const AttributesParser &value);
    AttributesParser *getParent() const;
    void setParent(AttributesParser *parent);

private:
    std::string key;
    std::string value;
    std::vector<AttributesParser> children;
    AttributesParser *parent;
};

#endif
