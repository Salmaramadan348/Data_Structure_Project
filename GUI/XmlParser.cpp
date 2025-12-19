#include "XmlParser.h"

std::vector<std::string> XmlParser::extractTags(const std::string& xml) {
    std::vector<std::string> tokens;
    std::string cur;
    for (size_t i = 0; i < xml.size(); i++) {
        if (xml[i] == '<') {
            if (!cur.empty()) {
                tokens.push_back(cur);
                cur.clear();
            }
            size_t j = i;
            while (j < xml.size() && xml[j] != '>') j++;
            tokens.push_back(xml.substr(i, j - i + 1));
            i = j;
        }
        else {
            cur += xml[i];
        }
    }
    if (!cur.empty()) tokens.push_back(cur);
    return tokens;
}

std::string XmlParser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}