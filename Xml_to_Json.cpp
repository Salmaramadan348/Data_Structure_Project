#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <stack>

using namespace std;

string trim(const string& s) {
    size_t start = 0;
    while (start < s.size() && isspace((unsigned char)s[start])) start++;

    size_t end = s.size();
    while (end > start && isspace((unsigned char)s[end - 1])) end--;

    return s.substr(start, end - start);
}

struct XMLNode {
    string name;
    string text;
    vector<XMLNode*> children;

    XMLNode(string n) : name(n), text("") {}

    XMLNode* findChild(const string& childName) {
        for (XMLNode* child : children) {
            if (child->name == childName) {
                return child;
            }
        }
        return nullptr;
    }

    vector<XMLNode*> findAllChildren(const string& childName) {
        vector<XMLNode*> result;
        for (XMLNode* child : children) {
            if (child->name == childName) {
                result.push_back(child);
            }
        }
        return result;
    }
};

void deleteXMLTree(XMLNode* node) {
    if (!node) return;
    for (XMLNode* child : node->children) {
        deleteXMLTree(child);
    }
    delete node;
}

XMLNode* parseXML(const string& xml) {
    stack<XMLNode*> st;
    XMLNode* root = nullptr;
    size_t i = 0;

    while (i < xml.size()) {
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

                XMLNode* node = new XMLNode(tag);

                if (!st.empty())
                    st.top()->children.push_back(node);
                else
                    root = node;

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
                string cleaned = trim(txt);
                if (cleaned != "") {
                    if (!st.top()->text.empty()) {
                        st.top()->text += " " + cleaned;
                    }
                    else {
                        st.top()->text = cleaned;
                    }
                }
            }
        }
    }

    return root;
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

    result = trim(result);
    result += "\"";

    return result;
}


string nodeToJSON(XMLNode* node, int indent = 0) {
    string indentStr(indent, ' ');
    string result = "";

    if (node->children.empty()) {
        return formatTextForJSON(node->text, indent);
    }

    bool isArrayContainer = false;
    if (!node->children.empty()) {
        string firstName = node->children[0]->name;
        isArrayContainer = true;
        for (XMLNode* child : node->children) {
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
            XMLNode* child = node->children[0];

            if (child->children.empty()) {
                result += formatTextForJSON(child->text, indent + 2);
            }
            else {
                result += "{\n";
                bool firstField = true;

                if (!child->text.empty()) {
                    result += formatTextForJSON(child->text, indent + 4);
                    firstField = false;
                }

                for (XMLNode* grandchild : child->children) {
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

                XMLNode* child = node->children[i];

                if (child->children.empty()) {
                    result += formatTextForJSON(child->text, indent + 4);
                }
                else {
                    result += "{\n";
                    bool firstField = true;

                    if (!child->text.empty()) {
                        result += formatTextForJSON(child->text, indent + 6);
                        firstField = false;
                    }

                    for (XMLNode* grandchild : child->children) {
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

        if (!node->text.empty() && !node->children.empty()) {
            result += formatTextForJSON(node->text, indent + 2);
            firstField = false;
        }

        for (XMLNode* child : node->children) {
            if (!firstField) result += ",\n";
            firstField = false;

            result += string(indent + 2, ' ') + "\"" + child->name + "\": ";
            result += nodeToJSON(child, indent + 2);
        }

        result += "\n" + indentStr + "}";
    }

    return result;
}

string toJSON(XMLNode* root) {
    if (!root) return "{}";

    string result = "{\n";
    result += "  \"" + root->name + "\": ";
    result += nodeToJSON(root, 2);
    result += "\n}\n";

    return result;
}
string convertXMLtoJSON(const string& xmlText) {
    XMLNode* root = parseXML(xmlText);

    if (!root) {
        return "";
    }

    string json = toJSON(root);
    deleteXMLTree(root);
    return json;
}

