#ifndef TREE_HPP
#define TREE_HPP

#include <iostream>
#include <list>
#include <string>

struct Node {
    std::string data;
    std::list<Node*> children;

    Node(std::string value) : data(value) {
        std::cout << "Crear " + data << std::endl;
    }

    ~Node() {
        std::cout << "Borrar " + data << std::endl;
    }

    void insert(std::string value) {
        Node* newNode = new Node(value);
        children.push_back(newNode);
    }

    // child must be already in heap
    void insert(Node *child) {
        children.push_back(child);
    }

    static void podate(Node* currentNode) {
        if (currentNode == nullptr) {
            return;
        }

        for (Node* child : currentNode->children) {
            podate(child);
        }

        delete currentNode;
    }
};

struct Tree {
    Node* root;
    Tree() : root(nullptr) {}
    
    void printTree(Node* currentNode, const std::string& prefix = "", bool isLastChild = true) {
        if (currentNode == nullptr) {
            return;
        }

        std::cout << prefix;
        if (currentNode != root)
            std::cout << (isLastChild ? "└── " : "├── ");
        std::cout << currentNode->data << "\n";

        const std::string& childPrefix = prefix + (isLastChild ? "    " : "│   ");
        const std::string& lastChildPrefix = prefix + (isLastChild ? "    " : "│   ");

        const size_t numChildren = currentNode->children.size();
        size_t childIndex = 0;
        for (Node* child : currentNode->children) {
            const bool isLast = (++childIndex == numChildren);
            printTree(child, isLast ? lastChildPrefix : childPrefix, isLast);
        }
    }

    ~Tree() {
        root->podate(root);
    }
};

#endif // TREE_HPP