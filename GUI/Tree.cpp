#include "Tree.h"

Tree::Tree() {
    root = new TreeNode("root");
}

<<<<<<< Updated upstream
=======
Tree::~Tree() {
    delete root;
}

>>>>>>> Stashed changes
TreeNode* Tree::getRoot() {
    return root;
}
