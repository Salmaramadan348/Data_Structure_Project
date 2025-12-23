//#ifndef TREENODE_H
//#define TREENODE_H
//
//#include <string>
//#include <vector>
//
//struct TreeNode {
//    std::string name;       // Tag name + attributes (e.g., "book category=\"web\"")
//    std::string tagValue;   // Text content
//    TreeNode* parent;       // Needed for XmlValidator.cpp
//    std::vector<TreeNode*> children;
//
//    TreeNode(std::string n = "") : name(n), parent(nullptr) {}
//
//    // Method used by unchangeable XmlValidator.cpp
//    void addChild(TreeNode* child) {
//        if (child) {
//            child->parent = this;
//            children.push_back(child);
//        }
//    }
//
//    ~TreeNode() {
//        for (auto child : children) delete child;
//    }
//};
//
//#endif
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