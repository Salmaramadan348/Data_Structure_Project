#include "Graph.h"
#include <fstream>
#include <algorithm>
#include <vector>
#include <unordered_set>

using namespace std;

void Graph::buildGraph(const vector<User>& users) {
    outgoing.clear();
    incoming.clear();
    usersMap.clear();


    for (const auto& u : users)
        usersMap[u.id] = u;


    for (const auto& u : users) {
        for (int f : u.followers) {
            outgoing[u.id].push_back(f);
            incoming[f].push_back(u.id);
        }
    }
    for (const auto& u : users) {
        incoming[u.id];
        outgoing[u.id];
    }
}

vector<int> Graph::mostInfluencer() {
    vector<int> result;
    int maxFollowers = -1;

    for (auto& p : usersMap) {
        int id = p.first;

        int followersCount = incoming[id].size();

        if (followersCount > maxFollowers) {
            maxFollowers = followersCount;
            result.clear();
            result.push_back(id);
        }
        else if (followersCount == maxFollowers) {
            result.push_back(id);
        }
    }

    return result;
}

vector<int> Graph::mostActive() {
    vector<int> result;
    int mx = -1;

    for (auto& p : usersMap) {
        int id = p.first;

        int activity = outgoing[id].size();

        if (activity > mx) {
            mx = activity;
            result.clear();
            result.push_back(id);
        }
        else if (activity == mx) {
            result.push_back(id);
        }
    }

    return result;
}

vector<int> Graph::mutualFollowers(const vector<int>& ids) {
    if (ids.empty()) return {};

    vector<int> res = incoming[ids[0]];

    for (int k = 1; k < (int)ids.size(); k++) {
        vector<int> temp;
        for (int x : incoming[ids[k]]) {
            if (find(res.begin(), res.end(), x) != res.end())
                temp.push_back(x);
        }
        res = temp;
    }
    return res;
}

vector<int> Graph::suggest(int id) {
    vector<int> suggestions;
    unordered_set<int> myFollowingSet;
    unordered_set<int> suggestionsSet;


    for (int followingId : outgoing[id]) {
        myFollowingSet.insert(followingId);
    }

    myFollowingSet.insert(id);

    for (int friendId : outgoing[id]) {
        for (int friendOfFriendId : outgoing[friendId]) {
            if (friendOfFriendId != id &&
                myFollowingSet.find(friendOfFriendId) == myFollowingSet.end()) {

                if (suggestionsSet.find(friendOfFriendId) == suggestionsSet.end()) {
                    suggestions.push_back(friendOfFriendId);
                    suggestionsSet.insert(friendOfFriendId);
                }
            }
        }
    }

    return suggestions;
}