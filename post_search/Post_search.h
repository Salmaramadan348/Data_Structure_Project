
#ifndef POST_SEARCH
#define POST_SEARCH
using namespace std;

#include <vector>
#include <string>
#include "User.h"
#include <iostream>



vector<Post> searchByWord(const vector<User>& users, string word);
vector<Post> searchByTopic(const vector<User>& users, string search_topic);

#endif