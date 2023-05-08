#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>

struct Token
{
    std::string pos;
    std::string lex;

    Token(std::string _pos, std::string _lex);
};

#endif // TOKEN_HPP