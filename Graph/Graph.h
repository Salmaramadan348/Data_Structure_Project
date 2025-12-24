#ifndef GRAPH_H
#define GRAPH_H

#include <unordered_map>
#include <vector>
#include <string>
#include "User.h"
using namespace std;

class Graph {
private:
    unordered_map<int, User> usersMap;
    unordered_map<int, vector<int>> outgoing;
    unordered_map<int, vector<int>> incoming;

public:
    void buildGraph(const vector<User>& users);
    vector<int> mostInfluencer();
    vector<int> mostActive();
    vector<int> mutualFollowers(const vector<int>& ids);
    vector<int> suggest(int id);

    string getUserName(int id) {
        if (usersMap.find(id) != usersMap.end())
            return usersMap[id].name;
        return "";
    }

    const unordered_map<int, vector<int>>& getOutgoing() const { return outgoing; }
    const unordered_map<int, vector<int>>& getIncoming() const { return incoming; }
};



#endif
