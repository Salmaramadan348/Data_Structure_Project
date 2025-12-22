#include "Post_search.h"
#include "XmlParser.h"

std::vector<PostResult> searchByTopicWithUser(const std::vector<User>& users, const std::string& key) {
    std::vector<PostResult> results;
    XmlParser parser;
    for (const User& user : users) {
        for (const Post& post : user.posts) {
            for (const std::string& topic : post.topics) {
                if (parser.trim(topic) == key) {
                    results.push_back({ user.id, post });
                    break;
                }
            }
        }
    }
    return results;
}


std::vector<PostResult> searchByWordWithUser(const std::vector<User>& users, const std::string& word) {
    std::vector<PostResult> results;
    XmlParser parser;
    for (const User& user : users) {
        for (const Post& post : user.posts) {
            std::string body = parser.trim(post.body);
            if (body.find(word) != std::string::npos) {
                results.push_back({ user.id, post });
            }
        }
    }
    return results;
}
