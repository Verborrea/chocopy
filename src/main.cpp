#include "Scanner.hpp"

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3) {
        std::cout << "usage: ./choco.exe <input_file> [-d]" << std::endl;
        return 1;
    }

    std::string input_file = argv[1];
    bool debug = false;

    if (argc == 3) {
        if (argv[2] != "-d") {
            std::cout << "error: " << argv[1] << "is not a valid flag"
                << std::endl;
            return 1;
        }
        debug = true;
    }

    Scanner S(input_file, debug);

    Token t = S.nextToken();
    while (t.pos != "EOF")
    {
        t = S.nextToken();
    } 

    return 0;
}