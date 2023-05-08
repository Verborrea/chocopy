#include "Scanner.hpp"

Scanner::Scanner(std::string filepath)
{
    input_file.open(filepath);
}

Scanner::~Scanner()
{
    input_file.close();
}

Token Scanner::nextToken()
{
    if (std::getline(input_file, current_line))
        return Token("LINE", current_line);
    else
        return Token("EOF","EOF");
}