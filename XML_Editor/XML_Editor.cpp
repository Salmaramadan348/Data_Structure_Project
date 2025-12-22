// XML_Editor.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "XmlValidator.h"
#include  "XmlParser.h"
#include "Compress.h"
#include "Decompress.h"
#include "Tree.h"
#include "TreeNode.h"
#include "Minify.h"
#include "post_search.h"
#include "Xml_to_json.h"


#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{   
    string mode, value, inputFile,outputFile;
     
    //xml_editor action -i input_file.xml -o output_file.xml
    //argv[0]    argv[1]    argv[3]            argv[5]


    if (argc < 4) { 
        
        cerr << "Usage: XML_Editor.exe action -i input_file [-o output_file]\n"; 
        return 1; }

    

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        }
    }

    if (inputFile.empty()) {
        cerr << "Error: Input file not specified.\n";
        return 1;
    }

    if (mode == "compress") {
        SimpleXMLCompressor compressor;

        if (outputFile.empty()) {
            outputFile = inputFile + ".pbx";
            cout << "No output file specified. Using default: " << outputFile << "\n";
        }

        compressor.compress(inputFile, outputFile);
        cout << "Compression Successful.\n";
    }


   // if (mode == "search"){}
    //if (mode == "verify"){}
    //if (mode == "decompress"){}

    return 0;
}
