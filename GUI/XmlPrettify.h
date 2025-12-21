#ifndef XML_PRETTIFY_H
#define XML_PRETTIFY_H

#include <string>
#include <vector>
#include "TreeNode.h"

class XMLTree {
public:
    std::string getFormattedXML(TreeNode* root, size_t maxLength) const;

private:
    void formatRecursive(TreeNode* node, int level, size_t maxLength, std::string& result) const;
    std::string getIndentation(int level) const;
    std::string trim(const std::string& str) const;
};

#endif