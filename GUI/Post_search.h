#ifndef POST_SEARCH
#define POST_SEARCH
using namespace std;

#include <vector>
#include <string>
#include "User.h"
#include <iostream>


struct PostResult {
    int userId;
    Post post;
};

std::vector<PostResult> searchByTopicWithUser(const std::vector<User>& users, const std::string& key);
std::vector<PostResult> searchByWordWithUser(const std::vector<User>& users, const std::string& key);


#endif
