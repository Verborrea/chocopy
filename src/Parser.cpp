#include "Parser.hpp"

Parser::Parser(std::string path, bool d_mode)
{
    scanner.setFile(path);
    scanner.setDebugMode(d_mode);
}

void Parser::addError(std::string msg)
{ 
    std::string row = std::to_string(scanner.token_row);
    std::string col = std::to_string(scanner.token_col);
    errors.push_back(msg + " en: (" + row + ":" + col + ")");
}

void Parser::throwError(std::string error)
{
    std::cerr << "\x1B[31mError: " << error << "\033[0m" << std::endl;
}

void Parser::goThrough(std::vector<std::string> *f)
{
    while (std::find(f->begin(), f->end(), current.pos) == f->end()) {
        current = scanner.nextToken();
    }
}

Node* Parser::literal()
{
    Node* lit = nullptr;

    if (current.lex == "True" || current.lex == "None" || current.lex == "False") {
        lit = new Node(current.lex);
        current = scanner.nextToken();
    } else if (current.pos == "INTEGER" || current.pos == "STRING") {
        lit = new Node(current.lex);
        current = scanner.nextToken();
    }

    return lit;
}

Node* Parser::exprListTail(Node* parent)
{
    std::vector<std::string> error_follow = {"CLO_PAR","CLO_BRA","NEWLINE"};

    // ExprList -> , Expr ExprListTail
    if (current.pos == "COMMA") {
        current = scanner.nextToken();
        Node* expr_node;
        if ((expr_node = expr())) {
            parent->insert(expr_node);
            return exprListTail(parent);
        }
        addError("Token inesperado: " + current.lex + " despues de ,");
        goThrough(&error_follow);
        return new Node("error");
    }

    // ExprList -> e
    if (current.pos == "CLO_BRA" || current.pos == "CLO_PAR")
        return parent;
    
    // error
    addError("Token inesperado: " + current.lex);
    goThrough(&error_follow);
    return new Node("error");
}

Node* Parser::exprList(std::string name)
{
    // ExprList -> Expr ExprListTail
    Node* exprList_node = new Node(name);

    Node* expr_node;
    if ((expr_node = expr())) {
        exprList_node->insert(expr_node);
        // los insert se hacen dentro de exprListTail
        Node* exprListTail_node = exprListTail(exprList_node);
        if (exprListTail_node->data == "error") {
            exprList_node->podate(exprList_node);
        }
        return exprListTail_node;
    }

    // ExprList -> e
    if (current.pos == "CLO_BRA" || current.pos == "CLO_PAR")
        return exprList_node;

    // error
    std::vector<std::string> follow = {"NEWLINE","CLO_PAR","CLO_BRA","COMMA"};
    addError("Token inesperado: " + current.lex + ". Dentro de lista");
    goThrough(&follow);
    return new Node("error");
}

Node* Parser::list()
{
    // List -> [ ExprList ]
    if (current.lex == "[") {
        current = scanner.nextToken();
        Node* exprList_node = exprList("LIST");

        std::vector<std::string> follow = {"NEWLINE","CLO_PAR","CLO_BRA","COMMA"};
        if (current.lex != "]") {
            exprList_node->podate(exprList_node);
            addError("Token inesperado: " + current.lex + ". Se esperaba ]");
            goThrough(&follow);
            return new Node("error");
        }

        current = scanner.nextToken();
        return exprList_node;
    }

    return nullptr;
}

Node* Parser::nameTail()
{
    Node *nameTail_node = nullptr;

    std::vector<std::string> follow = {"NEWLINE","CLO_PAR","CLO_BRA","COMMA"};

    // NameTail -> ( ExprList )
    if (current.lex == "(") {
        current = scanner.nextToken();
        Node* exprList_node = exprList("ARGS");

        if (current.lex != ")") {
            exprList_node->podate(exprList_node);
            addError("Token inesperado: " + current.lex + ". Se esperaba )");
            goThrough(&follow);
            return new Node("error");
        }

        current = scanner.nextToken();
        return exprList_node;
    }

    // NameTail -> List
    if ((nameTail_node = list())) {
        return nameTail_node;
    }

    // NameTail -> e
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return nameTail_node;
    }
    
    // error
    addError("Error al final de name");
    goThrough(&follow);
    return new Node("error");
}

Node* Parser::name()
{
    // Name -> ID NameTail
    if (current.pos != "IDNTF")
        return nullptr;
    
    Node* name_node = new Node("NAME");
    name_node->insert(current.lex);
    current = scanner.nextToken();

    Node* nameTail_node;
    if ((nameTail_node = nameTail()))
        name_node->insert(nameTail_node);
    return name_node;
}

Node* Parser::factor()
{
    Node *factor_node = nullptr;

    // Factor -> -Factor
    if (current.lex == "-") {
        current = scanner.nextToken();
        Node* neg_node = new Node("-");
        neg_node->insert(factor());
        return neg_node;
    }

    // Factor -> Name
    if ((factor_node = name())) {
        return factor_node;
    }
    
    // Factor -> Literal
    if ((factor_node = literal())) {
        return factor_node;
    }

    // Factor -> List
    if ((factor_node = list())) {
        return factor_node;
    }

    std::vector<std::string> follow = {"NEWLINE","CLO_PAR","CLO_BRA","COMMA"};

    // Factor -> ( Expr )
    if (current.lex == "(") {
        current = scanner.nextToken();
        Node* expr_node = expr();
        if (current.lex != ")") {
            expr_node->podate(expr_node);
            addError("Token inesperado: " + current.lex + ". Se esperaba )");
            goThrough(&follow);
            return new Node("error");
        }
        current = scanner.nextToken();
        return expr_node;
    }
    return factor_node;
}

Node* Parser::expr()
{
    return factor();
}

Node* Parser::assing()
{
    if (current.pos != "IDNTF")
        return new Node("error");
    current = scanner.nextToken();

    if (current.pos != "ASSIGN")
        return new Node("error");
    current = scanner.nextToken();

    Node* expr_node = expr();  // "a + b"

    Node* assign_node = new Node(":=");
    assign_node->insert("ID");
    assign_node->insert(expr_node);

    return assign_node;
}

Node* Parser::program()
{
    Node* program_node = new Node("PROGRAM");
    program_node->insert(factor());
    return program_node;
}

bool Parser::parse()
{
    current = scanner.nextToken();
    ast.root = program();
    if (current.pos != "NEWLINE")
        addError("Token inesperado: " + current.lex + " al final");
    if (errors.size()) {
        for (size_t i = 0; i < errors.size(); i++)
            throwError(errors[i]);

        std::cerr << "\x1B[31mLa compilación finalizó con errores :(\033[0m"
            << std::endl;
        return false;
    }
    ast.printTree(ast.root);
    return true;
}