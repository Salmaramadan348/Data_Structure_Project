#include "XmlValidator.h"
#include "XmlParser.h"
#include <stack>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

bool XmlValidator::isOpeningTag(const std::string& tag) {
    return tag.size() > 2 && tag[0] == '<' && tag[1] != '/' && tag.back() == '>';
}

bool XmlValidator::isClosingTag(const std::string& tag) {
    return tag.size() > 3 && tag[0] == '<' && tag[1] == '/' && tag.back() == '>';
}

std::string XmlValidator::getTagName(const std::string& tag) {
    if (isOpeningTag(tag)) return tag.substr(1, tag.size() - 2);
    if (isClosingTag(tag)) return tag.substr(2, tag.size() - 3);
    return "";
}




using namespace std;

std::vector<XmlError> XmlValidator::checkXml(const std::string& xmlContent) {
    XmlParser parser;
    std::vector<std::string> tokens = parser.extractTags(xmlContent);

    std::stack<std::pair<std::string, int>> st;
    std::vector<XmlError> errorList;
    size_t currentSearchPos = 0;

    for (const std::string& token : tokens) {
        // --- PRECISE LINE TRACKING ---
        size_t foundPos = xmlContent.find(token, currentSearchPos);
        int lineNumber = 1;
        if (foundPos != std::string::npos) {
            for (size_t j = 0; j < foundPos; j++) {
                if (xmlContent[j] == '\n') lineNumber++;
            }
            currentSearchPos = foundPos + token.length();
        }

        // --- VALIDATION LOGIC ---
        if (isOpeningTag(token)) {
            st.push({ getTagName(token), lineNumber });
        }
        else if (isClosingTag(token)) {
            std::string closingName = getTagName(token);

            if (st.empty()) {
                errorList.push_back({ lineNumber, "Extra closing tag: " + token + " at line " + std::to_string(lineNumber) });
                continue;
            }

            std::string openName = st.top().first;
            int openLine = st.top().second;

            if (openName == closingName) {
                st.pop();
            }
            else {
                bool foundInStack = false;
                std::stack<std::pair<std::string, int>> tempStack = st;
                std::vector<std::pair<std::string, int>> unclosedTags;

                while (!tempStack.empty()) {
                    if (tempStack.top().first == closingName) {
                        foundInStack = true;
                        break;
                    }
                    unclosedTags.push_back(tempStack.top());
                    tempStack.pop();
                }

                if (foundInStack) {
                    // MATCHING THE FORMAT IN YOUR 3rd IMAGE
                    std::string msg = "Missing closing tags for (out of order):";
                    for (const auto& tag : unclosedTags) {
                        msg += "\n    - </" + tag.first + "> (opened at line " + std::to_string(tag.second) + ")";
                    }

                    // We report this at the line where we discovered the out-of-order tag
                    errorList.push_back({ lineNumber, msg });

                    st = tempStack;
                    st.pop(); // Pop the matched tag
                }
                else {
                    // MATCHING THE FORMAT IN YOUR 3rd IMAGE
                    errorList.push_back({ lineNumber, "Tag mismatch: </" + closingName + "> doesn't match <" + openName + "> at line " + std::to_string(openLine) });
                    st.pop();
                }
            }
        }
    }

    // --- FINAL CHECK (END OF FILE) ---
    while (!st.empty()) {
        std::string tag = st.top().first;
        int line = st.top().second;
        st.pop();

        errorList.push_back({ line, "Missing closing tag: </" + tag + "> for <" + tag + "> at line " + std::to_string(line) });
    }

    return errorList;
}
// Tree-based fixer (unchanged)
std::string XmlValidator::fixXMLUsingTree(const std::vector<std::string>& tokens) {
    Tree tree;
    TreeNode* current = tree.getRoot();

    for (const std::string& token : tokens) {
        if (isOpeningTag(token)) {
            std::string name = getTagName(token);
            if (!current->tagValue.empty() && current->name != "root") current = current->parent;
            TreeNode* newNode = new TreeNode(name);
            current->addChild(newNode);
            current = newNode;
        }
        else if (isClosingTag(token)) {
            std::string closingName = getTagName(token);
            TreeNode* temp = current;
            bool found = false;
            while (temp != nullptr && temp->name != "root") {
                if (temp->name == closingName) { found = true; break; }
                temp = temp->parent;
            }
            if (found) current = temp->parent;
        }
        else {
            std::string cleaned = XmlParser::trim(token);
            if (!cleaned.empty()) current->tagValue = cleaned;
        }
    }
    return traverse(tree.getRoot());
}

std::string XmlValidator::traverse(TreeNode* node, int indent) {
    if (!node) return "";
    if (node->name == "root") {
        string res = "";
        for (auto child : node->children) res += traverse(child, indent);
        return res;
    }
    string result;
    string tab(indent, ' ');
    if (!node->children.empty()) {
        result += tab + "<" + node->name + ">\n";
        if (!node->tagValue.empty()) result += tab + "  " + node->tagValue + "\n";
        for (auto child : node->children) result += traverse(child, indent + 2);
        result += tab + "</" + node->name + ">\n";
    }
    else {
        result += tab + "<" + node->name + ">" + node->tagValue + "</" + node->name + ">\n";
    }
    return result;
}
