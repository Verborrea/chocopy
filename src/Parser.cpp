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

bool Parser::binOp()
{
    if (current.pos == "BIN_OP" || current.lex == "IS") {
        current = scanner.nextToken();
        return true;
    }
    return false;
}

bool Parser::literal()
{
    bool is_literal = false;

    is_literal |= current.pos == "INTEGER";
    is_literal |= current.pos == "STRING";
    is_literal |= current.lex == "None";
    is_literal |= current.lex == "True";
    is_literal |= current.lex == "False";

    if (is_literal) {
        current = scanner.nextToken();
        return true;
    }
    throwError(current.lex + " is not a literal");
    return false;
}

bool Parser::cexpr_3()
{
    if (current.lex == "-") {
        current = scanner.nextToken();
        return cexpr_3();
    }
    if (current.pos == "IDNTF") {
        current = scanner.nextToken();
        return true;
    }
    if (literal())
        return true;
    throwError("not valid cexpr3");
    return false;
}

bool Parser::cexpr_2()
{
    if (binOp() && cexpr_3()) {
        return cexpr_2();
    }
    else {
        if (current.pos == "NEWLINE")
            return true;
    }
    throwError("not valid cexpr2");
    return false;
}

bool Parser::cexpr()
{
    if (cexpr_3() && cexpr_2())
        return true;
    throwError("not valid cexpr");
    return false;
}

bool Parser::parse()
{
    current = scanner.nextToken();
    if (cexpr() && current.pos == "NEWLINE")
        return true;  
    std::cout << "Error de sintaxis" << std::endl;
    return false;
}