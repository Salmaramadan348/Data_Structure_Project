#ifndef XMLVALIDATOR_H
#define XMLVALIDATOR_H

#include <string>
#include <vector>

std::vector<std::string> extractTags(const std::string& xmlText);
std::string fixXML(const std::vector<std::string>& tags);
bool isOpeningTag(const std::string& tag);
bool isClosingTag(const std::string& tag);
std::string getTagName(const std::string& tag);
bool checkXML(const std::vector<std::string>& tags);

#endif
