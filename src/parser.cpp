#ifndef PARSER
#include "parser.h"
#endif

#ifndef IOSTREAM
#include <iostream>
#endif

parser::parser(tokenizer_interface& t) : tokens_(t) {
}

parse_tree_node parser::parse() {
    parse_tree_node* p = new parse_tree_node("program", 1);
    p->set_child(0, statement());
    return *p;
}

parse_tree_node& parser::statement() {
    parse_tree_node* p = new parse_tree_node("statement", 1);
    if (tokens_.peek_token().get_str() == "{") {
        p->set_child(0, compound_statement());
    }
    else {
        p->set_child(0, expression_statement());
    }
    return *p;
}

parse_tree_node& parser::expression_statement() {
    parse_tree_node* p = new parse_tree_node("expression statement", 1);
    p->set_child(0, expression());
    expect_token_type(";");
    return *p;
}

parse_tree_node& parser::compound_statement() {
    expect_token_type("{");
    parse_tree_node* p = new parse_tree_node("compound statement", 1);
    p->set_child(0, statement_list());
    expect_token_type("}");
    return *p;
}

parse_tree_node& parser::statement_list() {
    parse_tree_node* p = new parse_tree_node("statement list", 2);
    p->set_child(0, statement());
    if (tokens_.peek_token().get_type() != "}") {
        p->set_child(1, statement_list());
    }
    else {
        parse_tree_node* empty = new parse_tree_node("empty", 0);
        p->set_child(1, *empty);
    }
    return *p;
}

parse_tree_node& parser::expression() {
    parse_tree_node* p = new parse_tree_node("expression", 1);
    p->set_child(0, identifier());
    return *p;
}

parse_tree_node& parser::identifier() {
    token id_token = expect_token_type("identifier");
    parse_tree_node* p = new parse_tree_node(id_token.get_str(), 0);
    return *p;
}

token parser::expect_token_type(string type) {
    string message = "expected ";
    message += type;
    return expect_token_type(type, message);
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
