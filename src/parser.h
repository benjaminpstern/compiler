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
        parse_tree_node parse();
    private:
        tokenizer_interface& tokens_;
        parse_tree_node& statement();
        parse_tree_node& expression_statement();
        parse_tree_node& compound_statement();
        parse_tree_node& statement_list();
        parse_tree_node& expression();
        parse_tree_node& identifier();
        token expect_token_type(string type);
        token expect_token_type(string type, string message);
};
#endif
