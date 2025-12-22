#ifndef COMPRESS_H
#define COMPRESS_H

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <fstream>
#include <iomanip>

using namespace std;

class SimpleXMLCompressor {
private:
    map<std::string, int> compressDict;  // string -> code
    std::map<int, std::string> decompressDict; // code -> string
    int nextCode;


    string readFile(const string& filename);
    void writeFile(const string& filename, const string& content);
    void saveLZWFile(const std::string& filename, const std::string& data, int bitsPerCode);
    string loadLZWFile(const std::string& filename);
    void initializeDictionary();
    string codesToString(const vector<int>& codes, int bitsPerCode = 12);
    vector<int> stringToCodes(const string& data, int bitsPerCode = 12);

public:
    void compress(const string& inputFile, const string& outputFile);
};


#endif // !COMPRESS_H