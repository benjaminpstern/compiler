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
        int cur_line_num();
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
        void get_exp_tokens(std::vector<token>& token_list,
                std::vector<char>& stack, int line_num);
        parse_tree_node* var();
        parse_tree_node* assignment();
        parse_tree_node* comp_exp();
        parse_tree_node* E();
        parse_tree_node* T();
        parse_tree_node* F();
        parse_tree_node* factor();
        parse_tree_node* paren_exp();
        parse_tree_node* var_non_dereference_or_funcall();
        parse_tree_node* array_index();
        parse_tree_node* fun_call();
        parse_tree_node* args();
        parse_tree_node* arg_list();
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
