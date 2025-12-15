

#pragma once

#include <vector>
#include <string>

// بنية بيانات لتخزين معلومات الخطأ
struct XmlError {
    std::string message;
    int lineNumber;
};

// التصريحات العامة للدوال
std::vector<std::string> extractTags(const std::string& xmlText);
std::vector<XmlError> checkXML(const std::string& xmlContent);
std::string fixXML(const std::vector<std::string>& tags);
bool isOpeningTag(const std::string& tag);
bool isClosingTag(const std::string& tag);
std::string getTagName(const std::string& tag);

