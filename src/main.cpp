#include "Scanner.hpp"

#include <iostream>

int main(int argc, char *argv[])
{
    if (argc != 2){
        std::cout << "usage: ./choco.exe <input_file>" << std::endl;
        return 1;
    }
    std::string input_file = argv[1];
    Scanner S(input_file);

    Token t = S.nextToken();
    while (t.lex != "EOF")
    {
        std::cout << t.lex << std::endl;
        t = S.nextToken();
    } 

    return 0;
}