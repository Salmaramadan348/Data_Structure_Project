#include "Tree.h"

Tree::Tree() {
    root = new TreeNode("root");
}

TreeNode* Tree::getRoot() {
    return root;
}
