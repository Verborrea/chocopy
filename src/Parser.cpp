#include "Parser.hpp"

Parser::Parser(std::string path, bool d_mode)
{
    scanner.setFile(path);
    scanner.setDebugMode(d_mode);
}

void Parser::addError(std::string msg)
{
    errors.push_back(msg);
}

void Parser::throwError(std::string msg)
{
    std::cerr << "\x1B[31mError: " << msg << " en: (" <<
    scanner.token_row << ":" << scanner.token_col << ")\033[0m" << std::endl;
}

Node* Parser::expr()
{
    if (current.pos != "IDNTF")
        return new Node("error");
    current = scanner.nextToken();

    if (current.pos != "BIN_OP")
        return new Node("error");
    std::string bin_op = current.lex;   // guardar la operacion
    current = scanner.nextToken();

    if (current.pos != "IDNTF")
        return new Node("error");
    current = scanner.nextToken();

    Node* expr_node = new Node(bin_op);
    expr_node->insert("ID");
    expr_node->insert("ID");

    return expr_node;
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
    return assing();
}

bool Parser::parse()
{
    current = scanner.nextToken();
    ast.root = program();
    if (current.pos != "NEWLINE")
        addError("Token " + current.pos + " inesperado");
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