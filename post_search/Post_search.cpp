
#include "Post_search.h"

using namespace std;



vector<Post> searchByWord(const vector<User>& users,  string word)
{
    vector<Post> relevant_posts;
    for (const User& u : users) {
        for ( const Post& p : u.posts) {
            if (p.body.find(word) != string::npos) {
                relevant_posts.push_back(p);

            }
        }
    }
    return relevant_posts;
}



vector<Post> searchByTopic(const vector<User>& users,  string search_topic)
{
    vector<Post> relevant_posts;
    for (const User& u : users) {
        for (const Post& p : u.posts) {
            for (const string& topic : p.topics){
            
                if (topic == search_topic) {
                    relevant_posts.push_back(p);
                
                }
            

            }
        }
    }
    return relevant_posts;
}
