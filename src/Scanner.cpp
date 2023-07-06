#include "Scanner.hpp"

std::vector<std::string> keywords = {
    "False", "None", "True", "and", "as", "assert",
    "async", "await", "break", "class", "continue",
    "def", "del", "elif", "else", "except", "finally",
    "for", "from", "global", "if", "import", "in", "is",
    "lambda", "nonlocal", "not", "or", "pass", "raise",
    "return", "try", "while", "with", "yield","int","str"
};

Scanner::Scanner()
{
    line_count = 0;
    token_row = 0;
    token_col = 0;
    tab_size = 0;
    indents.push(0);
    indent_level = 0;

    // set flags
    debug = false;
    is_logic = false;
    eof = false;
    validating = false;
    is_first = false;
}

Scanner::~Scanner()
{
    input_file.close();
    indents.pop();
}

void Scanner::setFile(std::string path)
{
    input_file.open(path);
    nextLine();
}

void Scanner::setDebugMode(bool mode)
{
    debug = mode;
}

void Scanner::throwError(std::string msg)
{
    if (!validating)
        std::cerr << "\x1B[31mError: " << msg << ". found at: (" <<
        token_row << ":" << token_col << ")\033[0m" << std::endl;
}

void Scanner::nextLine()
{
    line_pos = 0;
    line_count++;
    if (!std::getline(input_file, current_line))
        eof = true;
}

bool Scanner::validateLogic()
{
    validating = true;
    bool d_mode = debug;
    debug = false;
    int backup = line_pos;
    bool result = true;

    if (currChar() == NEWLINE)
        result = false;
    else if (nextToken().pos == "")
        result = false;

    validating = false;
    debug = d_mode;
    line_pos = backup;
    return result;
}

void Scanner::getIndentLevel()
{
    indent_level = indents.top();
    size_t space_counter = 0;
    for (line_pos = 0; currChar() == 32; space_counter++)
        moveChar();

    // calcular el indent_level o establecer el tab_size
    if (validateLogic()) {
        if (tab_size == 0)
            tab_size = space_counter;
        indent_level = space_counter / tab_size;
    // indicar error si el numero de espacios no es multiplo
        if (space_counter % tab_size)
            throwError("Indentacion inadecuada");
    }
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

    if (!validating && is_first) {
        getIndentLevel();
        is_first = false;
    }

    char ch = (char)currChar();

    // whitespace
    while (ch == ' ')
    {
        ch = (char)moveChar();
    }

    token_col = line_pos + 1;
    token_row = line_count;

    if (indent_level != indents.top())
    {
        if (indent_level > indents.top())
        {
            t.set("INDENT","");
            indents.push(indent_level);
        }
        else
        {
            t.set("DEDENT","");
            indents.pop();
        }
    }
    else if (currChar() == EOF)
    {
        t.set("EOF","EOF");
    }
    else if (currChar() == NEWLINE)
    {
        if (is_logic)
            t.set("NEWLINE","\\n");
        moveChar();
        is_logic = false;
        is_first = true;
    }
    else if (currChar() == '#')
    {
        while (moveChar() != NEWLINE);
    }
    else
    {
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
        case '*': t.set("BIN_OP1","*"); moveChar(); break;
        case '%': t.set("BIN_OP1","%"); moveChar(); break;
        case '(': t.set("OPEN_PAR","("); moveChar(); break;
        case ')': t.set("CLO_PAR",")"); moveChar(); break;
        case '[': t.set("OPEN_BRA","["); moveChar(); break;
        case ']': t.set("CLO_BRA","]"); moveChar(); break;
        case ',': t.set("COMMA",","); moveChar(); break;
        case ':': t.set("TWO_DOTS",":"); moveChar(); break;
        case '.': t.set("DOT","."); moveChar(); break;
        case '/':
            if (moveChar() == '/') {
                t.set("BIN_OP1","//"); moveChar();
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
                t.set("ASSIGN","=");
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
            throwError(std::string(1,ch) + " no reconocido");
            moveChar();
        }
    }

    if (t.pos == "") {
        if (validating)
            return Token("","");
        return nextToken();
    }
    else {
        is_logic = (t.pos != "NEWLINE");
        if (debug)
            std::cout << t.pos << "\t[" << t.lex << "]\t found at (" <<
            token_row << ":" << token_col << ")" << std::endl;
    }

    return t;
}