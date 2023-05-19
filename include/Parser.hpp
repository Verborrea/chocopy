#ifndef PARSER_HPP
#define PARSER_HPP

#include "Scanner.hpp"

class Parser
{
    Scanner scanner;
    Token   current;
    
    void throwError(std::string msg);

    // bool list(bool (*item)(), std::string lookahead);
    // bool listTail(bool (*item)(), std::string lookahead);

    // bool cexpr_3();
    // bool cexpr_2();
    // bool cexpr();
    bool expr();

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