#include "compress.h" 
#include <map>
#include <iostream>  

using namespace std;

string SimpleXMLCompressor::readFile(const string& filename) {
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

void SimpleXMLCompressor::writeFile(const string& filename, const string& content) {
    ofstream file(filename, ios::binary);
    if (!file.is_open()) {
        cerr << "Error: Could not write to file " << filename << endl;
        return;
    }

    file.write(content.c_str(), content.size());
    file.close();
}

void SimpleXMLCompressor::saveLZWFile(const std::string& filename, const std::string& data, int bitsPerCode) {
    std::ofstream file(filename, std::ios::binary);
    if (!file.is_open()) return;

    // Simple header: bits per code (4 bytes)
    file.write(reinterpret_cast<const char*>(&bitsPerCode), sizeof(int));

    // Compressed data
    file.write(data.c_str(), data.length());
    file.close();
}

// Load compressed file WITH dictionary
string SimpleXMLCompressor::loadLZWFile(const std::string& filename) {
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

void SimpleXMLCompressor::initializeDictionary() {
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

string SimpleXMLCompressor::codesToString(const vector<int>& codes, int bitsPerCode) {
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
vector<int> SimpleXMLCompressor::stringToCodes(const string& data, int bitsPerCode) {
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

void SimpleXMLCompressor::compress(const string& inputFile, const string& outputFile) {
    // Read input file
    string xml = readFile(inputFile);
    if (xml.empty()) {
        cerr << "Error: Input file is empty or not found\n";
        return;
    }

    cout << "Compressing " << inputFile << " (" << xml.length() << " bytes)...\n";

    // LZW Compression
    initializeDictionary();
    std::vector<int> codes;
    std::string current;

    for (char c : xml) {
        std::string next = current + c;
        if (compressDict.find(next) != compressDict.end()) {
            current = next;  // Sequence already in dictionary
        }
        else {
            // Output code for current sequence
            codes.push_back(compressDict[current]);

            // Add new sequence to dictionary
            if (nextCode < 4096) {  // 12-bit codes (0-4095)
                compressDict[next] = nextCode++;
            }

            current = std::string(1, c);  // Start new sequence
        }
    }

    // Output last sequence
    if (!current.empty()) {
        codes.push_back(compressDict[current]);
    }

    // Convert codes to binary string
    std::string compressed = codesToString(codes);

    // Save with LZW-specific header
    saveLZWFile(outputFile, compressed, 12);  // 12 bits per code

    // Statistics
    double ratio = (1.0 - (double)compressed.length() / xml.length()) * 100.0;
    cout << "Compression ratio: " << std::fixed << std::setprecision(2)
        << ratio << "% saved\n";
    cout << "Dictionary size: " << (nextCode - 256) << " new entries\n";
}

