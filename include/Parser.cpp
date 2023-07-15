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
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA"
    };
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

    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP1","BIN_OP2","BIN_OP3","BIN_OP5","BIN_OP6","if","else"
    };

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

    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP1","BIN_OP2","BIN_OP3","BIN_OP5","BIN_OP6","if","else"
    };

    // Factor -> ( Expr )
    if (current.lex != "(") {
        addError("Token inesperado: " + current.lex + ". Se esperaba una expresion");
        goThrough(&follow);
        return new Node("error");
    }

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

// ============================================================================

Node* Parser::termPrime(Node* first)
{
    // Term' -> *  Factor Term'
    // Term' -> // Factor Term'
    // Term' -> %  Factor Term'
    if (current.lex == "*" || current.lex == "//" || current.lex == "%") {
        Node* op = new Node(current.lex);
        op->insert(first);
        current = scanner.nextToken();
        Node* second = factor();
        op->insert(second);
        return termPrime(op);
    }

    // Term' -> e
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP2","BIN_OP3","BIN_OP5","BIN_OP6","if","else"
    };
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::term()
{
    // Term -> Factor TermPrime
    Node *factor_node = factor();
    Node *term_prime_node = termPrime(factor_node);
    return term_prime_node;
}

Node* Parser::intExprPrime(Node* first)
{
    // IntExpr' -> + Term IntExpr'
    // IntExpr' -> - Term IntExpr'
    if (current.lex == "+" || current.lex == "-") {
        Node* op = new Node(current.lex);
        op->insert(first);
        current = scanner.nextToken();
        Node* second = term();
        op->insert(second);
        return intExprPrime(op);
    }

    // IntExpr' -> e
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP3","BIN_OP5","BIN_OP6","if","else"
    };
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::intExpr()
{
    // IntExpr -> Term IntExprPrime
    Node *term_node = term();
    Node *iexpr_prime_node = intExprPrime(term_node);
    return iexpr_prime_node;
}

Node* Parser::compExprPrime(Node* first)
{
    // CompExpr' -> (< | > | <= | >= | != | == | is) IntExpr CompExpr'
    if (current.pos == "BIN_OP3") {
        Node* op = new Node(current.lex);
        op->insert(first);
        current = scanner.nextToken();
        Node* second = intExpr();
        op->insert(second);
        return compExprPrime(op);
    }

    // CompExpr' -> e
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP5","BIN_OP6","if","else"
    };
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::compExpr()
{
    // CompExpr -> IntExpr CompExpr'
    Node *iexpr_node = intExpr();
    Node *cexpr_prime_node = compExprPrime(iexpr_node);
    return cexpr_prime_node;
}

Node* Parser::notExpr()
{
    // notExpr -> -notExpr
    if (current.lex == "not") {
        current = scanner.nextToken();
        Node* not_node = new Node("not");
        not_node->insert(notExpr());
        return not_node;
    }

    // notExpr -> CompExpr
    return compExpr();
}

Node* Parser::andExprPrime(Node* first)
{
    // andExpr' -> and notExpr andExpr'
    if (current.pos == "BIN_OP5") {
        Node* op = new Node(current.lex);
        op->insert(first);
        current = scanner.nextToken();
        Node* second = notExpr();
        op->insert(second);
        return andExprPrime(op);
    }

    // andExpr' -> e
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA",
        "BIN_OP6","if","else"
    };
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::andExpr()
{
    // andExpr -> notExpr andExpr'
    Node *nexpr_node = notExpr();
    Node *aexpr_prime_node = andExprPrime(nexpr_node);
    return aexpr_prime_node;
}

Node* Parser::orExprPrime(Node* first)
{
    // orExpr' -> or andExpr orExpr'
    if (current.pos == "BIN_OP6") {
        Node* op = new Node(current.lex);
        op->insert(first);
        current = scanner.nextToken();
        Node* second = andExpr();
        op->insert(second);
        return orExprPrime(op);
    }

    // orExpr' -> e
    std::vector<std::string> follow = {
        "NEWLINE","CLO_PAR","CLO_BRA","COMMA","if","else"
    };
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::orExpr()
{
    // orExpr -> andExpr orExpr'
    Node *andexpr_node = andExpr();
    Node *orexpr_prime_node = orExprPrime(andexpr_node);
    return orexpr_prime_node;
}

Node* Parser::exprPrime(Node* first)
{
    // Expr' -> if orExpr else orExpr Expr'
    if (current.pos == "if") {
        Node* op = new Node("if-else");
        op->insert(first);
        current = scanner.nextToken();
        Node* second = orExpr();
        op->insert(second);

        if (current.pos == "else") {
            current = scanner.nextToken();
            Node* third = orExpr();
            std::cout << third->data << std::endl;
            op->insert(third);
        }

        return exprPrime(op);
    }

    // Expr' -> e
    std::vector<std::string> follow = {"NEWLINE","CLO_PAR","CLO_BRA","COMMA"};
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return first;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    first->podate(first);
    return new Node("error");
}

Node* Parser::expr()
{
    // Expr -> orExpr orExpr'
    Node *orexpr_node = orExpr();
    Node *expr_prime_node = exprPrime(orexpr_node);
    return expr_prime_node;
}

// ============================================================================

Node* Parser::returnExpr()
{
    Node *return_node = nullptr;

    // ReturnExpr -> e
    std::vector<std::string> follow = {"NEWLINE"};
    if (std::find(follow.begin(), follow.end(), current.pos) != follow.end()) {
        return nullptr;
    }

    // ReturnExpr -> Expr
    if ((return_node = expr())) {
        return return_node;
    }

    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    return_node->podate(return_node);
    return new Node("error");
}

Node* Parser::simpleStatement()
{
    // SimpleStatement -> pass
    if (current.pos == "pass") {
        current = scanner.nextToken();
        return new Node("pass");
    }

    // SimpleStatement -> return ReturnExpr
    if (current.pos == "return") {
        current = scanner.nextToken();
        Node* return_node = new Node("return");
        Node* value = returnExpr();
        if (value)
            return_node->insert(value);
        return return_node;
    }

    std::vector<std::string> follow = {"NEWLINE"};
    
    addError("Token inesperado: " + current.lex);
    goThrough(&follow);
    return new Node("error");
}

Node* Parser::program()
{
    Node* program_node = new Node("PROGRAM");
    program_node->insert(simpleStatement());
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