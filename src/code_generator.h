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

#ifndef CODEGEN_UTILS
#include "codegen_utils.h"
#endif

#ifndef CODE_GENERATOR
#define CODE_GENERATOR
using std::map;
using std::string;
using std::cout;
using std::endl;
class code_generator {
    public:
        code_generator();
        string new_label();
        map<string, string> generate_rodata(parse_tree_node* p);
        void generate_code(parse_tree_node* p);
        void allocate_global_variables(parse_tree_node* p);
        void generate_functions(parse_tree_node* p);
        void generate_function(parse_tree_node* p);
        void generate_statement_list(parse_tree_node* p, int stack_size);
        void generate_statement(parse_tree_node* p, int stack_size);
        void generate_compound_statement(parse_tree_node* p, int stack_size);
        void generate_write(parse_tree_node* p);
        void generate_if_statement(parse_tree_node* p, int stack_size);
        void generate_while_statement(parse_tree_node* p, int stack_size);
        void generate_return_statement(parse_tree_node* p, int stack_size);
        void evaluate_expression(parse_tree_node* p);
        void evaluate_int_expression(parse_tree_node* p);
        void evaluate_int_assignment(parse_tree_node* p);
        void evaluate_int_compexp(parse_tree_node* p);
        void evaluate_int_E(parse_tree_node* p);
        void evaluate_int_T(parse_tree_node* p);
        void evaluate_int_F(parse_tree_node* p);
        void evaluate_int_factor(parse_tree_node* p);
        void evaluate_array_compexp(parse_tree_node* p);
        void evaluate_array_E(parse_tree_node* p);
        void evaluate_array_T(parse_tree_node* p);
        void evaluate_array_F(parse_tree_node* p);
        void evaluate_array_factor(parse_tree_node* p);
        void evaluate_array_expression(parse_tree_node* p);
        void evaluate_string_expression(parse_tree_node* p);
        void evaluate_pointer_compexp(parse_tree_node* p);
        void evaluate_pointer_E(parse_tree_node* p);
        void evaluate_pointer_T(parse_tree_node* p);
        void evaluate_pointer_F(parse_tree_node* p);
        void evaluate_pointer_factor(parse_tree_node* p);
        void evaluate_pointer_expression(parse_tree_node* p);
        void evaluate_pointer_assignment(parse_tree_node* p);
        int push_args(parse_tree_node* p);
        string get_string(parse_tree_node* p);
        string allocate_tmp_storage();
        void deallocate_tmp_storage(string space);
        string allocate_tmp_storage_32bit();
        void deallocate_tmp_storage_32bit(string space);
    private:
        int labelno_;
        vector<string> available_tmp_regs_;
        map<string, string> string_table_;
};
#endif
