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
map<string, string> generate_rodata(parse_tree_node* p);
void assign_variable_depth(parse_tree_node* p);
void generate_code(parse_tree_node* p);
void generate_function(parse_tree_node* p, map<string, string> string_table);
void generate_functions(parse_tree_node* p, map<string, string> string_table);
map<string, string> find_strings(parse_tree_node* p);
int find_strings(parse_tree_node* p, map<string, string>& m, int n);
void get_global_variables(parse_tree_node* p);
void generate_compound_statement(parse_tree_node* p,
        map<string, string> string_table, int stack_size);
void generate_statement(parse_tree_node* p,
        map<string, string> string_table, int stack_size);
void generate_statement_list(parse_tree_node* p,
        map<string, string> string_table, int stack_size);
void evaluate_expression(parse_tree_node* p);
void evaluate_int_expression(parse_tree_node* p);
string var_placement(parse_tree_node* p);
int max_pos(parse_tree_node* p);
#endif
