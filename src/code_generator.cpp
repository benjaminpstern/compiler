#ifndef ALGORITHM
#include <algorithm>
#endif
#ifndef CHAR_UTILS
#include "char_utils.h"
#endif
#ifndef CODE_GENERATOR
#include "code_generator.h"
#endif

void vecrm(vector<string>& vec, string s) {
    for (int i = 0; i < vec.size(); i++) {
        if (vec[i] == s) {
            vec.erase(vec.begin() + i);
        }
    }
}
code_generator::code_generator() : string_table_() {
    labelno_ = 0;
    available_tmp_regs_ = register_names();
    vecrm(available_tmp_regs_, "%rax");
    vecrm(available_tmp_regs_, "%rbx");
    vecrm(available_tmp_regs_, "%rdx");
    vecrm(available_tmp_regs_, "%rdi");
    vecrm(available_tmp_regs_, "%rsp");
}

string code_generator::allocate_tmp_storage() {
    if (!available_tmp_regs_.empty()) {
        string reg = available_tmp_regs_.back();
        available_tmp_regs_.pop_back();
        return reg;
    }
    cout << "push %rax" << endl;
    return "0(%rsp)";
}

void code_generator::deallocate_tmp_storage(string space) {
    if (space == "0(%rsp)") {
        cout << "addq $8, %rsp" << endl;
    }
    else {
        available_tmp_regs_.push_back(space);
    }
}

string code_generator::allocate_tmp_storage_32bit() {
    for (int i = 0; i < available_tmp_regs_.size(); i++) {
        if (!is_numeric(available_tmp_regs_[i].at(2))) {
            string reg = available_tmp_regs_[i];
            vecrm(available_tmp_regs_, reg);
            string ret = reg.replace(1, 1, "e");
            return ret;
        }
    }
    cout << "push %rax" << endl;
    return "0(%rsp)";
}

void code_generator::deallocate_tmp_storage_32bit(string space) {
    if (space == "0(%rsp)") {
        cout << "addq $8, %rsp" << endl;
    }
    else {
        string newspace = space.replace(1,1,"r");
        available_tmp_regs_.push_back(newspace);
    }
}

void code_generator::generate_code(parse_tree_node* p) {
    assign_variable_depth(p);
    string_table_ = generate_rodata(p);
    cout << ".text" << endl;
    allocate_global_variables(p);
    cout << ".globl main" << endl;
    generate_functions(p);
}

string code_generator::new_label() {
    static int labelno = 0;
    string label = ".L" + std::to_string(labelno);
    labelno++;
    return label;
}

map<string, string> code_generator::generate_rodata(parse_tree_node* p) {
    cout << ".section .rodata" << endl;
    cout << ".PrintIntString: .string \"%d \"" << endl;
    cout << ".ReadIntString: .string \"%d\"" << endl;
    cout << ".PrintFloatString: .string \"%f \"" << endl;
    cout << ".PrintStringString: .string \"%s\"" << endl;
    cout << ".PrintNewLineString: .string \"\\n\"" << endl;
    map<string, string> string_table = find_strings(p);
    for (std::map<string, string>::iterator it = string_table.begin();
            it != string_table.end(); ++it) {
        cout << it->second << ": .string " << it->first << endl;
    }
    string_table["\"%d \""] = ".PrintIntString";
    string_table["\"%d\""] = ".ReadIntString";
    string_table["\"%f \""] = ".PrintFloatString";
    string_table["\"%s\""]= ".PrintStringString";
    string_table["\"\\n\""]= ".PrintNewLineString";
    return string_table;
}

void code_generator::allocate_global_variables(parse_tree_node* p) {
    if (p->get_type() == "var_dec") {
        if (!p->get_variable_depth()) {
            string id = ((id_node*)(p->get_child_n(2)))->get_value();
            int size;
            if (p->get_child_n(3)->get_type() == "empty") {
                size = 8;
            }
            else {
                parse_tree_node* brackets = p->get_child_n(3);
                int_node* size_node = (int_node*)(brackets->get_child_n(0));
                size = size_node->get_value() * 8;
            }
            cout << ".comm " << id << ", " << size << ", 32" << endl;
        }
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            allocate_global_variables(p->get_child_n(i));
        }
    }
}

void code_generator::generate_functions(parse_tree_node* p) {
    if (p->get_type() == "function_dec") {
        generate_function(p);
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            generate_functions(p->get_child_n(i));
        }
    }
}

void code_generator::generate_function(parse_tree_node* p) {
    id_node* id = (id_node*)p->get_child_n(1);
    string s = id->get_value();
    cout << s << ":" << endl;
    cout << "movq %rsp, %rbx" << endl;
    int stack_size = max_pos(p) * 8 + 8;
    cout << "subq $" << stack_size << ", %rsp" << endl;
    generate_compound_statement(p->get_child_n(3), stack_size);
    cout << "addq $" << stack_size << ", %rsp" << endl;
    cout << "ret" << endl;
}

void code_generator::generate_statement_list(parse_tree_node* p, int stack_size) {
    parse_tree_node* stmt = p->get_child_n(0);
    parse_tree_node* stmt_list = p->get_child_n(1);
    if (stmt->get_type() != "empty") {
        generate_statement(stmt, stack_size);
    }
    if (stmt_list->get_type() != "empty") {
        generate_statement_list(stmt_list, stack_size);
    }
}

void code_generator::generate_statement(parse_tree_node* p, int stack_size) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "write statement") {
        generate_write(child);
    }
    else if (child->get_type() == "return statement") {
        generate_return_statement(child, stack_size);
    }
    else if (child->get_type() == "expression statement") {
        evaluate_expression(child->get_child_n(0));
    }
    else if (child->get_type() == "if statement") {
        generate_if_statement(child, stack_size);
    }
    else if (child->get_type() == "while statement") {
        generate_while_statement(child, stack_size);
    }
    else if (child->get_type() == "compound statement") {
        generate_compound_statement(child, stack_size);
    }
}

void code_generator::generate_compound_statement(parse_tree_node* p, int stack_size) {
    generate_statement_list(p->get_child_n(1), stack_size);
}

void code_generator::generate_if_statement(parse_tree_node* p, int stack_size) {
    evaluate_expression(p->get_child_n(0));
    string label1 = new_label();
    parse_tree_node* else_node = p->get_child_n(2);
    cout << "cmp $0, %rax" << endl;
    cout << "je " << label1 << endl;
    generate_statement(p->get_child_n(1), stack_size);
    if (else_node->get_type() != "empty") {
        string label2 = new_label();
        cout << "jmp " << label2 << endl;
        cout << label1 << ":" << endl;
        generate_statement(else_node->get_child_n(0), stack_size);
        cout << label2 << ":" << endl;
    }
    else {
        cout << label1 << ":" << endl;
    }
}

void code_generator::generate_write(parse_tree_node* p) {
    parse_tree_node* print_this = p->get_child_n(0);
    string string_name;
    if (print_this->get_type() == "empty") {
        string_name = string_table_["\"\\n\""];
    }
    else {
        if (print_this->get_evaluated_type() == "string") {
            string_name = string_table_["\"%s\""];
            evaluate_val_expression(print_this);
            cout << "movq %rax, %rsi" << endl;
        }
        else if (print_this->get_evaluated_type() == "int") {
            string_name = string_table_["\"%d \""];
            evaluate_val_expression(print_this);
            cout << "movq %rax, %rsi" << endl;
        }
        else {
        }
    }
    cout << "push %rbx" << endl;
    cout << "movq $0, %rax" << endl;
    cout << "movq $" << string_name << ", %rdi" << endl;
    cout << "call printf" << endl;
    cout << "pop %rbx" << endl;
}

void code_generator::generate_return_statement(parse_tree_node* p, int stack_size) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_evaluated_type() == "int" || child->get_evaluated_type() == "string") {
        evaluate_val_expression(child);
        cout << "addq $" << stack_size << ", %rsp" << endl;
        cout << "ret" << endl;
    }
    else {
        evaluate_pointer_expression(child);
        cout << "addq $" << stack_size << ", %rsp" << endl;
        cout << "ret" << endl;
    }
}

void code_generator::generate_while_statement(parse_tree_node* p, int stack_size) {
    string label1 = new_label();
    string label2 = new_label();
    cout << label1 << ":" << endl;
    evaluate_expression(p->get_child_n(0));
    cout << "cmp $0, %rax" << endl;
    cout << "je " << label2 << endl;
    generate_statement(p->get_child_n(1), stack_size);
    cout << "jmp " << label1 << endl;
    cout << label2 << ":" << endl;
}

void code_generator::evaluate_expression(parse_tree_node* p) {
    string type = p->get_evaluated_type();
    if (type == "int" || type == "void" || type == "string") {
        evaluate_val_expression(p);
    }
    else if (type == "int[]" || type == "string[]") {
        evaluate_array_expression(p);
    }
    else if (type == "int*" || type == "string*") {
        evaluate_pointer_expression(p);
    }
    else {
        // TODO floats
    }
}

void code_generator::evaluate_val_expression(parse_tree_node* p) {
    string type = p->get_child_n(0)->get_type();
    if (type == "assignment") {
        evaluate_val_assignment(p->get_child_n(0));
    }
    else {
        evaluate_val_compexp(p->get_child_n(0));
    }
}

void code_generator::evaluate_val_compexp(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_val_E(left);
    if (right->get_type() != "empty") {
        string op = ((op_node*)p->get_child_n(1))->get_op();
        string space = allocate_tmp_storage_32bit();
        cout << "movl %eax, " << space << endl;
        evaluate_val_E(right);
        cout << "cmp %eax, " << space << endl;
        string label1 = new_label();
        string label2 = new_label();
        string jump_op;
        if (op == "==") {
            jump_op = "jne";
        }
        else if(op == "<=") {
            jump_op = "jg";
        }
        else if(op == ">=") {
            jump_op = "jl";
        }
        else if(op == ">") {
            jump_op = "jle";
        }
        else if(op == "<") {
            jump_op = "jge";
        }
        else if(op == "!=") {
            jump_op = "je";
        }
        cout << jump_op << " " << label1 << endl;
        cout << "movq $1, %rax" << endl;
        cout << "jmp " << label2 << endl;
        cout << label1 << ":" << endl;
        cout << "movq $0, %rax" << endl;
        cout << label2 << ":" << endl;
        deallocate_tmp_storage_32bit(space);
    }
}

void code_generator::evaluate_val_assignment(parse_tree_node* p) {
    parse_tree_node* exp = p->get_child_n(1);
    parse_tree_node* index_exp = p->get_child_n(0)->get_child_n(2);
    parse_tree_node* star = p->get_child_n(0)->get_child_n(0);
    parse_tree_node* id = p->get_child_n(0)->get_child_n(1);
    evaluate_expression(exp);
    if (index_exp->get_type() != "empty") {
        parse_tree_node* dec = id->get_declaration();
        string space = allocate_tmp_storage();
        cout << "movq %rax, " << space << endl;
        evaluate_val_expression(index_exp);
        if (!dec->get_variable_depth()) {
            string id_str = ((id_node*)id)->get_value();
            cout << "movq " << space << ", %rdi" << endl;
            cout << "movq %rdi, " << id_str << "(, %rax, 8)" << endl;
        }
        else if (dec->get_variable_depth() == 1) {
            cout << "salq $3, %rax" << endl;
            cout << "addq " << var_placement(id) << ", %rax" << endl;
            cout << "movq " << space << ", %rdi" << endl;
            cout << "movq %rdi, 0(%rax)" << endl;
        }
        else {
            cout << "salq $3, %rax" << endl;
            cout << "addq %rbx, %rax" << endl;
            int offset = get_local_offset(dec);
            cout << "addq $" << offset << ", %rax" << endl;
            cout << "movq " << space << ", %rdi" << endl;
            cout << "movq %rdi, 0(%rax)" << endl;
        }
        deallocate_tmp_storage(space);
    }
    else if (star->get_type() != "empty") {
        cout << "movq %rax, %rdi" << endl;
        cout << "movq " << var_placement(id) << ", %rax" << endl;
        cout << "movq %rdi, 0(%rax)" << endl;
    }
    else {
        cout << "movq %rax, " << var_placement(id) << endl;
    }
}

void code_generator::evaluate_val_E(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_val_T(right);
    if (left->get_type() != "empty") {
        string space = allocate_tmp_storage();
        cout << "movq %rax, " << space << endl;
        evaluate_val_E(left);
        string op = ((op_node*)p->get_child_n(1))->get_op();
        string addop;
        if (op == "+") {
            addop = "addq";
        }
        else {
            addop = "subq";
        }
        cout << addop << " " << space  << ", %rax" << endl;
        deallocate_tmp_storage(space);
    }
}

void code_generator::evaluate_val_T(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_val_F(right);
    if (left->get_type() != "empty") {
        string space = allocate_tmp_storage_32bit();
        cout << "movl %eax, " << space << endl;
        evaluate_val_T(left);
        string op = ((op_node*)p->get_child_n(1))->get_op();
        if (op == "*") {
            cout << "imul " << space << ", %eax" << endl;
        }
        else {
            cout << "push %rbx" << endl;
            cout << "movl " << space << ", %ebx" << endl;
            cout << "cltq" << endl;
            cout << "cqto" << endl;
            cout << "idiv %ebx" << endl;
            if (op == "%") {
                cout << "movl %edx, %eax" << endl;
            }
            cout << "pop %rbx" << endl;
        }
        deallocate_tmp_storage_32bit(space);
    }
}

void code_generator::evaluate_val_F(parse_tree_node* p) {
    parse_tree_node* op = p->get_child_n(0);
    parse_tree_node* child = p->get_child_n(1);
    if (op->get_type() == "empty") {
        evaluate_val_factor(child);
    }
    else {
        string operation = ((op_node*)op)->get_op();
        if (operation == "-") {
            evaluate_val_F(child);
            cout << "imul $-1, %eax" << endl;
        }
        else if (operation == "*") {
            evaluate_pointer_factor(child);
            cout << "movq 0(%rax), %rax" << endl;
        }
        else if (operation == "&") {
            evaluate_pointer_factor(p->get_child_n(1));
        }
    }
}

void code_generator::evaluate_val_factor(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "paren_exp") {
        parse_tree_node* child_child = child->get_child_n(0);
        evaluate_val_expression(child_child);
    }
    else if (child->get_type() == "array index") {
        parse_tree_node* id = child->get_child_n(0);
        parse_tree_node* index_exp = child->get_child_n(1);
        parse_tree_node* dec = id->get_declaration();
        evaluate_val_expression(index_exp);
        if (!dec->get_variable_depth()) {
            string id_str = ((id_node*)id)->get_value();
            cout << "movq " << id_str << "(, %rax, 8), %rax" << endl;
        }
        else if (dec->get_variable_depth() == 1) {
            cout << "salq $3, %rax" << endl;
            cout << "addq " << var_placement(id) << ", %rax" << endl;
            cout << "movq 0(%rax), %rax" << endl;
        }
        else {
            cout << "salq $3, %rax" << endl;
            cout << "addq %rbx, %rax" << endl;
            int offset = get_local_offset(dec);
            cout << "addq $" << offset << ", %rax" << endl;
            cout << "movq 0(%rax), %rax" << endl;
        }
    }
    else if (child->get_type() == "fun call") {
        string name = ((id_node*)child->get_child_n(0))->get_value();
        parse_tree_node* args = child->get_child_n(1);
        parse_tree_node* arg_list = args->get_child_n(0);
        int argno;
        if (arg_list->get_type() == "empty") {
            argno = 0;
        }
        else {
            argno = push_args(arg_list);
        }
        cout << "push %rbx" << endl;
        cout << "call " << name << endl;
        cout << "pop %rbx" << endl;
        cout << "addq $" << 8*argno << ", %rsp" << endl;
    }
    else if (child->get_type() == "id") {
        cout << "movq " << var_placement(child) << ", %rax" << endl;
    }
    else if (child->get_type() == "int") {
        int value = ((int_node*)child)->get_value();
        cout << "movq $" << value << ", %rax" << endl;
    }
    else if (child->get_type() == "string") {
        string str = ((string_node*)child)->get_value();
        string strlabel = string_table_[str];
        cout << "movq $" << strlabel << ", %rax" << endl;
    }
    else if (child->get_type() == "read") {
        cout << "subq $40, %rsp" << endl;
        cout << "movq $0, %rax" << endl;
        cout << "movq %rsp, %rsi" << endl;
        cout << "addq $24, %rsi" << endl;
        cout << "movq $.ReadIntString, %rdi" << endl;
        cout << "push %rbx" << endl;
        cout << "call scanf" << endl;
        cout << "pop %rbx" << endl;
        cout << "movq 24(%rsp), %rax" << endl;
        cout << "addq $40, %rsp" << endl;
    }
}

int code_generator::push_args(parse_tree_node* p) {
    if (p->get_type() == "empty") {
        return 0;
    }
    int argno = push_args(p->get_child_n(1));
    evaluate_expression(p->get_child_n(0));
    cout << "push %rax" << endl;
    return 1 + argno;
}

void code_generator::evaluate_array_expression(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    string type = child->get_type();
    if (type == "assignment") {
        // Not allowed
    }
    else {
        evaluate_array_compexp(child);
    }
}

void code_generator::evaluate_array_compexp(parse_tree_node* p) {
    evaluate_array_E(p->get_child_n(0));
}

void code_generator::evaluate_array_E(parse_tree_node* p) {
    evaluate_array_T(p->get_child_n(2));
}

void code_generator::evaluate_array_T(parse_tree_node* p) {
    evaluate_array_F(p->get_child_n(2));
}

void code_generator::evaluate_array_F(parse_tree_node* p) {
    evaluate_array_factor(p->get_child_n(1));
}

void code_generator::evaluate_array_factor(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    parse_tree_node* dec = child->get_declaration();
    if (!dec->get_variable_depth()) {
        string id = ((id_node*)child)->get_value();
        cout << "movq $" << id << ", %rax" << endl;
    }
    else if (dec->get_variable_depth() == 1) {
        cout << "movq " << var_placement(child) << ", %rax" << endl;
    }
    else {
        cout << "movq %rbx, %rax" << endl;
        cout << "addq $" << get_local_offset(dec) << ", %rax" << endl;
    }
}

void code_generator::evaluate_pointer_expression(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    string type = child->get_type();
    if (type == "assignment") {
        evaluate_pointer_assignment(p->get_child_n(0));
    }
    else {
        evaluate_pointer_compexp(child);
    }
}

void code_generator::evaluate_pointer_assignment(parse_tree_node* p) {
    parse_tree_node* exp = p->get_child_n(1);
    parse_tree_node* id = p->get_child_n(0)->get_child_n(1);
    evaluate_expression(exp);
    cout << "movq %rax, " << var_placement(id) << endl;
}

void code_generator::evaluate_pointer_compexp(parse_tree_node* p) {
    evaluate_pointer_E(p->get_child_n(0));
}

void code_generator::evaluate_pointer_E(parse_tree_node* p) {
    evaluate_pointer_T(p->get_child_n(2));
}

void code_generator::evaluate_pointer_T(parse_tree_node* p) {
    evaluate_pointer_F(p->get_child_n(2));
}

void code_generator::evaluate_pointer_F(parse_tree_node* p) {
    parse_tree_node* op = p->get_child_n(0);
    parse_tree_node* child = p->get_child_n(1);
    if (op->get_type() == "empty") {
        evaluate_val_factor(child);
    }
    else {
        parse_tree_node* lvalue = get_lvalue_id(child);
        if (lvalue->get_type() == "id") {
            parse_tree_node* dec = lvalue->get_declaration();
            if (dec->get_variable_depth() == 0) {
                string id = ((id_node*)lvalue)->get_value();
                cout << "movq $" << id << ", %rax" << endl;
            }
            else {
                cout << "movq %rbx, %rax" << endl;
                cout << "addq $" << get_variable_offset(lvalue) 
                    << ", %rax" << endl;
            }
        }
        else { // array index
            parse_tree_node* dec = lvalue->get_child_n(0)->get_declaration();
            if (dec->get_variable_depth() == 0) {
                string id = ((id_node*)lvalue)->get_value();
                evaluate_val_expression(lvalue->get_child_n(1));
                cout << "salq $3, %rax" << endl;
                cout << "addq $" << id << ", %rax" << endl;
            }
            else {
                evaluate_val_expression(lvalue->get_child_n(1));
                cout << "salq $3, %rax" << endl;
                cout << "addq %rbx, %rax" << endl;
                parse_tree_node* id = lvalue->get_child_n(0);
                cout << "addq $" << get_variable_offset(id) 
                    << ", %rax" << endl;
            }
        }
    }
}

void code_generator::evaluate_pointer_factor(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "id") {
        parse_tree_node* dec = child->get_declaration();
        if (!dec->get_variable_depth()) {
            string id = ((id_node*)child)->get_value();
            cout << "movq " << id << ", %rax" << endl;
        }
        else {
            cout << "movq " << var_placement(child) << ", %rax" << endl;
        }
    }
    else if (child->get_type() == "fun call") {
        string name = ((id_node*)child->get_child_n(0))->get_value();
        parse_tree_node* args = child->get_child_n(1);
        parse_tree_node* arg_list = args->get_child_n(0);
        int argno;
        if (arg_list->get_type() == "empty") {
            argno = 0;
        }
        else {
            argno = push_args(arg_list);
        }
        cout << "push %rbx" << endl;
        cout << "call " << name << endl;
        cout << "pop %rbx" << endl;
        cout << "addq $" << 8*argno << ", %rsp" << endl;
    }
}

string code_generator::get_string(parse_tree_node* p) {
    if (p->get_type() != "string") {
        for (int i = 0; i < p->get_num_children(); i++) {
            string s = get_string(p->get_child_n(i));
            if (s != "") {
                return s;
            }
        }
        return "";
    }
    return ((string_node*)p)->get_value();
}
