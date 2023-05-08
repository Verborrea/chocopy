#include "Scanner.hpp"

int main(int argc, char *argv[])
{
    if (argc != 2){
        std::cout << "usage: ./choco.exe <input_file>" << std::endl;
        return 1;
    }
    std::string input_file = argv[1];
    Scanner S(input_file, true);

    Token t = S.nextToken();
    while (t.pos != "EOF")
    {
        t = S.nextToken();
    } 

    return 0;
}