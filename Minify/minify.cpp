#include "minify.h"
#include <cctype>
#include <string>
#include <stack>

using namespace std;

// Remove leading and trailing spaces
string trim(const string& s) {
    int start = 0, end = s.size() - 1;
    
    while (start <= end && isspace(s[start])) start++;
    while (end >= start && isspace(s[end])) end--;
    
    return s.substr(start, end - start + 1);
}

// Parse XML string to Tree structure
Tree* parseXMLToTree(const string& xml) {
    Tree* tree = new Tree();
    TreeNode* current = tree->getRoot();
    stack<TreeNode*> nodeStack;
    nodeStack.push(current);
    
    string buffer;
    bool insideTag = false;
    bool isClosingTag = false;
    string textContent;
    
    for (size_t i = 0; i < xml.size(); i++) {
        char c = xml[i];
        
        if (c == '<') {
            // Save text content before tag
            if (!textContent.empty()) {
                string trimmed = trim(textContent);
                if (!trimmed.empty() && current) {
                    current->tagValue = trimmed;
                }
                textContent.clear();
            }
            
            insideTag = true;
            buffer.clear();
            
            // Check if closing tag
            if (i + 1 < xml.size() && xml[i + 1] == '/') {
                isClosingTag = true;
                i++; // Skip '/'
            } else {
                isClosingTag = false;
            }
        }
        else if (c == '>') {
            insideTag = false;
            
            if (isClosingTag) {
                // Pop from stack
                if (!nodeStack.empty()) {
                    nodeStack.pop();
                    if (!nodeStack.empty()) {
                        current = nodeStack.top();
                    }
                }
            } else if (!buffer.empty()) {
                // Create new node
                TreeNode* newNode = new TreeNode(buffer);
                if (current) {
                    current->addChild(newNode);
                }
                current = newNode;
                nodeStack.push(current);
            }
            
            buffer.clear();
        }
        else if (insideTag) {
            if (!isspace(c)) {
                buffer += c;
            }
        }
        else {
            // Collect text outside tags
            textContent += c;
        }
    }
    
    return tree;
}

// Convert Tree back to minified XML string
string treeToMinifiedXML(TreeNode* node) {
    if (!node || node->name == "root") {
        // Root node - process children only
        string result;
        if (node) {
            for (auto child : node->children) {
                result += treeToMinifiedXML(child);
            }
        }
        return result;
    }
    
    string result = "<" + node->name + ">";
    
    // Add text content if exists
    if (!node->tagValue.empty()) {
        result += node->tagValue;
    }
    
    // Add children
    for (auto child : node->children) {
        result += treeToMinifiedXML(child);
    }
    
    result += "</" + node->name + ">";
    return result;
}

// Minify XML using Tree structure
string minifyXML(const string& xml) {
    Tree* tree = parseXMLToTree(xml);
    string result = treeToMinifiedXML(tree->getRoot());
    delete tree;
    return result;
}
