#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef TOKENIZER
#include "tokenizer.h"
#endif

#ifndef PARSER
#define PARSER
class parser {
    public:
        parser(tokenizer_interface& t);
        internal_node* parse();
    private:
        tokenizer_interface& tokens_;
        parse_tree_node* statement();
        parse_tree_node* expression_statement();
        parse_tree_node* compound_statement();
        parse_tree_node* statement_list();
        parse_tree_node* if_statement();
        parse_tree_node* while_statement();
        parse_tree_node* return_statement();
        parse_tree_node* write_statement();
        parse_tree_node* expression();
        parse_tree_node* var();
        parse_tree_node* identifier();
        parse_tree_node* function_dec();
        parse_tree_node* var_dec();
        parse_tree_node* declaration();
        parse_tree_node* declaration_list();
        parse_tree_node* local_decs();
        parse_tree_node* params();
        parse_tree_node* param_list();
        parse_tree_node* param();
        parse_tree_node* type_specifier();
        parse_tree_node* empty_node();
        token expect_token_type(string type);
        token expect_keyword(string keyword);
        token expect_token_type(string type, string message);
        token expect_keyword(string keyword, string message);
};
#endif
