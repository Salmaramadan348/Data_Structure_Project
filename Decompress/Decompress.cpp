#include "Decompress.h" 
#include <map>
#include <iostream>  

using namespace std;

string SimpleXMLDecompressor::readFile(const string& filename) {
    ifstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not open file " << filename << endl;
        return "";
    }
    file.seekg(0, ios::end);
    size_t fileSize = file.tellg();
    file.seekg(0, ios::beg);

    string content(fileSize, ' ');
    file.read(&content[0], fileSize);
    file.close();

    return content;
}

void SimpleXMLDecompressor::writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not write to file " << filename << endl;
        return;
    }

    file.write(content.c_str(), content.size());
    file.close();
}

void SimpleXMLDecompressor::saveLZWFile(const std::string& filename, const std::string& data, int bitsPerCode) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;

    // Simple header: bits per code (4 bytes)
    file.write(reinterpret_cast<const char*>(&bitsPerCode), sizeof(int));

    // Compressed data
    file.write(data.c_str(), data.length());
    file.close();
}

// Load compressed file WITH dictionary
string SimpleXMLDecompressor::loadLZWFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::binary);
    if (!file.is_open()) return "";

    // Read header
    int bitsPerCode;
    file.read(reinterpret_cast<char*>(&bitsPerCode), sizeof(int));
    // Read remaining data
    file.seekg(0, std::ios::end);
    size_t fileSize = file.tellg();
    file.seekg(sizeof(int), std::ios::beg);

    size_t dataSize = fileSize - sizeof(int);
    std::string data(dataSize, ' ');
    file.read(&data[0], dataSize);
    file.close();

    return data;
}

void SimpleXMLDecompressor::initializeDictionary() {
    compressDict.clear();
    decompressDict.clear();

    // Initialize with all 256 possible bytes
    for (int i = 0; i < 256; i++) {
        std::string s(1, (char)i);
        compressDict[s] = i;
        decompressDict[i] = s;
    }
    nextCode = 256;
}

string SimpleXMLDecompressor::codesToString(const vector<int>& codes, int bitsPerCode) {
    string result;
    unsigned int buffer = 0;
    int bitsInBuffer = 0;

    for (int code : codes) {
        buffer = (buffer << bitsPerCode) | code;
        bitsInBuffer += bitsPerCode;

        while (bitsInBuffer >= 8) {
            bitsInBuffer -= 8;
            result += (char)((buffer >> bitsInBuffer) & 0xFF);
        }
    }
    // Flush remaining bits  
    if (bitsInBuffer > 0) {
        buffer <<= (8 - bitsInBuffer);
        result += (char)(buffer & 0xFF);
    }

    return result;
}


// Convert binary string back to codes
vector<int> SimpleXMLDecompressor::stringToCodes(const string& data, int bitsPerCode) {
    vector<int> stringToCodes(const string & data, int bitsPerCode);
    vector<int> codes;
    unsigned int buffer = 0;
    int bitsInBuffer = 0;
    int maxCode = (1 << bitsPerCode) - 1;

    for (unsigned char c : data) {
        buffer = (buffer << 8) | c;
        bitsInBuffer += 8;

        while (bitsInBuffer >= bitsPerCode) {
            bitsInBuffer -= bitsPerCode;
            int code = (buffer >> bitsInBuffer) & maxCode;
            codes.push_back(code);
        }
    }
    return codes;
}

void SimpleXMLDecompressor::decompress(const string& inputFile, const string& outputFile) {
    // Load LZW file
    string compressed = loadLZWFile(inputFile);
    if (compressed.empty()) return;

    cout << "Decompressing " << inputFile << " (" << compressed.length() << " bytes)...\n";

    // Convert binary string back to codes
    vector<int> codes = stringToCodes(compressed, 12);

    // LZW Decompression
    initializeDictionary();

    if (codes.empty()) {
        writeFile(outputFile, "");
        return;
    }
    // Start with first code
    int oldCode = codes[0];
    string output = decompressDict[oldCode];
    string s = output;
    string c;

    for (size_t i = 1; i < codes.size(); i++) {
        int newCode = codes[i];

        if (decompressDict.find(newCode) == decompressDict.end()) {
            s = decompressDict[oldCode];
            s = s + c;
        }
        else {
            s = decompressDict[newCode];
        }

        output += s;
        c = s.substr(0, 1);

        // Add to dictionary BEFORE next iteration
        if (nextCode < 4095) {
            decompressDict[nextCode] = decompressDict[oldCode] + c;
            nextCode++;
        }

        oldCode = newCode;
    }

    writeFile(outputFile, output);
    cout << "Decompressed size: " << output.length() << " bytes\n";
}
