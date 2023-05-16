#ifndef PARSER_HPP
#define PARSER_HPP

#include "Scanner.hpp"

class Parser
{
    Scanner scanner;
    Token   current;
    
    void throwError(std::string msg);

    bool binOp();
public:
    Parser(std::string filepath, bool d_mode);

    bool parse();
};

#endif // PARSER_HPP