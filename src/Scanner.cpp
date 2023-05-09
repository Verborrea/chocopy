#include "Scanner.hpp"

std::vector<std::string> keywords = {
    "False", "None", "True", "and", "as", "assert",
    "async", "await", "break", "class", "continue",
    "def", "del", "elif", "else", "except", "finally",
    "for", "from", "global", "if", "import", "in", "is",
    "lambda", "nonlocal", "not", "or", "pass", "raise",
    "return", "try", "while", "with", "yield"
};

Scanner::Scanner(std::string filepath, bool d_mode)
{
    input_file.open(filepath);
    nextLine();
    debug = d_mode;
    line_count = 1;
    token_row = 0;
    token_col = 0;
}

Scanner::~Scanner()
{
    input_file.close();
}

void Scanner::throwError(std::string msg)
{
    std::cerr << "Error: " << msg << ". found at: (" <<
    token_row << ":" << token_col << ")" << std::endl;
}

void Scanner::nextLine()
{
    line_pos = 0;
    line_count++;
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

    token_col = line_pos + 1;
    token_row = line_count;

    if (currChar() == EOF)
    {
        t.set("EOF","EOF");
    }
    else if (currChar() == NEWLINE)
    {
        t.set("NEWLINE","");
        moveChar();
    }
    else
    {
        char ch = (char)currChar();

        // ids & keywords
        if (isalpha(currChar()))
        {
            std::string id(1,ch);
            while (isalnum(moveChar()) || currChar() == '_') {
                id += currChar();
            }
            t.set("IDNTF", id);
            for (size_t i = 0; i < keywords.size(); i++)
                if (keywords[i] == id) {
                    t.set("KEYWORD", id);
                    break;
                }
        }

        // enteros
        else if (isdigit(currChar()))
        {
            std::string entero(1,ch);
            while (isdigit(moveChar())) {
                entero += currChar();
            }
            if (entero[0] == '0' && entero.size() > 1)
                throwError("Un entero mayor que 0 no puede empezar con 0");
            else if (std::stol(entero) > 2147483647)
                throwError("Un entero no puede ser mayor que 2147483647");
            else
                t.set("INTEGER",entero);
        }

        // cadenas
        else if (ch == '\"')
        {
            bool not_recognized = false;
            std::string cadena;
            while (moveChar() != '\"') {
                if (currChar() == '\\'){
                    if (moveChar() != '\"') {
                        std::string cad = "\\";
                        cad += currChar();
                        not_recognized = true;
                        token_col = line_pos;
                        throwError(cad + " no reconocido");
                    }
                }
                cadena += currChar();
            }
            if (!not_recognized)
                t.set("STRING",cadena);
            moveChar();
        }

        // operadores y delimitadores
        else switch (ch)
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
                throwError("/ no es un operador válido");
            break;
        case '!':
            if (moveChar() == '=') {
                t.set("BIN_OP","!="); moveChar();
            }
            else
                throwError("! no es un operador válido");
            break;
        case '-':
            if (moveChar() == '>') {
                t.set("ARROW","->"); moveChar();
            }
            else
                t.set("BIN_OP","-");
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
        // default:
        //     t.set("CHAR",std::string(1, ch));
        //     moveChar();
        }
    }

    if (t.pos == "")
        return nextToken();
    else if (debug)
        std::cout << t.pos << "\t[" << t.lex << "]\t found at (" <<
        token_row << ":" << token_col << ")" << std::endl;

    return t;
}