#ifndef PARSER_HPP
#define PARSER_HPP

#include "Scanner.hpp"
#include "Tree.hpp"

class Parser
{
    Scanner scanner;
    Token   current;
    Tree    ast;
    std::vector<std::string> errors;
    
    void throwError(std::string msg);
    void addError(std::string msg);

    Node* program();
    Node* assing();
    Node* expr();
public:
    Parser(std::string filepath, bool d_mode);

    bool parse();
};

#endif // PARSER_HPP