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

void Scanner::throwError(std::string msg)
{
    std::cerr << "Error: " << msg << std::endl;
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

        // operadores y delimitadores
        switch (ch)
        {
        case '+': t.set("BIN_OP","+"); moveChar(); break;
        case '*': t.set("BIN_OP","*"); moveChar(); break;
        case '%': t.set("BIN_OP","%"); moveChar(); break;
        case '(': t.set("OPEN_PAR","("); moveChar(); break;
        case ')': t.set("CLO_PAR",")"); moveChar(); break;
        case '[': t.set("OPEN_BRA","["); moveChar(); break;
        case ']': t.set("CLO_BRA","]"); moveChar(); break;
        case ',': t.set("COMMA",","); moveChar(); break;
        case ':': t.set("TWO_DOTS",":"); moveChar(); break;
        case '.': t.set("DOT","."); moveChar(); break;
        case '/':
            if (moveChar() == '/') {
                t.set("BIN_OP","//"); moveChar();
            }
            else
                throwError("/ no es un operador válido.");
            break;
        case '!':
            if (moveChar() == '=') {
                t.set("BIN_OP","!="); moveChar();
            }
            else
                throwError("! no es un operador válido.");
            break;
        case '-':
            if (moveChar() == '>') {
                t.set("BIN_OP","->"); moveChar();
            }
            else
                t.set("ASSING","-");
            break;
        case '=':
            if (moveChar() == '=') {
                t.set("BIN_OP","=="); moveChar();
            }
            else
                t.set("ASSING","=");
            break;
        case '<':
            if (moveChar() == '=') {
                t.set("BIN_OP","<="); moveChar();
            }
            else
                t.set("BIN_OP","<");
            break;
        case '>':
            if (moveChar() == '=') {
                t.set("BIN_OP",">="); moveChar();
            }
            else
                t.set("BIN_OP",">");
            break;
        default:
            t.set("CHAR",std::string(1, ch));
            moveChar();
        }
    }

    if (debug)
    {
        std::cout << t.pos << "\t[" << t.lex << "]" << std::endl;
    }

    return t;
}