#include <iostream>
#include <list>
#include <string>

struct Node {
    std::string data;
    std::list<Node*> children;

    Node(std::string value) : data(value) {}

    void insert(std::string value) {
        Node* newNode = new Node(value);
        children.push_back(newNode);
    }

    // child must be already in heap
    void insert(Node *child) {
        children.push_back(child);
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

    void deleteTree(Node* currentNode) {
        if (currentNode == nullptr) {
            return;
        }

        for (Node* child : currentNode->children) {
            deleteTree(child);
        }

        delete currentNode;
    }

    ~Tree() {
        deleteTree(root);
    }
};