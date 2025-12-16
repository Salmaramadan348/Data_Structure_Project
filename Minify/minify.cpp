#include <minify.h>
#include <cctype>
#include <string>

using namespace std;

// Remove leading and trailing spaces
string trim(const string& s) {
    int start = 0, end = s.size() - 1;

    while (start <= end && isspace(s[start])) start++;
    while (end >= start && isspace(s[end])) end--;

    return s.substr(start, end - start + 1);
}

// Minify XML by removing unnecessary spaces
string minifyXML(const string& xml) {
    string result;          // Final output
    string textBuffer;      // Text outside tags
    bool insideTag = false; // Track if inside XML tag

    for (size_t i = 0; i < xml.size(); i++) {
        char c = xml[i];

        if (c == '<') {
            // Add trimmed text before tag
            if (!textBuffer.empty()) {
                result += trim(textBuffer);
                textBuffer.clear();
            }
            insideTag = true;
            result += c;
        }
        else if (c == '>') {
            insideTag = false;
            result += c;
        }
        else {
            if (insideTag) {
                // Skip spaces inside tags
                if (!isspace(c))
                    result += c;
            } else {
                // Collect text outside tags
                textBuffer += c;
            }
        }
    }
    return result;
}
