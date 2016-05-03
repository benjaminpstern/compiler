#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef STDEXCEPT
#include <stdexcept>
#endif

#ifndef IOSTREAM
#include <iostream>
#endif
parse_tree_node::parse_tree_node() {
    init("empty", 0, 0);
}

parse_tree_node::parse_tree_node(string type, int num_children) {
    init(type, num_children, 0);
}

string parse_tree_node::get_type() {
    return type_;
}

int parse_tree_node::get_line_num() {
    return line_num_;
}

int parse_tree_node::get_num_children() {
    return num_children_;
}

parse_tree_node* parse_tree_node::get_declaration() {
    return declaration_;
}

string parse_tree_node::get_evaluated_type() {
    return evaluated_type_;
}

void parse_tree_node::set_evaluated_type(string type) {
    evaluated_type_ = type;
}

int parse_tree_node::get_variable_depth() {
    return variable_depth_;
}

int parse_tree_node::get_variable_pos() {
    return variable_position_;
}

void parse_tree_node::set_variable_depth(int depth) {
    variable_depth_ = depth;
    // std::cout << "setting depth of " << to_str() << " to " << depth << std::endl;
}

void parse_tree_node::set_variable_pos(int pos) {
    variable_position_ = pos;
    // std::cout << "setting pos of " << to_str() << " to " << pos << std::endl;
}

parse_tree_node* parse_tree_node::get_child_n(int n) {
    return children_[n];
}

parse_tree_node::parse_tree_node(string type, int num_children, int line_num) {
    init(type, num_children, line_num);
}


void parse_tree_node::set_declaration(parse_tree_node* p) {
    declaration_ = p;
}

void parse_tree_node::init(string type, int num_children, int line_num) {
    type_ = type;
    num_children_ = num_children;
    line_num_ = line_num;
    if (num_children_ > 0) {
        children_ = new parse_tree_node*[num_children_];
    }
}

string internal_node::to_str() {
    std::stringstream s("", std::ios_base::app | std::ios_base::out);
    add_to_stream(s);
    string str = s.str();
    return str;
}

string internal_node::to_indented_str() {
    std::stringstream s("", std::ios_base::app | std::ios_base::out);
    to_indented_str(s, 0);
    string str = s.str();
    return str;
}

void internal_node::add_to_stream(std::stringstream& s) {
    s << type_;
    s << ": (";
    for (int i = 0; i < num_children_; ++i) {
        children_[i]->add_to_stream(s);
        if (i < num_children_ - 1) {
            s << ", ";
        }
    }
    s << ")";
}

void internal_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "  ";
    }
    s << "Line " << line_num_ << ": ";
    s << type_;
    s << ": (";
    for (int i = 0; i < num_children_; ++i) {
        s << "\n";
        children_[i]->to_indented_str(s, depth+1);
    }
    if (num_children_ > 0) {
        s << "\n";
        for (int i = 0; i < depth; ++i) {
            s << "  ";
        }
    }
    s << ")";
}

void parse_tree_node::set_child(int num, parse_tree_node* child) {
    if (num < 0 || num >= num_children_) {
        throw std::range_error("cannot set that child: index out of bounds");
    }
    children_[num] = child;
}

parse_tree_node::~parse_tree_node() {
    for (int i = 0; i < num_children_; ++i) {
        delete children_[i];
    }
    if (num_children_ > 0) {
        delete [] children_;
    }
}

terminal_node::terminal_node() {
    line_num_ = 0;
}

terminal_node::terminal_node(int line_num) {
    line_num_ = line_num;
}

id_node::id_node(string value, int line_num) : terminal_node() {
    init("id", 0, line_num);
    value_ = value;
}

id_node::id_node(string value) : terminal_node() {
    init("id", 0, 0);
    value_ = value;
}

string id_node::to_str() {
    string return_str = type_;
    return_str += ": ";
    return_str += value_;
    return return_str;
}

string id_node::get_value() {
    return value_;
}

int_node::int_node(int value, int line_num) : terminal_node() {
    init("int", 0, line_num);
    value_ = value;
}

int_node::int_node(int value) : terminal_node() {
    init("int", 0, 0);
    value_ = value;
}

int int_node::get_value() {
    return value_;
}

string int_node::to_str() {
    std::stringstream s(type_, std::ios_base::app | std::ios_base::out);
    s << ": ";
    s << value_;
    return s.str();
}

float_node::float_node(float value, int line_num) : terminal_node() {
    init("float", 0, line_num);
    value_ = value;
}

float_node::float_node(float value) : terminal_node() {
    init("float", 0, 0);
    value_ = value;
}

string float_node::to_str() {
    std::stringstream s(type_, std::ios_base::app | std::ios_base::out);
    s << ": ";
    s << value_;
    return s.str();
}

string_node::string_node(string value, int line_num) : terminal_node() {
    init("string", 0, line_num);
    value_ = value;
}

string_node::string_node(string value) : terminal_node() {
    init("string", 0, 0);
    value_ = value;
}

string string_node::get_value() {
    return value_;
}

string string_node::to_str() {
    std::stringstream s(type_, std::ios_base::app | std::ios_base::out);
    s << ": ";
    s << value_;
    return s.str();
}

internal_node::internal_node(string type, int num_children) : parse_tree_node() {
    init(type, num_children, 0);
}

internal_node::internal_node(string type, int num_children, int line_num) : parse_tree_node() {
    init(type, num_children, line_num);
}

type_node::type_node(string type) : terminal_node() {
    init("type", 0, 0);
    value_ = type;
}

type_node::type_node(string type, int line_num) : terminal_node() {
    init("type", 0, line_num);
    value_ = type;
}

string type_node::to_str() {
    return "type: " + value_;
}

string type_node::get_value() {
    return value_;
}

string terminal_node::to_indented_str() {
    return to_str();
}

void terminal_node::add_to_stream(std::stringstream& s) {
    s << to_str();
}

void terminal_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "  ";
    }
    s << to_indented_str();
}

op_node::op_node(string op) : terminal_node() {
    init("op", 0, 0);
    op_ = op;
}

op_node::op_node(string op, int line_num) : terminal_node() {
    init("op", 0, line_num);
    op_ = op;
}

string op_node::get_op() {
    return op_;
}

string op_node::to_str() {
    return "op: " + op_;
}
