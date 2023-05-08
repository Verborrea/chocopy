#include "Scanner.hpp"

Scanner::Scanner(std::string filepath, bool d_mode)
{
    input_file.open(filepath);
    nextLine();
    debug = d_mode;
}

Scanner::~Scanner()
{
    input_file.close();
}

void Scanner::nextLine()
{
    line_pos = 0;
    if (!std::getline(input_file, current_line))
        eof = true;
}

inline int Scanner::currChar()
{
    if (eof)
        return EOF;
    if (line_pos == current_line.size())
        return NEWLINE;
    return current_line[line_pos];
}

inline int Scanner::moveChar()
{
    line_pos++;
    if (line_pos > current_line.size())
        nextLine();
    return currChar();
}

Token Scanner::nextToken()
{
    Token t;
    if (currChar() == EOF)
    {
        t.set("EOF","EOF");
    }
    else if (currChar() == NEWLINE)
    {
        moveChar();
        t.set("NEWLINE","NEWLINE");
    }
    else
    {
        char ch = (char)currChar();
        moveChar();
        t.set("CHAR",std::string(1, ch));
    }

    if (debug)
    {
        std::cout << t.pos << "\t[" << t.lex << "]" << std::endl;
    }

    return t;
}