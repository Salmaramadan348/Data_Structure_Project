#ifndef XML_PARSER_H
#define XML_PARSER_H

#include <string>
#include <vector>

class XmlParser {
public:
    static std::vector<std::string> extractTags(const std::string& xml);
    static std::string trim(const std::string& str);
};


#endif