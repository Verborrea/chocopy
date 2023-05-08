#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"

#include <fstream>

class Scanner
{
private:
    std::fstream input_file;
    std::string current_line;
    size_t line_pos;

    bool nextLine();

    char currChar();
    char peakChar();
    char nextChar();

public:
    Scanner(std::string filepath);
    ~Scanner();

    Token nextToken();
};

#endif // SCANNER_HPP