#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef STDEXCEPT
#include <stdexcept>
#endif

parse_tree_node::parse_tree_node() {
    init("empty", 0, 0);
}

parse_tree_node::parse_tree_node(string type, int num_children) {
    init(type, num_children, 0);
}

parse_tree_node::parse_tree_node(string type, int num_children, int line_num) {
    init(type, num_children, line_num);
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
    to_str(s);
    string str = s.str();
    return str;
}

string internal_node::to_indented_str() {
    std::stringstream s("", std::ios_base::app | std::ios_base::out);
    to_indented_str(s, 0);
    string str = s.str();
    return str;
}

void internal_node::to_str(std::stringstream& s) {
    s << type_;
    s << ": (";
    for (int i = 0; i < num_children_; ++i) {
        children_[i]->to_str(s);
        if (i < num_children_ - 1) {
            s << ", ";
        }
    }
    s << ")";
}

void internal_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "\t";
    }
    s << type_;
    s << ": (";
    for (int i = 0; i < num_children_; ++i) {
        s << "\n";
        children_[i]->to_indented_str(s, depth+1);
    }
    if (num_children_ > 0) {
        s << "\n";
        for (int i = 0; i < depth; ++i) {
            s << "\t";
        }
    }
    s << ")";
}

void int_node::to_str(std::stringstream& s) {
    s << to_str();
}

void int_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "\t";
    }
    s << to_str();
}

void id_node::to_str(std::stringstream& s) {
    s << to_str();
}

void id_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "\t";
    }
    s << to_str();
}

void parse_tree_node::set_child(int num, parse_tree_node& child) {
    if (num < 0 || num >= num_children_) {
        throw std::range_error("cannot set that child: index out of bounds");
    }
    children_[num] = &child;
}

parse_tree_node::~parse_tree_node() {
    if (num_children_ > 0) {
        delete [] children_;
    }
}

id_node::id_node(string value, int line_num) : parse_tree_node() {
    init("id", 0, line_num);
    value_ = value;
}

id_node::id_node(string value) : parse_tree_node() {
    init("id", 0, 0);
    value_ = value;
}

string id_node::to_str() {
    string return_str = type_;
    return_str += ": ";
    return_str += value_;
    return return_str;
}

string id_node::to_indented_str() {
    return to_str();
}

int_node::int_node(int value, int line_num) : parse_tree_node() {
    init("id", 0, line_num);
    value_ = value;
}

int_node::int_node(int value) : parse_tree_node() {
    init("id", 0, 0);
    value_ = value;
}

string int_node::to_str() {
    std::stringstream s(type_, std::ios_base::app | std::ios_base::out);
    s << ": ";
    s << value_;
    return s.str();
}

string int_node::to_indented_str() {
    return to_str();
}

internal_node::internal_node(string type, int num_children) : parse_tree_node() {
    init(type, num_children, 0);
}

internal_node::internal_node(string type, int num_children, int line_num) : parse_tree_node() {
    init(type, num_children, line_num);
}
