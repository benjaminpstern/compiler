#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef map
#include <map>
#endif

#ifndef vector
#include <vector>
#endif

#ifndef string
#include <string>
#endif
#ifndef CODEGEN_UTILS
#define CODEGEN_UTILS
using std::map;
using std::vector;
using std::string;
void assign_variable_depth(parse_tree_node* p);
map<string, string> find_strings(parse_tree_node* p);
int find_strings(parse_tree_node* p, map<string, string>& m, int n);
string var_placement(parse_tree_node* p);
int max_pos(parse_tree_node* p);
int get_variable_offset(parse_tree_node* p);
int get_array_offset(parse_tree_node* p);
vector<string> register_names();
parse_tree_node* get_lvalue_id(parse_tree_node* p);
#endif
