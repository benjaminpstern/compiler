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
    internal_node* p = new internal_node("program", 1, cur_line_num());
    p->set_child(0, declaration_list());
    return p;
}

int parser::cur_line_num() {
    if (tokens_.has_next_token()) {
        return tokens_.peek_token().get_line_num();
    }
    return -1;
}

parse_tree_node* parser::statement() {
    internal_node* p = new internal_node("statement", 1, cur_line_num());
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
        expect_token_type(";");
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
    internal_node* p = new internal_node("expression statement", 1, cur_line_num());
    p->set_child(0, expression());
    expect_token_type(";");
    return p;
}

parse_tree_node* parser::compound_statement() {
    expect_token_type("{");
    internal_node* p = new internal_node("compound statement", 2, cur_line_num());
    p->set_child(0, local_decs());
    p->set_child(1, statement_list());
    expect_token_type("}");
    return p;
}

parse_tree_node* parser::statement_list() {
    internal_node* p = new internal_node("statement list", 2, cur_line_num());
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
    internal_node* p = new internal_node("local decs", 2, cur_line_num());
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
    internal_node* p = new internal_node("if statement", 3, cur_line_num());
    expect_keyword("if");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
    if (tokens_.peek_token().get_str() == "else") {
        expect_keyword("else");
        internal_node* else_node = new internal_node("else statement", 1, cur_line_num());
        else_node->set_child(0, statement());
        p->set_child(2, else_node);
    }
    else {
        p->set_child(2, empty_node());
    }
    return p;
}

parse_tree_node* parser::while_statement() {
    internal_node* p = new internal_node("while statement", 2, cur_line_num());
    expect_keyword("while");
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    p->set_child(1, statement());
    return p;
}

parse_tree_node* parser::return_statement() {
    internal_node* p = new internal_node("return statement", 1, cur_line_num());
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
    internal_node* p = new internal_node("write statement", 1, cur_line_num());
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
    expect_token_type(";");
    return p;
}

parse_tree_node* parser::function_dec() {
    internal_node* p = new internal_node("function_dec", 4, cur_line_num());
    p->set_child(0, type_specifier());
    p->set_child(1, identifier());
    expect_token_type("(");
    p->set_child(2, params());
    expect_token_type(")");
    p->set_child(3, compound_statement());
    return p;
}

parse_tree_node* parser::params() {
    internal_node* p = new internal_node("params", 1, cur_line_num());
    if (tokens_.peek_token().get_str() == "void") {
        expect_keyword("void");
        internal_node* v = new internal_node("void", 0, cur_line_num());
        p->set_child(0, v);
    }
    else if (tokens_.peek_token().get_str() == ")") {
        internal_node* v = new internal_node("void", 0, cur_line_num());
        p->set_child(0, v);
    }
    else {
        p->set_child(0, param_list());
    }
    return p;
}

parse_tree_node* parser::param_list() {
    internal_node* p = new internal_node("param list", 2, cur_line_num());
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
    internal_node* p = new internal_node("declaration list", 2, cur_line_num());
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
    internal_node* p = new internal_node("declaration", 1, cur_line_num());
    if (!tokens_.has_next_token()) {
        throw std::range_error("Unexpected EOF");
    }
    token type = expect_token_type("keyword");
    if (!tokens_.has_next_token()) {
        throw std::range_error("Unexpected EOF");
    }
    if (tokens_.peek_token().get_type() == "*") {
        tokens_.unget_token(type);
        p->set_child(0, var_dec());
    }
    else {
        token id = expect_token_type("identifier");
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
    internal_node* p = new internal_node("param", 4, cur_line_num());
    p->set_child(0, type_specifier());
    if (tokens_.peek_token().get_type() == "*") {
        op_node* star = new op_node("*", cur_line_num());
        p->set_child(1, star);
        expect_token_type("*");
    }
    else {
        p->set_child(1, empty_node());
    }
    p->set_child(2, identifier());
    if (tokens_.peek_token().get_type() == "[") {
        internal_node* brackets = new internal_node("[]", 0, cur_line_num());
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
    internal_node* p = new internal_node("var_dec", 4, cur_line_num());
    p->set_child(0, type_specifier());
    if (tokens_.peek_token().get_type() == "*") {
        op_node* star = new op_node("*", cur_line_num());
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
        internal_node* brackets = new internal_node("[]", 1, cur_line_num());
        int_node* size = new int_node(std::stoi(t.get_str()), cur_line_num());
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

void parser::get_exp_tokens(std::vector<token>& token_list,
        std::vector<char>& stack, int line_num) {
    token t("placeholder", "", 0);
    while (!stack.empty()) {
        if (!tokens_.has_next_token()) {
            std::stringstream s("unmatched ( on line ");
            s << line_num;
            throw std::range_error(s.str());
        }
        t = tokens_.next_token();
        token_list.push_back(t);
        if (t.get_type() == ")") {
            if (stack.back() == '(') {
                stack.pop_back();
            }
            else {
                std::stringstream s("unmatched [ on line ");
                s << line_num;
                throw std::range_error(s.str());
            }
        }
        else if (t.get_type() == "]") {
            if (stack.back() == '[') {
                stack.pop_back();
            }
            else {
                std::stringstream s("unmatched [ on line ");
                s << line_num;
                throw std::range_error(s.str());
            }
        }
        else if(t.get_type() == "(") {
            stack.push_back('(');
        }
        else if(t.get_type() == "[") {
            stack.push_back('[');
        }
    }
}
parse_tree_node* parser::expression() {
    internal_node* p = new internal_node("expression", 1, cur_line_num());
    std::vector<token> token_list;
    std::vector<char> stack;
    token t = tokens_.peek_token();
    while (t.get_type() != ";" && t.get_type() != ")" && t.get_type() != "]") {
        if (t.get_type() == "(") {
            token_list.push_back(expect_token_type("("));
            stack.push_back('(');
            get_exp_tokens(token_list, stack, t.get_line_num());
        }
        else if (t.get_type() == "=") {
            while (!token_list.empty()) {
                tokens_.unget_token(token_list.back());
                token_list.pop_back();
            }
            p->set_child(0, assignment());
            return p;
        }
        else if (t.get_type() == "[") {
            token_list.push_back(expect_token_type("["));
            stack.push_back('[');
            get_exp_tokens(token_list, stack, t.get_line_num());
        }
        else {
            token_list.push_back(tokens_.next_token());
        }
        t = tokens_.peek_token();
    }
    while (!token_list.empty()) {
        tokens_.unget_token(token_list.back());
        token_list.pop_back();
    }
    p->set_child(0, comp_exp());
    return p;
}

parse_tree_node* parser::assignment() {
    internal_node* p = new internal_node("assignment", 2, cur_line_num());
    p->set_child(0, var());
    expect_token_type("=");
    p->set_child(1, expression());
    return p;
}
parse_tree_node* parser::comp_exp() {
    internal_node* p = new internal_node("comp exp", 3, cur_line_num());
    p->set_child(0, E());
    if (is_relop(tokens_.peek_token().get_type())) {
        op_node* relop = new op_node(tokens_.next_token().get_type(), cur_line_num());
        p->set_child(1, relop);
        p->set_child(2, E());
    }
    else {
        p->set_child(1, empty_node());
        p->set_child(2, empty_node());
    }
    return p;
}

parse_tree_node* parser::E() {
    internal_node* e = new internal_node("E", 3, cur_line_num());
    e->set_child(0, empty_node());
    e->set_child(1, empty_node());
    e->set_child(2, T());
    while (is_addop(tokens_.peek_token().get_type())) {
        op_node* addop = new op_node(tokens_.next_token().get_type(), cur_line_num());
        internal_node* e1 = new internal_node("E", 3, cur_line_num());
        e1->set_child(0, e);
        e1->set_child(1, addop);
        e1->set_child(2, T());
        e = e1;
    }
    return e;
}

parse_tree_node* parser::T() {
    internal_node* e = new internal_node("T", 3, cur_line_num());
    e->set_child(0, empty_node());
    e->set_child(1, empty_node());
    e->set_child(2, F());
    while (is_mulop(tokens_.peek_token().get_type())) {
        op_node* mulop = new op_node(tokens_.next_token().get_type(), cur_line_num());
        internal_node* e1 = new internal_node("T", 3, cur_line_num());
        e1->set_child(0, e);
        e1->set_child(1, mulop);
        e1->set_child(2, F());
        e = e1;
    }
    return e;
}

parse_tree_node* parser::F() {
    internal_node* p = new internal_node("F", 2, cur_line_num());
    if (tokens_.peek_token().get_type() == "-") {
        op_node* minus = new op_node(tokens_.next_token().get_type(), cur_line_num());
        p->set_child(0, minus);
        p->set_child(1, F());
        return p;
    }
    else if (tokens_.peek_token().get_type() == "*") {
        op_node* star = new op_node(tokens_.next_token().get_type(), cur_line_num());
        p->set_child(0, star);
    }
    else if (tokens_.peek_token().get_type() == "&") {
        op_node* ampersand = new op_node(tokens_.next_token().get_type(), cur_line_num());
        p->set_child(0, ampersand);
    }
    else {
        p->set_child(0, empty_node());
    }
    p->set_child(1, factor());
    return p;
}

parse_tree_node* parser::factor() {
    internal_node* p = new internal_node("factor", 1, cur_line_num());
    if (tokens_.peek_token().get_type() == "(") {
        p->set_child(0, paren_exp());
    }
    else if (tokens_.peek_token().get_type() == "identifier") {
        p->set_child(0, var_non_dereference_or_funcall());
    }
    else if (tokens_.peek_token().get_type() == "int") {
        token int_token = expect_token_type("int");
        int num = std::stoi(int_token.get_str());
        p->set_child(0, new int_node(num, int_token.get_line_num()));
    }
    else if (tokens_.peek_token().get_type() == "float") {
        token float_token = expect_token_type("float");
        float num = std::stof(float_token.get_str());
        p->set_child(0, new float_node(num, float_token.get_line_num()));
    }
    else if (tokens_.peek_token().get_type() == "string") {
        token string_token = expect_token_type("string");
        p->set_child(0, new string_node(string_token.get_str(),
                    string_token.get_line_num()));
    }
    else {
        expect_keyword("read");
        expect_token_type("(");
        expect_token_type(")");
        p->set_child(0, new internal_node("read", 0, cur_line_num()));
    }
    return p;
}

parse_tree_node* parser::paren_exp() {
    internal_node* p = new internal_node("paren_exp", 1, cur_line_num());
    expect_token_type("(");
    p->set_child(0, expression());
    expect_token_type(")");
    return p;
}

parse_tree_node* parser::var_non_dereference_or_funcall() {
    token id = expect_token_type("identifier");
    if (tokens_.peek_token().get_type() == "[") {
        tokens_.unget_token(id);
        return array_index();
    }
    if (tokens_.peek_token().get_type() == "(") {
        tokens_.unget_token(id);
        return fun_call();
    }
    tokens_.unget_token(id);
    return identifier();
}


parse_tree_node* parser::array_index() {
    internal_node* p = new internal_node("array index", 2, cur_line_num());
    p->set_child(0, identifier());
    expect_token_type("[");
    p->set_child(1, expression());
    expect_token_type("]");
    return p;
}

parse_tree_node* parser::fun_call() {
    internal_node* p = new internal_node("fun call", 2, cur_line_num());
    p->set_child(0, identifier());
    expect_token_type("(");
    p->set_child(1, args());
    expect_token_type(")");
    return p;
}

parse_tree_node* parser::args() {
    internal_node* p = new internal_node("args", 1, cur_line_num());
    if (tokens_.peek_token().get_type() == ")") {
        p->set_child(0, empty_node());
    }
    else {
        p->set_child(0, arg_list());
    }
    return p;
}

parse_tree_node* parser::arg_list() {
    internal_node* p = new internal_node("arg list", 2, cur_line_num());
    p->set_child(0, expression());
    if (tokens_.peek_token().get_type() != ")") {
        expect_token_type(",");
        p->set_child(1, arg_list());
    }
    else {
        p->set_child(1, empty_node());
    }
    return p;
}

parse_tree_node* parser::var() {
    internal_node* p = new internal_node("var", 3, cur_line_num());
    if (tokens_.peek_token().get_type() == "*") {
        expect_token_type("*");
        op_node* star = new op_node("*", cur_line_num());
        p->set_child(0, star);
        p->set_child(1, identifier());
        p->set_child(2, empty_node());
    }
    else {
        p->set_child(0, empty_node());
        p->set_child(1, identifier());
        if (tokens_.peek_token().get_type() == "[") {
            expect_token_type("[");
            p->set_child(2, expression());
            expect_token_type("]");
        }
        else {
            p->set_child(2, empty_node());
        }
    }
    return p;
}

parse_tree_node* parser::identifier() {
    token id_token = expect_token_type("identifier");
    id_node* p = new id_node(id_token.get_str(), cur_line_num());
    return p;
}

parse_tree_node* parser::empty_node() {
    internal_node* p = new internal_node("empty", 0, cur_line_num());
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
    if (!tokens_.has_next_token()) {
        std::ios_base::openmode flags = SSTREAM_FLAGS;
        stringstream error("unexpected EOF ", flags);
        error << " : ";
        error << message;
        throw std::range_error(error.str());
    }
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
    if (!tokens_.has_next_token()) {
        std::ios_base::openmode flags = SSTREAM_FLAGS;
        stringstream error("unexpected EOF ", flags);
        error << " : ";
        error << message;
        throw std::range_error(error.str());
    }
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
