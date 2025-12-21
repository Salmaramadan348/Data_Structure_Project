#ifndef DECOMPRESS_H
#define DECOMPRESS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream> // to read the file
#include <iomanip>

using namespace std;

class SimpleXMLDecompressor {
private:
    map<std::string, int> compressDict;  // string -> code
    map<int, std::string> decompressDict; // code -> string
    int nextCode;


    string readFile(const string& filename);
    void writeFile(const string& filename, const string& content);
    void saveLZWFile(const std::string& filename, const std::string& data, int bitsPerCode);
    string loadLZWFile(const std::string& filename);
    void initializeDictionary();
    string codesToString(const vector<int>& codes, int bitsPerCode = 12);
    vector<int> stringToCodes(const string& data, int bitsPerCode = 12);

public:
    void decompress(const string& inputFile, const string& outputFile);
};


#endif // !DECOMPRESS_H
