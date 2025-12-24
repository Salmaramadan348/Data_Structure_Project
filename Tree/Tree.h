#pragma once
#ifndef TREE_H
#define TREE_H

#include "TreeNode.h"

class Tree {
private:
    TreeNode* root;

public:
    Tree() {
        // The validator expects a dummy root named "root"
        root = new TreeNode("root");
    }

    ~Tree() {
        delete root;
    }

    TreeNode* getRoot() {
        return root;
    }
};

#endif