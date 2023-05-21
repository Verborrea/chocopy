#ifndef PARSER_HPP
#define PARSER_HPP

#include "Scanner.hpp"

class Parser
{
    Scanner scanner;
    Token   current;
    
    void throwError(std::string msg);

    bool expr();
    bool exprPrime();
    bool orExpr();
    bool orExprPrime();
    bool andExpr();
    bool andExprPrime();
    bool notExpr();

    bool compExpr();
    bool compExprPrime();
    bool intExpr();
    bool intExprPrime();
    bool term();
    bool termPrime();
    bool factor();

    bool name();
    bool nameTail();

    bool list();
    bool exprList();
    bool exprListTail();

    bool literal();
    bool compOp();
public:
    Parser(std::string filepath, bool d_mode);

    bool parse();
};

#endif // PARSER_HPP