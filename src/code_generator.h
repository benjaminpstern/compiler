#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef map
#include <map>
#endif

#ifndef string
#include <string>
#endif

#ifndef CODE_GENERATOR
#define CODE_GENERATOR
using std::map;
using std::string;
using std::cout;
using std::endl;
typedef struct symbol_tables {
    map<string, string> symbol_table;
    map<string, string> string_table;
    symbol_tables(map<string, string> syms, map<string, string> strings);
} symbol_tables;
symbol_tables* generate_rodata(parse_tree_node* p);
void assign_variable_depth(parse_tree_node* p);
void generate_code(parse_tree_node* p);
void generate_function(parse_tree_node* p, symbol_tables* global_tables);
map<string, string> find_strings(parse_tree_node* p);
int find_strings(parse_tree_node* p, map<string, string>& m, int n);
map<string, string> get_global_variables(parse_tree_node* p);
#endif
