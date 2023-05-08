#include "Scanner.hpp"

Scanner::Scanner(std::string filepath)
{
    input_file.open(filepath);
    line_pos = 0;
}

Scanner::~Scanner()
{
    input_file.close();
}

bool Scanner::nextLine()
{
    line_pos = 0;
    return (bool)std::getline(input_file, current_line);
}

inline char Scanner::currChar()
{
    return current_line[line_pos];
}

inline char Scanner::peakChar()
{
    return current_line[line_pos + 1];
}

inline char Scanner::nextChar()
{
    return current_line[++line_pos];
}

Token Scanner::nextToken()
{
    if (nextLine())
        return Token("LINE", current_line);
    else
        return Token("EOF","EOF");
}