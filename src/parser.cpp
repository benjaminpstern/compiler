#ifndef PARSER
#include "parser.h"
#endif

#ifndef IOSTREAM
#include <iostream>
#endif

parser::parser(tokenizer_interface& t) : tokens_(t) {
}

internal_node* parser::parse() {
    internal_node* p = new internal_node("program", 1);
    p->set_child(0, statement());
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
    internal_node* p = new internal_node("compound statement", 1);
    p->set_child(0, statement_list());
    expect_token_type("}");
    return p;
}

parse_tree_node* parser::statement_list() {
    internal_node* p = new internal_node("statement list", 2);
    p->set_child(0, statement());
    if (tokens_.peek_token().get_type() != "}") {
        p->set_child(1, statement_list());
    }
    else {
        internal_node* empty = new internal_node("empty", 0);
        p->set_child(1, empty);
    }
    return p;
}

parse_tree_node* parser::if_statement() {
    internal_node* p = new internal_node("if statement", 2);
    expect_keyword("if");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
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

parse_tree_node* parser::function_dec() {
    internal_node* p = new internal_node("function_dec", 2);
    expect_keyword("while");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
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
        std::stringstream messageStream("invalid type on line ");
        messageStream << t.get_line_num() << ": ";
        messageStream << type;
        throw std::range_error(messageStream.str());
    }
    expect_keyword(type);
    type_node* n = new type_node(type, t.get_line_num());
    return n;
}

parse_tree_node* parser::expression() {
    internal_node* p = new internal_node("expression", 1);
    p->set_child(0, identifier());
    return p;
}

parse_tree_node* parser::identifier() {
    token id_token = expect_token_type("identifier");
    id_node* p = new id_node(id_token.get_str(), 0);
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
        std::ios_base::openmode flags = std::ios_base::app | std::ios_base::out;
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
        std::ios_base::openmode flags = std::ios_base::app | std::ios_base::out;
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
