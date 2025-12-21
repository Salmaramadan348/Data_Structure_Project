#include "XMLToUsersParser.h"
#include "User.h"
#include <iostream>


bool isNumber(const std::string& s) {
    for (char c : s) if (!std::isdigit(c)) return false;
    return !s.empty();
}

std::vector<User> parseUsersFromTags(const std::vector<std::string>& tags) {
    std::vector<User> users;
    User current;
    Post currentPost;


    bool insideUser = false;
    bool insidePosts = false;
    bool insideBody = false;
    bool insideTopic = false;
    bool insideFollowers = false;
    bool insideFollower = false;
    bool insideName = false;
    std::string lastTag;

    for (const auto& token : tags) {

        if (token == "<user>") {
            insideUser = true;
            current = User();
        }
        else if (token == "</user>") {
            users.push_back(current);
            insideUser = false;
        }
        else if (!insideUser) continue;

        else if (token == "<name>") insideName = true;
        else if (token == "</name>") insideName = false;

        else if (token == "<post>") { insidePosts = true; currentPost = Post(); }
        else if (token == "</post>") { 
            
            current.posts.push_back(currentPost);
            insidePosts = false; }

        else if (token == "<body>") insideBody = true;
        else if (token == "</body>") insideBody = false;

        else if (token == "<topic>") insideTopic = true;
        else if (token == "</topic>") insideTopic = false;

        else if (token == "<followers>") insideFollowers = true;
        else if (token == "</followers>") insideFollowers = false;

        else if (token == "<follower>") insideFollower = true;
        else if (token == "</follower>") insideFollower = false;

        else if (token == "<id>") {}
        else if (token == "</id>") {}

        else {
            if (insideName) {
                current.name += token+ " ";
            }

            if (insideFollower && lastTag == "<id>" && isNumber(token)) {
                current.followers.push_back(stoi(token)); 
            }
            else if (insideBody) {

                currentPost.body+=token+"\n";
            }
            else if (insideTopic) {

                currentPost.topics.push_back(token);
            }
            
            else if (!insideFollowers && !insidePosts && lastTag == "<id>" && isNumber(token)) {
                current.id = stoi(token); 
            }
        }

        lastTag = token;
    }

    return users;
}
