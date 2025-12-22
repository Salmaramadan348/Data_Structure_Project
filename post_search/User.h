
#ifndef USER_H
#define USER_H

#include <string>
#include <vector>
using namespace std;


class Post {
public:
    string body;
    vector<string> topics;
};

class User {
public:
    int id;
    string name;
    vector<Post> posts;
    vector<int> followers;
};

#endif