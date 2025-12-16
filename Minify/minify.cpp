#include <minify.h>
#include <cctype>
#include <string>

using namespace std;

string trim(const string& s) {
    int start = 0, end = s.size() - 1;

    while (start <= end && isspace(s[start])) start++;
    while (end >= start && isspace(s[end])) end--;

    return s.substr(start, end - start + 1);
}

string minifyXML(const string& xml) {
    string result;
    string textBuffer;
    bool insideTag = false;

    for (size_t i = 0; i < xml.size(); i++) {
        char c = xml[i];

        if (c == '<') {
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
                if (!isspace(c))
                    result += c;
            } else {
                textBuffer += c;
            }
        }
    }
    return result;
}
