#include "Token.hpp"

#include <iostream>

int main()
{
    Token T("hola","mundo");
    std::cout << T.pos << " " << T.lex << std::endl;
    return 0;
}