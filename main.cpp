#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XMLValidator.h"
#include "Xml_to_Json.h"
using namespace std;

string readFile(const string& path) {
    ifstream file(path);
    if (!file) { cerr << "Cannot open file: " << path << endl; return ""; }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

int main() {
    string xmlText = readFile("sample.xml");
    if (xmlText.empty()) return 1;

    vector<string> tags = extractTags(xmlText);

    cout << "Checking XML consistency...\n";
    bool valid = checkXML(tags);

    if (valid) cout << "XML is valid!\n";
    else {
        cout << "XML is invalid!\n";
        string fixedXML = fixXML(tags);
        cout << "\n--- Fixed XML ---\n";
        cout << fixedXML;
    }
    string json = convertXMLtoJSON(xmlText);
    if (!json.empty()) {
        cout << "\n--- JSON Output ---\n";
        cout << json << endl;
    }
    return 0;
}
