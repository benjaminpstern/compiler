#ifndef PARSER
#include "parser.h"
#endif

#ifndef MAP
#include <map>
#endif

#ifndef TYPE_CHECKER
#define TYPE_CHECKER
using std::map;
void type_check(parse_tree_node* p);
void top_down_pass(parse_tree_node* p);
string check_type(parse_tree_node* p);
string type_check_if_statement(parse_tree_node* p);
string type_check_while_statement(parse_tree_node* p);
string type_check_expression(parse_tree_node* p);
string type_check_E(parse_tree_node* p);
string type_check_T(parse_tree_node* p);
string type_check_F(parse_tree_node* p);
string type_check_factor(parse_tree_node* p);
string type_check_array_index(parse_tree_node* p);
string type_check_function_call(parse_tree_node* p);
string get_type_of_var(parse_tree_node* p);
string get_type_of_var_dec(parse_tree_node* p);
void top_down_pass(parse_tree_node* p,
        map<string, parse_tree_node*>& symbol_table);
void clone(map<string, parse_tree_node*>& dest,
        map<string, parse_tree_node*>& source);
void assert_type(string expected, string actual, int line_num);
#endif
