#ifndef XML_VALIDATOR_H
#define XML_VALIDATOR_H

#include <string>
#include <vector>
#include "Tree.h"

// Structure to hold error details for the GUI
struct XmlError {
    int lineNumber;
    std::string message;
};

class XmlValidator {
public:
    
    static bool isOpeningTag(const std::string& tag);
    static bool isClosingTag(const std::string& tag);
    static std::string getTagName(const std::string& tag);

    
    std::string fixXMLUsingTree(const std::vector<std::string>& tokens);

    std::vector<XmlError> checkXml(const std::string& xmlContent);

private:
    
    std::string traverse(TreeNode* node, int indent = 0);
};

#endif
