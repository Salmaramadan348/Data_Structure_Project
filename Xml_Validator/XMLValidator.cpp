#include <iostream>
#include <stack>
#include <vector>
#include <string>
using namespace std;

bool isOpeningTag(const string& tag) {
    return !tag.empty() && tag[0] == '<' && tag[1] != '/' && tag.back() == '>';
}

bool isClosingTag(const string& tag) {
    return !tag.empty() && tag[0] == '<' && tag[1] == '/' && tag.back() == '>';
}

string getTagName(const string& tag) {
    if (!tag.empty() && tag.front() == '<' && tag.back() == '>') {
        if (tag[1] == '/') return tag.substr(2, tag.size() - 3);//if the tag is closing tag the name of tag after / at index 2
        return tag.substr(1, tag.size() - 2);//if the tag is opening tag the name of tag after < at index 1
    }
    return "";
}


vector<string> extractTags(const string& xmlText) {
    vector<string> tags;
    string current; //for the content between tags
    size_t i = 0;

    while (i < xmlText.size()) {
        if (xmlText[i] == '<') {

            if (!current.empty()) {
                string content = current;
                while (!content.empty() && isspace(content.front())) content.erase(0, 1);//remove leading spaces
                while (!content.empty() && isspace(content.back())) content.pop_back();//remove trailing spaces
                if (!content.empty()) tags.push_back(content);//add all content 
                current.clear();//clear current for next content
            }

            size_t j = i + 1;//find the end of the tag
            while (j < xmlText.size() && xmlText[j] != '>') j++; //find the closing '>' of the tag

            if (j < xmlText.size()) {
                string tag = xmlText.substr(i, j - i + 1);//extract the tag name
                tags.push_back(tag);
                i = j + 1;
            }
            else {
                current += xmlText[i];//if no closing '>' found, treat as content
                i++;
            }
        }
        else {
            current += xmlText[i];//add to content
            i++;
        }
    }

    if (!current.empty()) {
        string content = current;//process any remaining content
        while (!content.empty() && isspace(content.front())) content.erase(0, 1);
        while (!content.empty() && isspace(content.back())) content.pop_back();
        if (!content.empty()) tags.push_back(content);//add remaining content
    }

    return tags;//return all tags and content
}


bool checkXML(const vector<string>& lines) {

    string xmlContent = "";
    for (size_t i = 0; i < lines.size(); i++) {
        xmlContent += lines[i] + "\n";
    }

    vector<string> tokens = extractTags(xmlContent);

    stack<pair<string, int>> st;//pair of tag name and line number
    bool valid = true;
    vector<string> errors;

    int currentPos = 0;//to track position in xmlContent

    for (int i = 0; i < (int)tokens.size(); i++) {

        const string& token = tokens[i];//current tag or content

        size_t foundPos = xmlContent.find(token, currentPos);//find position of token in original content

        int lineNumber = 1;

        if (foundPos != (size_t)-1) {  
            lineNumber = 1;

            for (size_t j = 0; j < foundPos; j++) {
                if (xmlContent[j] == '\n')
                    lineNumber++;
            }

            currentPos = foundPos + token.length();//update current position
        }

        if (isOpeningTag(token)) {

            st.push(make_pair(getTagName(token), lineNumber));//push opening tag and line number onto stack
        }
        else if (isClosingTag(token)) {

            string closingName = getTagName(token);//get name of closing tag

            if (st.empty()) {
                errors.push_back("Extra closing tag: " + token + " at line " +
                    to_string(lineNumber));
                valid = false;
                continue;
            }

            string openName = st.top().first;//get name of last opened tag
            int openLine = st.top().second;//get line number of last opened tag

            if (openName != closingName) {

                bool foundInStack = false;
                stack<pair<string, int>> tempStack = st;//copy of original stack
                vector<pair<string, int>> unclosedTags;//to store unclosed tags

                while (!tempStack.empty()) {

                    if (tempStack.top().first == closingName) {
                        foundInStack = true;
                        break;
                    }

                    unclosedTags.push_back(
                        make_pair(tempStack.top().first, tempStack.top().second)//store unclosed tag name and line number
                    );

                    tempStack.pop();
                }

                if (foundInStack) {//if the closing tag was found in the stack

                    errors.push_back("Missing closing tags for: ");

                    for (size_t k = 0; k < unclosedTags.size(); k++) {
                        errors.push_back(
                            "   - </" + unclosedTags[k].first +
                            "> (opened at line " +
                            to_string(unclosedTags[k].second) + ")"
                        );
                    }

                    st = tempStack;
                    st.pop();
                }
                else {

                    errors.push_back(
                        "Tag mismatch: " + token +
                        " at line " + to_string(lineNumber) +
                        " doesn't match <" + openName + "> at line " +
                        to_string(openLine)
                    );

                    st.pop();
                }

                valid = false;
            }
            else {
                st.pop();
            }
        }
    }

    while (!st.empty()) {

        string tag = st.top().first;
        int line = st.top().second;
        st.pop();

        errors.push_back(
            "Missing closing tag: </" + tag +
            "> for <" + tag + "> at line " + to_string(line)
        );

        valid = false;
    }

    for (size_t i = 0; i < errors.size(); i++) {
        cout << errors[i] << endl;
    }

    return valid;
}


string fixXML(const vector<string>& tags) {
    stack<string> st;            //to track opened tags    
    vector<string> output;     //to store the fixed XML structure       
    stack<bool> hasContent;    //to track if a tag has content       

    for (size_t i = 0; i < tags.size(); i++) {

        const string tag = tags[i];

        if (isOpeningTag(tag)) {

            string tagName = getTagName(tag);

            if (!st.empty() && hasContent.top() && st.top() != tagName) {
                output.push_back("</" + st.top() + ">");
                st.pop();
                hasContent.pop();
            }

            st.push(tagName);
            hasContent.push(false);
            output.push_back(tag);
        }

        else if (isClosingTag(tag)) {

            string closingName = getTagName(tag);

            if (!st.empty()) {

                string correctName = st.top();
                output.push_back("</" + correctName + ">");
                st.pop();
                hasContent.pop();
            }
            else {
                output.push_back("<" + closingName + ">");
                output.push_back(tag);
            }
        }

        else {

            string content = tag;

            while (!content.empty() && isspace(content.front()))
                content.erase(0, 1);

            while (!content.empty() && isspace(content.back()))
                content.pop_back();

            if (!content.empty()) {
                output.push_back(content);

                if (!st.empty()) {
                    bool topValue = hasContent.top();
                    hasContent.pop();
                    topValue = true;
                    hasContent.push(topValue);
                }
            }
        }
    }

    while (!st.empty()) {
        output.push_back("</" + st.top() + ">");
        st.pop();
        if (!hasContent.empty()) {
            hasContent.pop();
        }
    }

    string fixedXML = "";

    for (size_t i = 0; i < output.size(); i++) {

        const string t = output[i];

        fixedXML += t;

        if (isOpeningTag(t) || isClosingTag(t))
            fixedXML += "\n";
    }

    return fixedXML;
}




