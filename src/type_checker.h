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
void bottom_up_pass(parse_tree_node* p);
#endif
