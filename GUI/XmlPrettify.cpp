#include "XmlPrettify.h"

std::string XMLTree::getIndentation(int level) const {
    return std::string(level * 4, ' ');
}

std::string XMLTree::trim(const std::string& str) const {
    size_t first = str.find_first_not_of(" \t\n\r");
    if (first == std::string::npos) return "";
    size_t last = str.find_last_not_of(" \t\n\r");
    return str.substr(first, (last - first + 1));
}

void XMLTree::formatRecursive(TreeNode* node, int level, size_t maxLength, std::string& result) const {
    // FIX: Only skip if node is null or name is empty. 
    // Do NOT check for "root" here because that might be your actual XML root tag.
    if (!node || node->name.empty()) return;

    if (!result.empty()) result += "\r\n";
    std::string indent = getIndentation(level);

    // --- CDATA and Comments ---
    if (node->name.find("!--") == 0 || node->name.find("![CDATA[") == 0) {
        result += indent + "<" + node->name + ">";
        return;
    }

    // --- Attribute Splitting ---
    std::string pureName = node->name;
    size_t spacePos = node->name.find(' ');
    if (spacePos != std::string::npos) {
        pureName = node->name.substr(0, spacePos);
    }

    std::string openingTag = "<" + node->name + ">";
    std::string cleanValue = trim(node->tagValue);
    bool hasChildren = !node->children.empty();
    bool hasValue = !cleanValue.empty();

    // Logic: Short text + no children = one line
    bool isSimple = !hasChildren && hasValue && (cleanValue.length() <= maxLength);

    if (isSimple) {
        result += indent + openingTag + cleanValue + "</" + pureName + ">";
    }
    else {
        result += indent + openingTag;

        if (hasValue) {
            result += "\r\n" + getIndentation(level + 1) + cleanValue;
        }

        for (auto child : node->children) {
            formatRecursive(child, level + 1, maxLength, result);
        }

        result += "\r\n" + indent + "</" + pureName + ">";
    }
}

std::string XMLTree::getFormattedXML(TreeNode* root, size_t maxLength) const {
    std::string result = "";
    if (!root) return "";
    // This loop starts from the children of the internal dummy root
    for (auto child : root->children) {
        formatRecursive(child, 0, maxLength, result);
    }
    return result;
}