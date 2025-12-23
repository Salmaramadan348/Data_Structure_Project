#include "Xml_to_Json.h"
#include "Tree.h"
#include "TreeNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <stack>

using namespace std;
string trimm(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;

    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;

    return s.substr(start, end - start);
}

void deleteXMLTree(TreeNode* node) {
    if (!node) return;
    for (TreeNode* child : node->children) {
        deleteXMLTree(child);
    }
    delete node;
}

Tree* parseXML(const string& xml) {
    Tree* tree = new Tree();
    stack<TreeNode*> st;
    TreeNode* xmlRoot = nullptr;
    size_t i = 0;

    while (i < xml.size()) {

        size_t old_i = i;
        if (xml[i] == '<') {
            if (i + 1 < xml.size() && xml[i + 1] == '/') {
                while (i < xml.size() && xml[i] != '>') i++;
                if (!st.empty()) {
                    st.pop();
                }
                if (i < xml.size()) i++;
            }
            else {
                i++;
                string tag = "";
                while (i < xml.size() && xml[i] != '>' && xml[i] != ' ' && xml[i] != '\n') {
                    tag += xml[i];
                    i++;
                }

                TreeNode* node = new TreeNode(tag);

                if (!st.empty())
                    st.top()->addChild(node);
                else
                    xmlRoot = node;

                st.push(node);

                while (i < xml.size() && xml[i] != '>') i++;
                if (i < xml.size()) i++;
            }
        }
        else {
            string txt = "";
            while (i < xml.size() && xml[i] != '<') {
                txt += xml[i];
                i++;
            }
            if (!st.empty()) {
                string cleaned = trimm(txt);
                if (cleaned != "") {
                    if (!st.top()->tagValue.empty()) {
                        st.top()->tagValue += " " + cleaned;
                    }
                    else {
                        st.top()->tagValue = cleaned;
                    }
                }
            }
        }
        if (i == old_i)
            i++;
    }

    if (xmlRoot) {
        tree->getRoot()->addChild(xmlRoot);
    }

    return tree;
}

string escapeJsonString(const string& s) {
    string result;
    for (size_t i = 0; i < s.size(); i++) {
        char c = s[i];
        if (c == '"') result += "\\\"";
        else if (c == '\\') result += "\\\\";
        else if (c == '\n') result += "\\n";
        else if (c == '\r') result += "\\r";
        else if (c == '\t') result += "\\t";
        else result += c;
    }
    return result;
}

string formatTextForJSON(const string& text, int indent, int wrap = 60) {
    string indentStr(indent, ' ');
    string extraIndent(indent + 8, ' ');

    stringstream ss(text);
    string word;
    string result = indentStr + "\"";
    size_t currentLength = 0;

    while (ss >> word) {
        if (currentLength + word.length() + 1 > static_cast<size_t>(wrap)) {
            result += "\n" + extraIndent;
            currentLength = 0;
        }
        result += word + " ";
        currentLength += word.length() + 1;
    }

    result = trimm(result);
    result += "\"";

    return result;
}


string nodeToJSON(TreeNode* node, int indent = 0) {
    string indentStr(indent, ' ');
    string result = "";

    if (node->children.empty()) {
        return formatTextForJSON(node->tagValue, indent);
    }

    bool isArrayContainer = false;
    if (!node->children.empty()) {
        string firstName = node->children[0]->name;
        isArrayContainer = true;
        for (TreeNode* child : node->children) {
            if (child->name != firstName) {
                isArrayContainer = false;
                break;
            }
        }
    }

    if (isArrayContainer) {
        string childName = node->children[0]->name;
        result += "{\n";
        result += string(indent + 2, ' ') + "\"" + childName + "\": ";

        if (node->children.size() == 1) {
            TreeNode* child = node->children[0];

            if (child->children.empty()) {
                result += formatTextForJSON(child->tagValue, indent + 2);
            }
            else {
                result += "{\n";
                bool firstField = true;

                if (!child->tagValue.empty()) {
                    result += formatTextForJSON(child->tagValue, indent + 4);
                    firstField = false;
                }

                for (TreeNode* grandchild : child->children) {
                    if (!firstField) result += ",\n";
                    firstField = false;

                    result += string(indent + 4, ' ') + "\"" + grandchild->name + "\": ";
                    result += nodeToJSON(grandchild, indent + 4);
                }

                result += "\n" + string(indent + 2, ' ') + "}";
            }
        }
        else {
            result += "[\n";

            for (size_t i = 0; i < node->children.size(); i++) {
                result += string(indent + 4, ' ');

                TreeNode* child = node->children[i];

                if (child->children.empty()) {
                    result += formatTextForJSON(child->tagValue, indent + 4);
                }
                else {
                    result += "{\n";
                    bool firstField = true;

                    if (!child->tagValue.empty()) {
                        result += formatTextForJSON(child->tagValue, indent + 6);
                        firstField = false;
                    }

                    for (TreeNode* grandchild : child->children) {
                        if (!firstField) result += ",\n";
                        firstField = false;

                        result += string(indent + 6, ' ') + "\"" + grandchild->name + "\": ";
                        result += nodeToJSON(grandchild, indent + 6);
                    }

                    result += "\n" + string(indent + 4, ' ') + "}";
                }

                if (i != node->children.size() - 1) result += ",";
                result += "\n";
            }
            result += string(indent + 2, ' ') + "]";
        }

        result += "\n" + indentStr + "}";
    }
    else {
        result += "{\n";

        bool firstField = true;

        if (!node->tagValue.empty() && !node->children.empty()) {
            result += formatTextForJSON(node->tagValue, indent + 2);
            firstField = false;
        }

        for (TreeNode* child : node->children) {
            if (!firstField) result += ",\n";
            firstField = false;

            result += string(indent + 2, ' ') + "\"" + child->name + "\": ";
            result += nodeToJSON(child, indent + 2);
        }

        result += "\n" + indentStr + "}";
    }

    return result;
}

string toJSON(Tree* tree) {
    if (!tree || !tree->getRoot()) return "{}";

    TreeNode* root = tree->getRoot();

    if (root->children.empty()) return "{}";

    TreeNode* xmlRoot = root->children[0];

    string result = "{\n";
    result += "  \"" + xmlRoot->name + "\": ";
    result += nodeToJSON(xmlRoot, 2);
    result += "\n}\n";

    return result;
}

string convertXMLtoJSON(const string& xmlText) {
    Tree* tree = parseXML(xmlText);

    if (!tree) {
        return "";
    }

    string json = toJSON(tree);

    deleteXMLTree(tree->getRoot());
    delete tree;

    return json;
}