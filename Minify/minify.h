#ifndef MINIFY_XML_H
#define MINIFY_XML_H
#include <string>
#include "Tree.h"

std::string trim(const std::string& s);
std::string minifyXML(const std::string& xml);
Tree* parseXMLToTree(const std::string& xml);
std::string treeToMinifiedXML(TreeNode* node);

#endif
