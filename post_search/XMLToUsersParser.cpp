#include "XMLToUsersParser.h"
#include "User.h"
#include "XmlParser.h"

#include <iostream>
#include <cctype>
#include <vector>
#include <string>

// ---------------- Helper ----------------
bool isNumber(const std::string& s) {
    for (char c : s)
        if (!std::isdigit(static_cast<unsigned char>(c)))
            return false;
    return !s.empty();
}

// ---------------- Main Parser ----------------
std::vector<User> parseUsersFromTags(const std::vector<std::string>& tags) {
    std::vector<User> users;
    User currentUser;
    Post currentPost;
    XmlParser parser;

    bool insideUser = false;
    bool insideName = false;
    bool insidePosts = false;
    bool insidePost = false;
    bool insideBody = false;
    bool insideTopic = false;
    bool insideFollowers = false;
    bool insideFollower = false;
    bool insideId = false;

    enum class IdContext { None, UserId, FollowerId };
    IdContext idContext = IdContext::None;

    for (const auto& token : tags) {
        std::string value = parser.trim(token);

        // ---------- TAG HANDLING ----------
        if (token == "<user>") {
            insideUser = true;
            currentUser = User();
        }
        else if (token == "</user>") {
            users.push_back(currentUser);
            insideUser = false;
        }
        else if (token == "<name>") {
            insideName = true;
        }
        else if (token == "</name>") {
            insideName = false;
        }
        else if (token == "<posts>") {
            insidePosts = true;
        }
        else if (token == "</posts>") {
            insidePosts = false;
        }
        else if (token == "<post>") {
            insidePost = true;
            currentPost = Post();
        }
        else if (token == "</post>") {
            currentUser.posts.push_back(currentPost);
            insidePost = false;
        }
        else if (token == "<body>") {
            insideBody = true;
        }
        else if (token == "</body>") {
            insideBody = false;
        }
        else if (token == "<topic>") {
            insideTopic = true;
        }
        else if (token == "</topic>") {
            insideTopic = false;
        }
        else if (token == "<followers>") {
            insideFollowers = true;
        }
        else if (token == "</followers>") {
            insideFollowers = false;
        }
        else if (token == "<follower>") {
            insideFollower = true;
        }
        else if (token == "</follower>") {
            insideFollower = false;
        }
        else if (token == "<id>") {
            insideId = true;
            if (insideUser && !insidePosts && !insideFollowers)
                idContext = IdContext::UserId;
            else if (insideFollower)
                idContext = IdContext::FollowerId;
        }
        else if (token == "</id>") {
            insideId = false;
            idContext = IdContext::None;
        }
        else {
            // ---------- TEXT CONTENT ----------
            if (value.empty()) continue;

            if (insideName) {
                currentUser.name += value + " ";
            }
            else if (insideBody) {
                currentPost.body += value + "\n";
            }
            else if (insideTopic) {
                currentPost.topics.push_back(value);
            }
            else if (insideId) {
                if (idContext == IdContext::UserId && isNumber(value)) {
                    currentUser.id = std::stoi(value);
                }
                else if (idContext == IdContext::FollowerId && isNumber(value)) {
                    currentUser.followers.push_back(std::stoi(value));
                }
            }
        }
    }

    return users;
}
