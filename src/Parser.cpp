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
    if (current.pos == "BIN_OP" || current.lex == "IS")
        return true;
    throwError(current.lex + " is not a valid BIN_OP");
    return false;
}

bool Parser::parse()
{
    current = scanner.nextToken();
    if (binOp() && current.pos == "EOF")
        return true;  
    std::cout << "Error de sintaxis" << std::endl;
}