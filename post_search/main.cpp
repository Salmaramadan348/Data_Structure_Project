#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XMLValidator.h"
#include "Xml_to_Json.h"
#include "User.h"
#include <iostream>
#include "XMLToUsersParser.h"
#include "Post_search.h"
#include "XmlParser.h"

using namespace std;


string readFile(const string& path) {
    ifstream file(path);
    if (!file) { cerr << "Cannot open file: " << path << endl; return ""; }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}



int main() {
    string word;
    string topic;
    string xmlText = readFile("C:/Users/dinas/source/repos/test_dsa/test_dsa/sample.xml");
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


    vector<User> users = parseUsersFromTags(tags);
    cout << "User posts:\n";
    if (users.size() > 1) {
        for (const Post& p : users[0].posts) {
            cout << p.body << endl;

            for (const string& topic : p.topics) {

                cout << topic << endl;
            }
        }


        cout << "Search by Word ";
        cin >> word;

        out << "Search by Topic ";
        cin >> topic;

        vector<Post> related_posts = searchByWord(users, word);
        cout << related_posts.size() << endl;

        vector<Post> related_posts_topic = searchByTopic(users, topic);
        cout << related_posts_topic.size() << endl;
    }


    return 0;
}




