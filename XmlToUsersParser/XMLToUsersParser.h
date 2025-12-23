#pragma once
#include <vector>
#include <string>
#include "User.h"
#include "XmlParser.h"


std::vector<User> parseUsersFromTags(const std::vector<std::string>& tags);
