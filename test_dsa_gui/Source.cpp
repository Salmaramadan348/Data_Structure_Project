#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include "XMLToUsersParser.h"
#include "XmlValidator.h"
using namespace std;
/*
string readFile(const string& path) {
    ifstream file(path);
    if (!file) {
        cerr << "Cannot open file: " << path << endl;
        return "";
    }
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

void printUsers(const vector<int>& users) {
    for (int id : users)
        cout << id << " ";
    cout << endl;
}*/
/*

int main() {
    string xmlText = readFile("sample.xml");
    if (xmlText.empty()) return 1;

    vector<string> tags = extractTags(xmlText);
    vector<User> users = parseUsersFromTags(tags);
    cout << "Parsed users: " << users.size() << endl;

    Graph g;
    g.buildGraph(users);

   
    vector<int> influencers = g.mostInfluencer();
    vector<int> actives = g.mostActive();

    cout << "Most Influencer User(s): ";
    printUsers(influencers);

    cout << "Most Active User(s): ";
    printUsers(actives);

    // ----- mutualFollowers -----
    vector<int> idsToCheck = { 1, 2, 3 }; 
    vector<int> mutuals = g.mutualFollowers(idsToCheck);
    cout << "Mutual Followers of 1,2,3: ";
    printUsers(mutuals);

    // ----- suggest -----
    int userToSuggest = 1; 
    vector<int> suggestions = g.suggest(userToSuggest);
    cout << "Suggestions for user 1: ";
    printUsers(suggestions);

    g.exportDOT("graph.dot");
    cout << "DOT exported!" << endl;

    return 0;
}
*/