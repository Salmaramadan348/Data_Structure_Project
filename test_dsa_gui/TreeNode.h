#pragma once
#include <string>
#include <vector>
using namespace std;

struct TreeNode {
    string name;
    string tagValue;
    TreeNode* parent;
    vector<TreeNode*> children;

    TreeNode(string n, string v = "") : name(n), tagValue(v), parent(nullptr) {}

    void addChild(TreeNode* child) {
        child->parent = this;
        children.push_back(child);
    }
};