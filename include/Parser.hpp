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
    void goThrough(std::vector<std::string> *follow);

    Node* program();
    Node* block(std::string name);
    Node* defList(Node* parent);
    Node* def();

    Node* type();
    Node* typedVar();
    Node* typedVarList();
    Node* typedVarListTail(Node* parent);

    Node* statementList(Node* parent);
    Node* statement();
    Node* statement_error(std::string msg, Node* father);
    Node* elifList(Node* parent);

    Node* simpleStatement();
    Node* ssTail(Node* first);
    Node* returnExpr();

    Node* expr();
    Node* exprPrime(Node* first);
    Node* orExpr();
    Node* orExprPrime(Node* first);
    Node* andExpr();
    Node* andExprPrime(Node* first);
    Node* notExpr();
    Node* compExpr();
    Node* compExprPrime(Node* first);
    Node* intExpr();
    Node* intExprPrime(Node* first);
    Node* term();
    Node* termPrime(Node* first);

    Node* factor();
    Node* literal();
    Node* name();
    Node* nameTail();

    Node* list();
    Node* exprList(std::string name);
    Node* exprListTail(Node* parent);
public:
    Parser(std::string filepath, bool d_mode);

    bool parse();
};

#endif // PARSER_HPP