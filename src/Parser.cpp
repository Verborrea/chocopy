#include "Parser.hpp"

Parser::Parser(std::string path, bool d_mode)
{
    scanner.setFile(path);
    scanner.setDebugMode(d_mode);
}

void Parser::throwError(std::string msg)
{
    std::cerr << "\x1B[31mError: " << msg << "\033[0m" << std::endl;
}

bool Parser::compOp()
{
    bool comp_op = false;

    comp_op |= current.lex == "==";
    comp_op |= current.lex == "!=";
    comp_op |= current.lex == "<";
    comp_op |= current.lex == ">";
    comp_op |= current.lex == "<=";
    comp_op |= current.lex == ">=";
    comp_op |= current.lex == "is";

    if (comp_op)
        current = scanner.nextToken();
    return comp_op;
}

bool Parser::literal()
{
    bool literal = false;
    
    literal |= current.lex == "True";
    literal |= current.lex == "False";
    literal |= current.lex == "None";
    literal |= current.pos == "INTEGER";
    literal |= current.pos == "STRING";

    if (literal)
        current = scanner.nextToken();
    return literal;
}

bool Parser::exprListTail()
{
    if (current.pos == "COMMA") {
        current = scanner.nextToken();
        if (expr())
            if (exprListTail())
                return true;
        else {
            throwError("lista no puede terminar en coma");
            return false;
        }
    }
    else if (current.pos == "CLO_BRA")
        return true;
    return false;
}

bool Parser::exprList()
{
    if (expr())
        if (exprListTail())
            return true;
    else if (current.pos == "CLO_BRA")
        return true;
    return false;
}

bool Parser::list()
{
    if (current.pos == "OPEN_BRA") { // [3,2,1]
        current = scanner.nextToken();
        if (exprList()) {
            if (current.pos == "CLO_BRA") {
                current = scanner.nextToken();
                return true;
            }
            throwError("lista no cerrada");
            return false;
        }
    }
    
    return false;
}

bool Parser::nameTail()
{
    if (list())
        return true;
    if (current.pos == "OPEN_PAR") {
        current = scanner.nextToken();
        if (exprList()) {
            if (current.pos == "CLO_PAR") {
                current = scanner.nextToken();
                return true;
            }
        }
        throwError("parentesis no cerrados");
        return false;
    }
    // comprobar follow

    return false;
}

bool Parser::name()
{
    if (current.pos == "IDNTF") {
        current = scanner.nextToken();
        return nameTail();
    }
    return false;
}

bool Parser::parse()
{
    current = scanner.nextToken();
    if (expr() && current.pos == "NEWLINE")
        return true;  
    std::cout << "Error de sintaxis" << std::endl;
    return false;
}