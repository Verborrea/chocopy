#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"

#include <fstream>
#include <iostream>
#include <vector>

class Scanner
{
private:
    std::fstream input_file;
    std::string current_line;
    size_t line_pos;
    size_t line_count;
    size_t token_row;
    size_t token_col;

    // flags
    bool debug;     // indica modo debug
    bool eof;       // indica fin de input_file
    bool is_logic;  // indica si la linea actual es logica

    void throwError(std::string msg);
    void nextLine();

    int currChar();
    int moveChar();

public:
    static const int NEWLINE = -2;

    Scanner(std::string filepath, bool d_mode);
    ~Scanner();

    Token nextToken();
};

#endif // SCANNER_HPP