#ifndef PARSER
#include "parser.h"
#endif

#ifndef IOSTREAM
#include <iostream>
#endif

#define SSTREAM_FLAGS std::ios_base::app | std::ios_base::out

parser::parser(tokenizer_interface& t) : tokens_(t) {
}

internal_node* parser::parse() {
    internal_node* p = new internal_node("program", 1);
    p->set_child(0, declaration_list());
    return p;
}

parse_tree_node* parser::statement() {
    internal_node* p = new internal_node("statement", 1);
    if (tokens_.peek_token().get_str() == "{") {
        p->set_child(0, compound_statement());
    }
    else if (tokens_.peek_token().get_str() == "if") {
        p->set_child(0, if_statement());
    }
    else if (tokens_.peek_token().get_str() == "while") {
        p->set_child(0, while_statement());
    }
    else if (tokens_.peek_token().get_str() == "return") {
        p->set_child(0, return_statement());
    }
    else if (tokens_.peek_token().get_str() == "write" ||
            tokens_.peek_token().get_str() == "writeln") {
        p->set_child(0, write_statement());
    }
    else {
        p->set_child(0, expression_statement());
    }
    return p;
}

parse_tree_node* parser::expression_statement() {
    internal_node* p = new internal_node("expression statement", 1);
    p->set_child(0, expression());
    expect_token_type(";");
    return p;
}

parse_tree_node* parser::compound_statement() {
    expect_token_type("{");
    internal_node* p = new internal_node("compound statement", 2);
    p->set_child(0, local_decs());
    p->set_child(1, statement_list());
    expect_token_type("}");
    return p;
}

parse_tree_node* parser::statement_list() {
    internal_node* p = new internal_node("statement list", 2);
    if (tokens_.peek_token().get_type() != "}") {
        p->set_child(0, statement());
    }
    else {
        p->set_child(0, empty_node());
    }
    if (tokens_.peek_token().get_type() != "}") {
        p->set_child(1, statement_list());
    }
    else {
        p->set_child(1, empty_node());
    }
    return p;
}

parse_tree_node* parser::local_decs() {
    internal_node* p = new internal_node("local decs", 2);
    string types[4] = {"int", "void", "float", "string"};
    bool found = false;
    for (int i = 0; i < 4; ++i) {
        if (tokens_.peek_token().get_str() == types[i]) {
            found = true;
        }
    }
    if (found) {
        p->set_child(0, var_dec());
    }
    else {
        p->set_child(0, empty_node());
    }
    found = false;
    for (int i = 0; i < 4; ++i) {
        if (tokens_.peek_token().get_str() == types[i]) {
            found = true;
        }
    }
    if (found) {
        p->set_child(1, local_decs());
    }
    else {
        p->set_child(1, empty_node());
    }
    return p;
}

parse_tree_node* parser::if_statement() {
    internal_node* p = new internal_node("if statement", 3);
    expect_keyword("if");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
    if (tokens_.peek_token().get_str() == "else") {
        expect_keyword("else");
        internal_node* else_node = new internal_node("else statement", 1);
        else_node->set_child(0, statement());
        p->set_child(2, else_node);
    }
    else {
        p->set_child(2, empty_node());
    }
    return p;
}

parse_tree_node* parser::while_statement() {
    internal_node* p = new internal_node("while statement", 2);
    expect_keyword("while");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
    return p;
}

parse_tree_node* parser::return_statement() {
    internal_node* p = new internal_node("return statement", 2);
    expect_keyword("return");
    if (tokens_.peek_token().get_type() == ";") {
        expect_token_type(";");
        p->set_child(0, empty_node());
    }
    else {
        p->set_child(0, expression());
    }
    return p;
}

parse_tree_node* parser::write_statement() {
    internal_node* p = new internal_node("write statement", 1);
    if (tokens_.peek_token().get_str() == "write") {
        expect_keyword("write");
        expect_token_type("(");
        p->set_child(0, expression());
        expect_token_type(")");
    }
    else {
        expect_keyword("writeln");
        expect_token_type("(");
        p->set_child(0, empty_node());
        expect_token_type(")");
    }
    return p;
}

parse_tree_node* parser::function_dec() {
    internal_node* p = new internal_node("function_dec", 4);
    p->set_child(0, type_specifier());
    p->set_child(1, identifier());
    expect_token_type("(");
    p->set_child(2, params());
    expect_token_type(")");
    p->set_child(3, compound_statement());
    return p;
}

parse_tree_node* parser::params() {
    internal_node* p = new internal_node("params", 1);
    if (tokens_.peek_token().get_str() == "void") {
        expect_keyword("void");
        internal_node* v = new internal_node("void", 0);
        p->set_child(0, v);
    }
    else {
        p->set_child(0, param_list());
    }
    return p;
}

parse_tree_node* parser::param_list() {
    internal_node* p = new internal_node("param list", 2);
    p->set_child(0, param());
    if (tokens_.peek_token().get_type() == ",") {
        expect_token_type(",");
        p->set_child(1, param_list());
    }
    else {
        p->set_child(1, empty_node());
    }
    return p;
}

parse_tree_node* parser::declaration_list() {
    internal_node* p = new internal_node("declaration list", 2);
    if (tokens_.has_next_token()) {
        p->set_child(0, declaration());
    }
    if (tokens_.has_next_token()) {
        p->set_child(1, declaration_list());
    }
    else {
        p->set_child(1, empty_node());
    }
    return p;
}

parse_tree_node* parser::declaration() {
    internal_node* p = new internal_node("declaration", 1);
    if (!tokens_.has_next_token()) {
        throw std::range_error("Unexpected EOF");
    }
    token type = tokens_.next_token();
    if (!tokens_.has_next_token()) {
        throw std::range_error("Unexpected EOF");
    }
    if (tokens_.peek_token().get_type() == "*") {
        tokens_.unget_token(type);
        p->set_child(0, var_dec());
    }
    else {
        token id = tokens_.next_token();
        if (tokens_.peek_token().get_type() == "(") {
            tokens_.unget_token(id);
            tokens_.unget_token(type);
            p->set_child(0, function_dec());
        }
        else {
            tokens_.unget_token(id);
            tokens_.unget_token(type);
            p->set_child(0, var_dec());
        }
    }
    return p;
}

parse_tree_node* parser::param() {
    internal_node* p = new internal_node("param", 4);
    p->set_child(0, type_specifier());
    if (tokens_.peek_token().get_type() == "*") {
        internal_node* star = new internal_node("*", 0);
        p->set_child(1, star);
        expect_token_type("*");
    }
    else {
        p->set_child(1, empty_node());
    }
    p->set_child(2, identifier());
    if (tokens_.peek_token().get_type() == "[") {
        internal_node* brackets = new internal_node("[]", 0);
        p->set_child(3, brackets);
        expect_token_type("[");
        expect_token_type("]");
    }
    else {
        p->set_child(3, empty_node());
    }
    return p;
}

parse_tree_node* parser::var_dec() {
    internal_node* p = new internal_node("var_dec", 4);
    p->set_child(0, type_specifier());
    if (tokens_.peek_token().get_type() == "*") {
        internal_node* star = new internal_node("*", 0);
        p->set_child(1, star);
        expect_token_type("*");
    }
    else {
        p->set_child(1, empty_node());
    }
    p->set_child(2, identifier());
    if (tokens_.peek_token().get_type() == "[") {
        expect_token_type("[");
        token t = expect_token_type("int");
        internal_node* brackets = new internal_node("[]", 1);
        int_node* size = new int_node(std::stoi(t.get_str()));
        brackets->set_child(0, size);
        p->set_child(3, brackets);
        expect_token_type("]");
    }
    else {
        p->set_child(3, empty_node());
    }
    expect_token_type(";");
    return p;
}

parse_tree_node* parser::type_specifier() {
    string allowed_types[4] = {"int", "float", "string", "void"};
    token t = tokens_.peek_token();
    string type = t.get_str();
    bool found = false;
    for (int i = 0; i < 4; ++i) {
        if (type == allowed_types[i]) {
            found = true;
        }
    }
    if (!found) {
        std::stringstream messageStream("invalid type on line ", SSTREAM_FLAGS);
        messageStream << t.get_line_num() << ": ";
        messageStream << type;
        throw std::range_error(messageStream.str());
    }
    expect_keyword(type);
    type_node* n = new type_node(type, t.get_line_num());
    return n;
}

parse_tree_node* parser::expression() {
    internal_node* p = new internal_node("expression", 2);
    std::vector<token> token_list;
    std::vector<parse_tree_node*> node_list;
    token t = tokens_.peek_token();
    bool assignment = false;
    while (t.get_type() != ";" && t.get_type() != ")" && t.get_type() != "]") {
        if (tokens_.peek_token().get_type() == "(") {
            token_list.push_back(expect_token_type("("));
            node_list.push_back(expression());
            token placeholder("placeholder", "", 0);
            token_list.push_back(placeholder);
            token_list.push_back(expect_token_type(")"));
        }
        else if (tokens_.peek_token().get_type() == "=") {
            token_list.push_back(expect_token_type("="));
            node_list.push_back(expression());
            token placeholder("placeholder", "", 0);
            token_list.push_back(placeholder);
            assignment = true;
            if (tokens_.peek_token().get_type() != ";") {
                std::stringstream s("missing semicolon on line ");
                s << tokens_.peek_token().get_line_num() - 1;
                throw std::range_error(s.str());
            }
        }
        else if (tokens_.peek_token().get_type() == "[") {
            token_list.push_back(expect_token_type("["));
            node_list.push_back(expression());
            token placeholder("placeholder", "", 0);
            token_list.push_back(placeholder);
            token_list.push_back(expect_token_type("]"));
        }
        else {
            token_list.push_back(tokens_.next_token());
        }
        t = tokens_.peek_token();
    }
    if (assignment) {
        // get var, put it in p
    }
    else {
        // parse as comp_exp
    }
    return p;
}

parse_tree_node* parser::identifier() {
    token id_token = expect_token_type("identifier");
    id_node* p = new id_node(id_token.get_str(), 0);
    return p;
}

parse_tree_node* parser::empty_node() {
    internal_node* p = new internal_node("empty", 0);
    return p;
}

token parser::expect_token_type(string type) {
    string message = "expected ";
    message += type;
    return expect_token_type(type, message);
}

token parser::expect_keyword(string keyword) {
    string message = "expected keyword ";
    message += keyword;
    return expect_keyword(keyword, message);
}

token parser::expect_token_type(string type, string message) {
    token t = tokens_.next_token();
    if (t.get_type() != type) {
        std::ios_base::openmode flags = SSTREAM_FLAGS;
        stringstream error("syntax error: line ", flags);
        error << t.get_line_num();
        error << " : ";
        error << message;
        error << " ; got token ";
        error << t.get_str();
        throw std::range_error(error.str());
    }
    return t;
}

token parser::expect_keyword(string keyword, string message) {
    token t = tokens_.next_token();
    if (t.get_type() != "keyword" || t.get_str() != keyword) {
        std::ios_base::openmode flags = SSTREAM_FLAGS;
        stringstream error("syntax error; invalid keyword:  line ", flags);
        error << t.get_line_num();
        error << " : ";
        error << message;
        error << " ; got token ";
        error << t.get_str();
        throw std::range_error(error.str());
    }
    return t;
}
