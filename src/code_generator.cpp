#ifndef CODE_GENERATOR
#include "code_generator.h"
#endif
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
    generate_compound_statement(p->get_child_n(3), string_table);
}

void generate_compound_statement(parse_tree_node* p,
        map<string, string> string_table) {
    generate_statement_list(p->get_child_n(1), string_table);
}

void generate_statement_list(parse_tree_node* p,
        map<string, string> string_table) {
    parse_tree_node* stmt = p->get_child_n(0);
    parse_tree_node* stmt_list = p->get_child_n(1);
    if (stmt->get_type() != "empty") {
        generate_statement(stmt, string_table);
    }
    if (stmt_list->get_type() != "empty") {
        generate_statement_list(stmt_list, string_table);
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

void generate_eval_exp(parse_tree_node* p) {
}
void generate_write(parse_tree_node* p, map<string, string> string_table) {
    parse_tree_node* print_this = p->get_child_n(0);
    string string_name;
    if (print_this->get_type() == "empty") {
        string_name = string_table["\"\\n\""];
    }
    else {
        if (print_this->get_evaluated_type() == "string") {
            string_name = string_table[get_string(print_this)];
        }
        else if (print_this->get_evaluated_type() == "int") {
            string_name = string_table["\"%d\\n\""];
            generate_eval_exp(print_this);
            cout << "movq %rax, %rsi";
        }
        else {
        }
    }
    cout << "movq $0, %rax" << endl;
    cout << "movq $" << string_name << ", %rdi" << endl;
    cout << "call printf" << endl;
}

void generate_statement(parse_tree_node* p,
        map<string, string> string_table) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "write statement") {
        generate_write(child, string_table);
    }
}

symbol_tables::symbol_tables(map<string, string> syms,
        map<string, string> strings) {
    symbol_table = syms;
    string_table = strings;
}

int assign_variable_depth_param_list(parse_tree_node* p, int depth, int pos) {
    parse_tree_node* child0 = p->get_child_n(0);
    if (child0->get_type() != "empty") {
        child0->set_variable_depth(depth);
        child0->set_variable_pos(pos);
        pos++;
        parse_tree_node* child1 = p->get_child_n(1);
        if (child1->get_type() != "empty") {
            pos = assign_variable_depth_param_list(child1, depth, pos);
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
        int newPos = pos;
        if (param->get_child_n(0)->get_type() != "void") {
            newPos = assign_variable_depth_param_list(param->get_child_n(0), depth + 1, newPos);
        }
        assign_variable_depth(p->get_child_n(3), depth + 2, newPos);
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
    cout << ".PrintFloatString: .string \"%f\\n\"" << endl;
    cout << ".PrintStringString: .string \"%s\"" << endl;
    cout << ".PrintNewLineString: .string \"\\n\"" << endl;
    map<string, string> string_table = find_strings(p);
    for (std::map<string, string>::iterator it = string_table.begin();
            it != string_table.end(); ++it) {
        cout << it->second << ": .string " << it->first << endl;
    }
    string_table["\"%d\\n\""] = ".PrintIntString";
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
            cout << ".comm " << id << ", 8, 32" << endl;
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
