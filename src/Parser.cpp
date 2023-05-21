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

bool Parser::expr()
{
    return orExpr() && exprPrime();
}

bool Parser::exprPrime()
{
    if (current.lex == "if") {
        current = scanner.nextToken();
        if (orExpr()) {
            if (current.lex == "else") {
                current = scanner.nextToken();
                if (orExpr())
                    return exprPrime();
                throwError("expected expresion after else");
                return false;
            }
        }
        else {
            throwError("expected expresion");
            return false;
        }
    }
    // comprobar follow
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    return false;
}

bool Parser::orExpr()
{
    return andExpr() && orExprPrime();
}

bool Parser::orExprPrime()
{
    if (current.lex == "or") {
        current = scanner.nextToken();
        if (andExpr()) {
            return orExprPrime();
        }
        throwError("expected andExpr");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else"};
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 2; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::andExpr()
{
    return notExpr() && andExprPrime();
}

bool Parser::andExprPrime()
{
    if (current.lex == "and") {
        current = scanner.nextToken();
        if (notExpr()) {
            return andExprPrime();
        }
        throwError("expected notExpr");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else","or"};
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 3; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::notExpr()
{
    if (current.lex == "not") {
        current = scanner.nextToken();
        return notExpr();
    }
    return compExpr();
}

bool Parser::compExpr()
{
    return intExpr() && compExprPrime();
}

bool Parser::compExprPrime()
{
    if (compOp()) {
        if (intExpr()) {
            return compExprPrime();
        }
        throwError("expected intTerm");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else","or","and"};
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 4; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::intExpr()
{
    return term() && intExprPrime();
}

bool Parser::intExprPrime()
{
    if (current.lex == "+" || current.lex == "-") {
        current = scanner.nextToken();
        if (term()) {
            return intExprPrime();
        }
        throwError("expected term");
        return false;
    }
    // comprobar follow
    std::string follow[] = {
        "if","else","or","and","==","!=",">=","<=","<",">","is"
    };
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 11; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::term()
{
    return factor() && termPrime();
}

bool Parser::termPrime()
{
    if (current.lex == "*" || current.lex == "//" || current.lex == "%") {
        current = scanner.nextToken();
        if (factor()) {
            return termPrime();
        }
        throwError("expected factor");
        return false;
    }
    // comprobar follow
    std::string follow[] = {
        "if","else","or","and","==","!=",">=","<=","<",">","is","+","-"
    };
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 13; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::factor()
{
    if (current.lex == "-") {
        current = scanner.nextToken();
        return factor();
    }
    if (name())
        return true;
    if (literal())
        return true;
    if (list())
        return true;
    if (current.pos == "OPEN_PAR") {
        current = scanner.nextToken();
        if (expr()) {
            if (current.pos == "CLO_PAR") {
                current = scanner.nextToken();
                return true;
            }
        }
        throwError("parentesis no cerrados");
        return false;
    }
    throwError("expected factor");
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
    std::string follow[] = {
        "if","else","or","and","==","!=",">=","<=","<",">","is","+","-",
        "*","//","%"
    };
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.pos == "CLOSE_PAR")
        return true;
    for (int i = 0; i < 13; i++)
        if (current.lex == follow[i])
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

bool Parser::exprList()
{
    if (expr() & exprListTail())
        return true;
    else if (current.pos == "CLO_BRA")
        return true;
    throwError("Invalid Expresion List");
    return false;
}

bool Parser::exprListTail()
{
    if (current.pos == "COMMA") {
        current = scanner.nextToken();
        if (expr()) {
            return exprListTail();
        } else {
            throwError("lista no puede terminar en coma");
            return false;
        }
    }
    else if (current.pos == "CLO_BRA")
        return true;
    return false;
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

bool Parser::parse()
{
    current = scanner.nextToken();
    if (expr() && current.pos == "NEWLINE")
        return true;  
    std::cout << "Error de sintaxis" << std::endl;
    return false;
}