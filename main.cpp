#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XMLValidator.h"
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
    // Compress and Decompress main code ==>> Yomna
    int choice;

    cout << "=== XML File Compression & Decompression Tool ===\n";
    cout << "1. Compress an XML file\n";
    cout << "2. Decompress a compressed file\n";
    cout << "3. Exit\n";
    cout << "Enter your choice (1-3): ";
    cin >> choice;
    cin.ignore();  // Clear newline

    switch (choice) {
    case 1: {
        SimpleXMLCompressor compressor;
        string inputFile, outputFile;

        cout << "Enter input XML filename: ";
        getline(cin, inputFile);

        cout << "Enter output compressed filename: ";
        getline(cin, outputFile);

        compressor.compress(inputFile, outputFile);
        break;
    }
    case 2: {
        SimpleXMLDecompressor decompressor;

        string inputFile, outputFile;

        cout << "Enter input compressed filename: ";
        getline(cin, inputFile);

        cout << "Enter output XML filename: ";
        getline(cin, outputFile);

        decompressor.decompress(inputFile, outputFile);
        break;
    }
    case 3:
        cout << "Goodbye!\n";
        break;

    default:
        cout << "Invalid choice!\n";
    }
    
    return 0;
}
