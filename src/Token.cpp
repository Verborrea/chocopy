#include "Token.hpp"

Token::Token()
{
    pos = "";
    lex = "";
};

Token::Token(std::string _pos, std::string _lex)
{
    pos = _pos;
    lex = _lex;
};

void Token::set(std::string _pos, std::string _lex)
{
    pos = _pos;
    lex = _lex;
};