//
// Created by rperrot on 3/25/25.
//

#include "AttributesParser.hpp"

AttributesParser::AttributesParser()
{
}

AttributesParser::AttributesParser(const std::string &key, const std::string &value, const std::vector<AttributesParser> &values)
        : key(key), value(value), children(values)
{

}

const std::string &AttributesParser::getValue() const
{
    return value;
}

void AttributesParser::setValue(const std::string &value)
{
    AttributesParser::value = value;
}

const std::vector<AttributesParser> &AttributesParser::getChildren() const
{
    return children;
}

void AttributesParser::setChildren(const std::vector<AttributesParser> &values)
{
    AttributesParser::children = values;
}

const std::string &AttributesParser::getKey() const
{
    return key;
}

void AttributesParser::setKey(const std::string &key)
{
    AttributesParser::key = key;
}

void AttributesParser::addChildren(const AttributesParser &value)
{
    this->children.push_back(value);
}

AttributesParser *AttributesParser::getParent() const
{
    return parent;
}

void AttributesParser::setParent(AttributesParser *parent)
{
    AttributesParser::parent = parent;
}
