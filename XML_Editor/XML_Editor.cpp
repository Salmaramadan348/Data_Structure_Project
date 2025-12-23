// XML_Editor.cpp : This file contains the 'main' function. Program execution begins and ends there.


#include "XmlValidator.h"
#include "XmlParser.h"
#include "XmlPrettify.h"
#include "Compress.h"
#include "Decompress.h"
#include "Tree.h"
#include "TreeNode.h"
#include "Minify.h"
#include "Post_search.h"
#include "Xml_to_Json.h"
#include "User.h"
#include "XMLToUsersParser.h"
#include "Graph.h"


#include <sstream>
#include <fstream>
#include <string>
#include <vector>
#include <iostream>

using namespace std;

int main(int argc, char** argv)
{
    string mode, inputFile, outputFile, keyword;
    bool topicMode = false, wordMode = false;


    if (argc < 3) {
        cerr << "Usage: XML_Editor.exe action [-t topic/-w word] -i input_file [-o output_file]\n";
        return 1;
    }


    mode = argv[1];


    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "-i" && i + 1 < argc) {
            inputFile = argv[++i];
        }
        else if (arg == "-o" && i + 1 < argc) {
            outputFile = argv[++i];
        }

        else  if (arg == "-t" && i + 1 < argc) {
            keyword = argv[++i];
            topicMode = true;

        }
        else if (arg == "-w" && i + 1 < argc) {
            keyword = argv[++i];
            wordMode = true;
        }

    }

    if (inputFile.empty()) {
        cerr << "Error: Input file not specified.\n";
        return 1;
    }

    // Step 3: add IF blocks for each class

    if (mode == "compress") {
        SimpleXMLCompressor compressor;
        if (outputFile.empty()) outputFile = inputFile + ".pbx";
        compressor.compress(inputFile, outputFile);
        cout << "Compression Successful.\n";
    }
    else if (mode == "format") {
        string xmlText = XmlParser::readFile(inputFile);
        if (xmlText.empty()) {
            cerr << "Failed to read input file.\n";
            return 1;
        }

        XmlValidator validator;
        XmlParser parser;
        XMLTree formatter;

        vector<string> tokens = parser.extractTags(xmlText);
        Tree tree;
        TreeNode* root = tree.getRoot();
        TreeNode* current = root;

        for (const string& token : tokens) {
            string t = parser.trim(token);
            if (t.empty()) continue;
            if (validator.isOpeningTag(t)) {
                TreeNode* newNode = new TreeNode(validator.getTagName(t));
                current->addChild(newNode);
                current = newNode;
            }
            else if (validator.isClosingTag(t)) {
                if (current && current->parent) current = current->parent;
            }
            else {
                if (current != root) current->tagValue = t;
            }
        }

        string formattedXML = formatter.getFormattedXML(root, 15);

       
        if (outputFile.empty()) {
           
            outputFile = inputFile.substr(0, inputFile.find_last_of('.')) + "_formatted.xml";
        }

        ofstream out(outputFile);
        if (!out) {
            cerr << "Failed to open output file.\n";
            return 1;
        }
        out << formattedXML;
        out.close();

        cout << "Formatting successful. Saved to: " << outputFile << endl;
    }



    else if (mode == "decompress") {
        SimpleXMLDecompressor decompressor;
        if (outputFile.empty()) outputFile = inputFile + ".xml";
        decompressor.decompress(inputFile, outputFile);
        cout << "Decompression Successful.\n";
    }
    else if (mode == "minify") {
        ifstream in(inputFile);
        string xml((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();

        string result = minifyXML(xml);
        if (outputFile.empty()) outputFile = inputFile + ".min.xml";
        ofstream out(outputFile);
        out << result;
        out.close();

        cout << "Minification Successful.\n";
    }

    else if (mode == "verify") {
        ifstream in(inputFile);
        string xml((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();

        XmlValidator validator;
        vector<XmlError> errors = validator.checkXml(xml);

        if (errors.empty()) {
            cout << "XML is valid.\n";
        }
        else {
            cout << "XML has errors:\n";
            for (const auto& err : errors) {
                cout << "Line " << err.lineNumber << ": " << err.message << "\n";
            }
        }
    }

    else if (mode == "tojson") {
        ifstream in(inputFile);
        string xml((istreambuf_iterator<char>(in)), istreambuf_iterator<char>());
        in.close();
        cout << "Starting JSON conversion...\n";

        string json = convertXMLtoJSON(xml);
        if (outputFile.empty()) outputFile = inputFile + ".json";
        ofstream out(outputFile);
        out << json;
        out.close();

        cout << "Conversion to JSON Successful.\n";
    }

    else if (mode == "search") {


        string inputFile_1 = XmlParser::readFile(inputFile);
        vector<string> tags = XmlParser::extractTags(inputFile_1);

        vector<User> users = parseUsersFromTags(tags);

        if (wordMode)
        {
            vector<PostResult> related_posts = searchByWordWithUser(users, keyword);
            cout << related_posts.size() << " related posts  " << endl;


            for (const PostResult& p : related_posts) {
                cout << "Post of user " << p.userId << " for the word *" << keyword << "*" << endl;
                cout << p.post.body << endl;
            }
        }
        else if (topicMode) {
            vector<PostResult> related_posts_topic = searchByTopicWithUser(users, keyword);
            cout << related_posts_topic.size() << " related topics " << endl;


            for (const PostResult& p : related_posts_topic) {
                cout << "Post of user " << p.userId << " for the topic *" << keyword << "*" << endl;
                cout << p.post.body << endl;
            }
        }
    }
    else {
        string xmlText = XmlParser::readFile(inputFile);
        if (xmlText.empty()) return 1;

        vector<string> tags = XmlParser::extractTags(xmlText);
        vector<User> users = parseUsersFromTags(tags);

        Graph g;
        g.buildGraph(users);

        if (mode == "most_active") {
            auto res = g.mostActive();
            cout << "Most Active Users: ";
            for (int id : res) cout << id << " ";
            cout << endl;
        }

        else if (mode == "most_influencer") {
            auto res = g.mostInfluencer();
            cout << "Most Influencer Users: ";
            for (int id : res) cout << id << " ";
            cout << endl;
        }

        else if (mode == "mutual") {
            vector<int> ids;
            for (int i = 2; i < argc; i++) {
                string arg = argv[i];
                if (arg == "-ids" && i + 1 < argc) {
                    stringstream ss(argv[++i]);
                    string temp;
                    while (getline(ss, temp, ',')) {
                        ids.push_back(stoi(temp));
                    }
                }
            }

            auto res = g.mutualFollowers(ids);
            cout << "Mutual Followers: ";
            for (int id : res) cout << id << " ";
            cout << endl;
        }

        else if (mode == "suggest") {
            int userId = -1;
            for (int i = 2; i < argc; i++) {
                if (string(argv[i]) == "-id" && i + 1 < argc) {
                    userId = stoi(argv[++i]);
                }
            }

            auto res = g.suggest(userId);
            cout << "Suggested Users: ";
            for (int id : res) cout << id << " ";
            cout << endl;
        }

        else {
            cerr << "Unknown action: " << mode << endl;
        }
    }

    return 0;
}