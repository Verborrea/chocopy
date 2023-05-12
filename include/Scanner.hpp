#ifndef SCANNER_HPP
#define SCANNER_HPP

#include "Token.hpp"

#include <fstream>
#include <iostream>
#include <vector>
#include <stack>

class Scanner
{
private:
    std::fstream input_file;
    std::string current_line;
    std::stack<int> indents;

    size_t line_pos;
    size_t line_count;
    size_t token_row;
    size_t token_col;
    size_t tab_size;
    int    indent_level;

    // flags
    bool debug;      // indica modo debug
    bool eof;        // indica fin de input_file
    bool is_logic;   // indica si la linea actual es logica
    bool validating; // indica que hay una posible indentacion
    bool is_first;

    void throwError(std::string msg);
    void nextLine();
    void getIndentLevel();
    bool validateLogic();

    int currChar();
    int moveChar();

public:
    static const int NEWLINE = -2;

    Scanner(std::string filepath, bool d_mode);
    ~Scanner();

    Token nextToken();
};

#endif // SCANNER_HPP