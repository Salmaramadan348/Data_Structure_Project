#ifndef XML_TO_USERS_PARSER_H
#define XML_TO_USERS_PARSER_H
using namespace std;

#include <vector>
#include <string>
#include "User.h"

// Convert the tags extracted by XMLValidator into user structures
vector<User> parseUsersFromTags(const vector<string>& tags);

#endif
