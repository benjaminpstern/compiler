#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef STDEXCEPT
#include <stdexcept>
#endif

parse_tree_node::parse_tree_node() {
    parse_tree_node("empty", 0);
}

parse_tree_node::parse_tree_node(string type, int num_children) {
    type_ = type;
    num_children_ = num_children;
    if (num_children_ > 0) {
        children_ = new parse_tree_node[num_children_];
    }
}

string parse_tree_node::to_str() {
    std::stringstream s("", std::ios_base::app | std::ios_base::out);
    to_str(s);
    string str = s.str();
    return str;
}

string parse_tree_node::to_indented_str() {
    std::stringstream s("", std::ios_base::app | std::ios_base::out);
    to_indented_str(s, 0);
    string str = s.str();
    return str;
}

void parse_tree_node::to_str(std::stringstream& s) {
    s << "<";
    s << type_;
    s << ", (";
    for (int i = 0; i < num_children_; ++i) {
        children_[i].to_str(s);
    }
    s << ")>";
}

void parse_tree_node::to_indented_str(std::stringstream& s, int depth) {
    for (int i = 0; i < depth; ++i) {
        s << "\t";
    }
    s << "<";
    s << type_;
    s << ", (";
    for (int i = 0; i < num_children_; ++i) {
        s << "\n";
        children_[i].to_indented_str(s, depth+1);
    }
    if (num_children_ > 0) {
        s << "\n";
        for (int i = 0; i < depth; ++i) {
            s << "\t";
        }
    }
    s << ")>";
}

void parse_tree_node::set_child(int num, parse_tree_node& child) {
    if (num < 0 || num >= num_children_) {
        throw std::range_error("cannot set that child: index out of bounds");
    }
    children_[num] = child;
}
parse_tree_node::~parse_tree_node() {
    if (num_children_ > 0) {
        delete [] children_;
    }
}
