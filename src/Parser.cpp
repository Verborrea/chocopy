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

bool Parser::program()
{
    return defList() && statementList();
}

bool Parser::defList()
{
    if (def() && defList())
        return true;
    // comprobar follow
    std::string follow[] = {"if","while","for","pass","return","not",
    "-","(","None","True","False","["};
    if (current.pos == "INTEGER" || current.pos == "STRING" ||
        current.pos == "IDNTF" || current.pos == "EOF")
        return true;
    for (int i = 0; i < 12; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::def()
{
    if (current.lex == "def") {
        current = scanner.nextToken();
        if (current.pos == "IDNTF") {
            current = scanner.nextToken();
            if (current.pos == "OPEN_PAR") {
                current = scanner.nextToken();
                if (typedVarList()) {
                    if (current.pos == "CLO_PAR") {
                        current = scanner.nextToken();
                        if (return_()) {
                            if (current.pos == "TWO_DOTS") {
                                current = scanner.nextToken();
                                if (block())
                                    return true;
                                throwError("expected block");
                            }
                        }
                        throwError("expected :");
                    }
                }
                throwError("expected )");
            }
            throwError("expected (");
        }
        throwError("expected ID");
    }
    return false;
}

bool Parser::typedVar()
{
    if (current.pos == "IDNTF") {
        current = scanner.nextToken();
        if (current.pos == "TWO_DOTS") {
            current = scanner.nextToken();
            if (type())
                return true;
            throwError("expected type");
        }
        throwError("expected : after ID");
    }
    return false;
}

bool Parser::type()
{
    if (current.lex == "int"|| current.lex == "str") {
        current = scanner.nextToken();
        return true;
    }
    if (current.pos == "OPEN_BRA") {
        current = scanner.nextToken();
        if (type()) {
            if (current.pos == "CLO_PAR") {
                current = scanner.nextToken();
                return true;
            }
        }
        throwError("expected [");
    }
    return false;
}

bool Parser::typedVarList()
{
    if (typedVar()) {
        if (typedVarListTail())
            return true;
        throwError("expected typedVarListTail");
    }
    if (current.pos == "CLO_PAR")
        return true;
    return false;
}

bool Parser::typedVarListTail()
{
    if (current.pos == "COMMA") {
        current = scanner.nextToken();
        if (typedVar()) {
            if (typedVarListTail())
                return true;
            throwError("expected typedVarListTail");
        }
    }
    if (current.pos == "CLO_PAR")
        return true;
    return false;
}

bool Parser::return_()
{
    if (current.pos == "ARROW") {
        current = scanner.nextToken();
        if (type())
            return true;
        throwError("expected type");
    }
    // comprobar follow
    if (current.pos == "TWO_DOTS")
        return true;

    return false;
}

bool Parser::block()
{
    if (current.pos == "NEWLINE") {
        current = scanner.nextToken();
        if (current.pos == "INDENT") {
            current = scanner.nextToken();
            if (statement() && statementList()) {
                if (current.pos == "DEDENT") {
                    current = scanner.nextToken();
                    return true;
                }
                throwError("expected DEDENT");
            }
            throwError("expected statements");
        }
        throwError("expected INDENT");
    }
    throwError("expected NEWLINE");
    return false;
}

bool Parser::statementList()
{
    if (statement() && statementList())
        return true;
    // comprobar follow
    if (current.pos == "EOF" || current.pos == "DEDENT")
        return true;
    return false;
}

bool Parser::statement()
{
    if (current.lex == "if") {
        current = scanner.nextToken();
        if (expr()) {
            if (current.pos == "TWO_DOTS") {
                current = scanner.nextToken();
                if (block() && elifList() && else_())
                    return true;
                throwError("expected block,elif,else after expr:");
            }
            throwError("expected : after expr");
        }
        throwError("expected expr after if");
    }
    if (current.lex == "while") {
        current = scanner.nextToken();
        if (expr()) {
            if (current.pos == "TWO_DOTS") {
                current = scanner.nextToken();
                if (block())
                    return true;
                throwError("expected block after expr:");
            }
            throwError("expected : after expr");
        }
        throwError("expected expr after while");
    }
    if (current.lex == "for") {
        current = scanner.nextToken();
        if (current.pos == "IDNTF") {
            current = scanner.nextToken();
            if (current.lex == "in") {
                current = scanner.nextToken();
                if (expr()) {
                    if (current.pos == "TWO_DOTS") {
                        current = scanner.nextToken();
                        if (block())
                            return true;
                        throwError("expected block after expr:");
                    }
                    throwError("expected : after expr");
                }
                throwError("expected expr");
            }
            throwError("expected 'in' at for");
        }
        throwError("expected <id> after for");
    }
    if (simpleStatement()) {
        if (current.pos == "NEWLINE") {
            current = scanner.nextToken();
            return true;
        }
    }
    return false;
}

bool Parser::elifList()
{
    if (elif() && elifList())
        return true;
    // comprobar follow
    std::string follow[] = {"if","while","for","pass","return","not",
    "-","(","None","True","False","[","else"};
    if (current.pos == "INTEGER" || current.pos == "STRING" ||
        current.pos == "IDNTF")
        return true;
    for (int i = 0; i < 13; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::elif()
{
    if (current.lex == "elif") {
        current = scanner.nextToken();
        if (expr()) {
            if (current.pos == "TWO_DOTS") {
                current = scanner.nextToken();
                if (block())
                    return true;
                throwError("expected block after : in elif");
            }
            throwError("expected : after expr in elif");
        }
        throwError("expr expected after elif");
    }
    return false;
}

bool Parser::else_()
{
    if (current.lex == "else") {
        current = scanner.nextToken();
        if (current.pos == "TWO_DOTS") {
            current = scanner.nextToken();
            if (block())
                return true;
            throwError("expected block after else");
        }
        throwError(": expected after else");
    }
    // comprobar follow
    std::string follow[] = {"if","while","for","pass","return","not",
    "-","(","None","True","False","["};
    if (current.pos == "INTEGER" || current.pos == "STRING" ||
        current.pos == "IDNTF")
        return true;
    for (int i = 0; i < 12; i++)
        if (current.lex == follow[i])
            return true;
    return false;
}

bool Parser::simpleStatement()
{
    if (expr() && sstail())
        return true;
    if (current.lex == "pass") {
        current = scanner.nextToken();
        return true;
    }
    if (current.lex == "return") {
        current = scanner.nextToken();
        return returnExpr();
    }
    return false;
}

bool Parser::sstail()
{
    if (current.pos == "ASSING") {
        current = scanner.nextToken();
        if (expr())
            return true;
        throwError("expr expected after =");
        return false;
    }
    // comprobar follow
    if (current.pos == "NEWLINE")
        return true;
    return false;
}

bool Parser::returnExpr()
{
    if (expr()) {
        return true;
    }
    // comprobar follow
    if (current.pos == "NEWLINE")
        return true;
    return false;
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
    if (current.pos == "NEWLINE"||current.lex == "="||current.lex == ":"
     || current.lex == ")" || current.lex == "]" || current.lex == ",")
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
        throwError("expected expr after or");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else",","};
    if (current.pos == "NEWLINE"||current.lex == "="||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 3; i++)
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
        throwError("expected expr after and");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else","or",","};
    if (current.pos == "NEWLINE"||current.lex == "="||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 4; i++)
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
        throwError("expected expr after compOp");
        return false;
    }
    // comprobar follow
    std::string follow[] = {"if","else","or","and",","};
    if (current.pos == "NEWLINE"||current.lex == "="||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 5; i++)
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
        throwError("expected expr after +/-");
        return false;
    }
    // comprobar follow
    std::string follow[] = {
        "if","else","or","and","==","!=",">=","<=","<",">","is",","
    };
    if (current.pos == "NEWLINE"||current.lex == "="||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 12; i++)
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
        throwError("expected expr after operation");
        return false;
    }
    // comprobar follow
    std::string follow[] = {
        "if","else","or","and","==","!=",">=","<=","<",">","is","+","-",","
    };
    if (current.pos == "NEWLINE" || current.lex == "=" ||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 14; i++)
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
    // throwError("expected factor");
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
        "*","//","%",","
    };
    if (current.pos == "NEWLINE"||current.lex == "="||
        current.lex == ":" || current.lex == ")" || current.lex == "]")
        return true;
    for (int i = 0; i < 17; i++)
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
        if (expr() && exprListTail())
            return true;
        throwError("lista no puede terminar en coma");
        return false;
    }
    else if (current.pos == "CLO_BRA" || current.pos == "CLO_PAR")
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
    if (program() && current.pos == "EOF")
        return true;  
    std::cout << "Error de sintaxis" << std::endl;
    return false;
}