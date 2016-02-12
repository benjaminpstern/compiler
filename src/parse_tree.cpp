#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

parse_tree_node::parse_tree_node() {
}

parse_tree_node::~parse_tree_node() {
    for (int i = 0; i < num_children; ++i) {
    }
}
