#ifndef TYPE_CHECKER
#include "type_checker.h"
#endif

void type_check(parse_tree_node* p) {
    top_down_pass(p);
    bottom_up_pass(p);
}

void top_down_pass(parse_tree_node* p) {
    map<string, parse_tree_node*> symbol_table;
}

void top_down_pass(map<string, parse_tree_node*> symbol_table,
        parse_tree_node* p) {
}

void bottom_up_pass(parse_tree_node* p) {
    map<string, parse_tree_node*> symbol_table;
}
