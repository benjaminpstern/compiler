#ifndef CODE_GENERATOR
#include "code_generator.h"
#endif
string new_label() {
    static int labelno = 0;
    string label = "L" + std::to_string(labelno);
    labelno++;
    return label;
}

int max_pos(parse_tree_node* p) {
    if (p->get_type() == "var_dec") {
        if (p->get_variable_depth() > 1) {
            return p->get_variable_pos();
        }
    }
    int max = 0;
    for (int i = 0; i < p->get_num_children(); i++) {
        int pos = max_pos(p->get_child_n(i));
        if (pos > max) {
            max = pos;
        }
    }
    return max;
}

void generate_code(parse_tree_node* p) {
    assign_variable_depth(p);
    map<string, string> string_table = generate_rodata(p);
    cout << ".text" << endl;
    get_global_variables(p);
    cout << ".globl main" << endl;
    generate_functions(p, string_table);
}

void generate_function(parse_tree_node* p,
        map<string, string> string_table) {
    id_node* id = (id_node*)p->get_child_n(1);
    string s = id->get_value();
    cout << s << ":" << endl;
    cout << "movq %rsp, %rbx" << endl;
    int stack_size = max_pos(p) * 8 + 8;
    cout << "subq $" << stack_size << ", %rsp" << endl;
    generate_compound_statement(p->get_child_n(3), string_table, stack_size);
    cout << "addq $" << stack_size << ", %rsp" << endl;
    cout << "ret" << endl;
}

void generate_compound_statement(parse_tree_node* p,
        map<string, string> string_table,
        int stack_size) {
    generate_statement_list(p->get_child_n(1), string_table, stack_size);
}

void generate_statement_list(parse_tree_node* p,
        map<string, string> string_table,
        int stack_size) {
    parse_tree_node* stmt = p->get_child_n(0);
    parse_tree_node* stmt_list = p->get_child_n(1);
    if (stmt->get_type() != "empty") {
        generate_statement(stmt, string_table, stack_size);
    }
    if (stmt_list->get_type() != "empty") {
        generate_statement_list(stmt_list, string_table, stack_size);
    }
}

string get_string(parse_tree_node* p) {
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

void evaluate_int_assignment(parse_tree_node* p) {
    parse_tree_node* exp = p->get_child_n(1);
    parse_tree_node* index_exp = p->get_child_n(0)->get_child_n(2);
    parse_tree_node* id = p->get_child_n(0)->get_child_n(1);
    evaluate_expression(exp);
    if (index_exp->get_type() != "empty") {
        cout << "push %rax" << endl;
        evaluate_int_expression(index_exp);
        string id_str = ((id_node*)id)->get_value();
        cout << "movq 0(%rsp), %rdi" << endl;
        cout << "movq %rdi, " << id_str << "(, %rax, 8)" << endl;
        cout << "pop %rax" << endl;
    }
    else {
        cout << "movq %rax, " << var_placement(id) << endl;
    }
}

int push_args(parse_tree_node* p) {
    if (p->get_type() == "empty") {
        return 0;
    }
    int argno = push_args(p->get_child_n(1));
    evaluate_expression(p->get_child_n(0));
    cout << "push %rax" << endl;
    return 1 + argno;
}

string var_placement(parse_tree_node* p) {
    parse_tree_node* declaration = p->get_declaration();
    int depth = declaration->get_variable_depth();
    if (depth == 0) {
        return ((id_node*)p)->get_value();
    }
    if (depth == 1) {
        int offset = 8 * declaration->get_variable_pos() + 16;
        return std::to_string(offset) + "(%rbx)";
    }
    int offset = -8 * declaration->get_variable_pos() - 8;
    return std::to_string(offset) + "(%rbx)";
}
void evaluate_int_factor(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "paren_exp") {
        parse_tree_node* child_child = child->get_child_n(0);
        evaluate_int_expression(child_child);
    }
    else if (child->get_type() == "array index") {
        parse_tree_node* id = child->get_child_n(0);
        parse_tree_node* index_exp = child->get_child_n(1);
        parse_tree_node* dec = id->get_declaration();
        evaluate_int_expression(index_exp);
        if (dec->get_variable_depth()) {
        }
        else {
            string id_str = ((id_node*)id)->get_value();
            cout << "movq " << id_str << "(, %rax, 8), %rax" << endl;
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
    else {
        // TODO
    }
}

void evaluate_int_F(parse_tree_node* p) {
    parse_tree_node* op = p->get_child_n(0);
    parse_tree_node* child = p->get_child_n(1);
    if (op->get_type() == "empty") {
        evaluate_int_factor(child);
    }
    else {
        string operation = ((op_node*)op)->get_op();
        if (operation == "-") {
            evaluate_int_F(child);
            cout << "imul $-1, %eax" << endl;
        }
        else if (operation == "*") {
            // TODO
        }
        else if (operation == "&") {
            // TODO
        }
    }
}

void evaluate_int_T(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_int_F(right);
    if (left->get_type() != "empty") {
        cout << "push %rax" << endl;
        evaluate_int_T(left);
        string op = ((op_node*)p->get_child_n(1))->get_op();
        if (op == "*") {
            cout << "imul 0(%rsp), %eax" << endl;
        }
        else {
            cout << "push %rbx" << endl;
            cout << "movq 0(%rsp), %rbx" << endl;
            cout << "cltq" << endl;
            cout << "cqto" << endl;
            cout << "idiv %ebx" << endl;
            if (op == "%") {
                cout << "movl %edx, %eax" << endl;
            }
            cout << "pop %rbx" << endl;
        }
        cout << "addq $8, %rsp" << endl;
    }
}

void evaluate_int_E(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_int_T(right);
    if (left->get_type() != "empty") {
        cout << "push %rax" << endl;
        evaluate_int_E(left);
        string op = ((op_node*)p->get_child_n(1))->get_op();
        string addop;
        if (op == "+") {
            addop = "addq";
        }
        else {
            addop = "subq";
        }
        cout << addop << " 0(%rsp), %rax" << endl;
        cout << "addq $8, %rsp" << endl;
    }
}

void evaluate_int_compexp(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    parse_tree_node* right = p->get_child_n(2);
    evaluate_int_E(left);
    if (right->get_type() != "empty") {
        string op = ((op_node*)p->get_child_n(1))->get_op();
        cout << "push %rax" << endl;
        evaluate_int_E(right);
        cout << "cmp %eax, 0(%rsp)" << endl;
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
        cout << "addq $8, %rsp" << endl;
    }
}

void evaluate_int_expression(parse_tree_node* p) {
    string type = p->get_child_n(0)->get_type();
    if (type == "assignment") {
        evaluate_int_assignment(p->get_child_n(0));
    }
    else {
        evaluate_int_compexp(p->get_child_n(0));
    }
}

void evaluate_expression(parse_tree_node* p) {
    string type = p->get_evaluated_type();
    if (type == "int" || type == "void") {
        evaluate_int_expression(p);
    }
    else {
        // TODO
    }
}

void generate_write(parse_tree_node* p, map<string, string> string_table) {
    parse_tree_node* print_this = p->get_child_n(0);
    string string_name;
    if (print_this->get_type() == "empty") {
        string_name = string_table["\"\\n\""];
    }
    else {
        if (print_this->get_evaluated_type() == "string") {
            string_name = string_table["\"%s\""];
            string print_string = string_table[get_string(print_this)];
            cout << "movq $" << print_string << ", %rsi" << endl;
        }
        else if (print_this->get_evaluated_type() == "int") {
            string_name = string_table["\"%d\\n\""];
            evaluate_int_expression(print_this);
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

void generate_return_statement(parse_tree_node* p, int stack_size) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_evaluated_type() == "int") {
        evaluate_int_expression(child);
        cout << "addq $" << stack_size << ", %rsp" << endl;
        cout << "ret" << endl;
    }
    else {
        // TODO
    }
}

void generate_while_statement(parse_tree_node* p,
        map<string, string> string_table,
        int stack_size) {
    string label1 = new_label();
    string label2 = new_label();
    cout << label1 << ":" << endl;
    evaluate_expression(p->get_child_n(0));
    cout << "cmp $0, %rax" << endl;
    cout << "je " << label2 << endl;
    generate_statement(p->get_child_n(1), string_table, stack_size);
    cout << "jmp " << label1 << endl;
    cout << label2 << ":" << endl;
}

void generate_if_statement(parse_tree_node* p,
        map<string, string> string_table,
        int stack_size) {
    evaluate_expression(p->get_child_n(0));
    string label1 = new_label();
    parse_tree_node* else_node = p->get_child_n(2);
    cout << "cmp $0, %rax" << endl;
    cout << "je " << label1 << endl;
    generate_statement(p->get_child_n(1), string_table, stack_size);
    if (else_node->get_type() != "empty") {
        string label2 = new_label();
        cout << "jmp " << label2 << endl;
        cout << label1 << ":" << endl;
        generate_statement(else_node->get_child_n(0), string_table, stack_size);
        cout << label2 << ":" << endl;
    }
    else {
        cout << label1 << ":" << endl;
    }
}

void generate_statement(parse_tree_node* p,
        map<string, string> string_table,
        int stack_size) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "write statement") {
        generate_write(child, string_table);
    }
    else if (child->get_type() == "return statement") {
        generate_return_statement(child, stack_size);
    }
    else if (child->get_type() == "expression statement") {
        evaluate_expression(child->get_child_n(0));
    }
    else if (child->get_type() == "if statement") {
        generate_if_statement(child, string_table, stack_size);
    }
    else if (child->get_type() == "while statement") {
        generate_while_statement(child, string_table, stack_size);
    }
    else if (child->get_type() == "compound statement") {
        generate_compound_statement(child, string_table, stack_size);
    }
}

symbol_tables::symbol_tables(map<string, string> syms,
        map<string, string> strings) {
    symbol_table = syms;
    string_table = strings;
}

int assign_variable_depth_param_list(parse_tree_node* p, int pos) {
    int depth = 1;
    parse_tree_node* child0 = p->get_child_n(0);
    if (child0->get_type() != "empty") {
        child0->set_variable_depth(depth);
        child0->set_variable_pos(pos);
        pos++;
        parse_tree_node* child1 = p->get_child_n(1);
        if (child1->get_type() != "empty") {
            pos = assign_variable_depth_param_list(child1, pos);
        }
    }
    return pos;
}

int assign_variable_depth_local_decs(parse_tree_node* p, int depth, int pos) {
    parse_tree_node* child0 = p->get_child_n(0);
    if (child0->get_type() != "empty") {
        child0->set_variable_depth(depth);
        child0->set_variable_pos(pos);
        pos++;
        parse_tree_node* child1 = p->get_child_n(1);
        if (child1->get_type() != "empty") {
            pos = assign_variable_depth_local_decs(child1, depth, pos);
        }
    }
    return pos;
}
int assign_variable_depth(parse_tree_node* p, int depth, int pos) {
    if (p->get_type() == "var_dec") {
        p->set_variable_depth(depth);
    }
    if (p->get_type() == "compound statement") {
        int newDepth = depth;
        pos = assign_variable_depth_local_decs(p->get_child_n(0), newDepth, pos);
        newDepth++;
        assign_variable_depth(p->get_child_n(1), newDepth, pos);
    }
    else if (p->get_type() == "function_dec") {
        parse_tree_node* param = p->get_child_n(2);
        if (param->get_child_n(0)->get_type() != "void") {
            assign_variable_depth_param_list(param->get_child_n(0), 0);
        }
        assign_variable_depth(p->get_child_n(3), 2, 0);
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            assign_variable_depth(p->get_child_n(i), depth, pos);
        }
    }
    return pos;
}
void assign_variable_depth(parse_tree_node* p) {
    assign_variable_depth(p, 0, 0);
}

map<string, string> generate_rodata(parse_tree_node* p) {
    cout << ".section .rodata" << endl;
    cout << ".PrintIntString: .string \"%d\\n\"" << endl;
    cout << ".ReadIntString: .string \"%d\"" << endl;
    cout << ".PrintFloatString: .string \"%f\\n\"" << endl;
    cout << ".PrintStringString: .string \"%s\"" << endl;
    cout << ".PrintNewLineString: .string \"\\n\"" << endl;
    map<string, string> string_table = find_strings(p);
    for (std::map<string, string>::iterator it = string_table.begin();
            it != string_table.end(); ++it) {
        cout << it->second << ": .string " << it->first << endl;
    }
    string_table["\"%d\\n\""] = ".PrintIntString";
    string_table["\"%d\""] = ".ReadIntString";
    string_table["\"%f\\n\""] = ".PrintFloatString";
    string_table["\"%s\""]= ".PrintStringString";
    string_table["\"\\n\""]= ".PrintNewLineString";
    return string_table;
}

map<string, string> find_strings(parse_tree_node* p) {
    map<string, string> m;
    find_strings(p, m, 0);
    return m;
}

int find_strings(parse_tree_node* p, map<string, string>& m, int n) {
    if (p->get_type() == "string") {
        string_node* s = (string_node*)p;
        string label = "s";
        label += (n + '0');
        m[s->get_value()] = label;
        return n + 1;
    }
    for (int i = 0; i < p->get_num_children(); i++) {
        n = find_strings(p->get_child_n(i), m, n);
    }
    return n;
}

void get_global_variables(parse_tree_node* p) {
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
            get_global_variables(p->get_child_n(i));
        }
    }
}

void generate_functions(parse_tree_node* p, map<string, string> string_table) {
    if (p->get_type() == "function_dec") {
        generate_function(p, string_table);
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            generate_functions(p->get_child_n(i), string_table);
        }
    }
}
