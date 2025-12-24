#pragma once
#include <vector>
#include <string>
#include "User.h"
#include "XmlParser.h"
using namespace std;

vector<User> parseUsersFromTags(const vector<string>& tags);
