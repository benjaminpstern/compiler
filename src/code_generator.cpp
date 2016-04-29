#ifndef CODE_GENERATOR
#include "code_generator.h"
#endif
void generate_code(parse_tree_node* p) {
    symbol_tables* tables = generate_rodata(p);
    cout << ".globl main" << endl;
}

void generate_function(parse_tree_node* p, symbol_tables* global_tables) {
    id_node* id = (id_node*)p->get_child_n(1);
    string s = id->get_value();
    cout << s << ":" << endl;
    generate_code(p->get_child_n(3));
}

void generate_compound_statement(symbol_tables* global_tables,
        map<string, string> local_table) {
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
    if (p->get_type() == "compound statement") {
        int newDepth = depth;
        pos = assign_variable_depth_local_decs(p->get_child_n(0), newDepth, pos);
        newDepth++;
        assign_variable_depth(p->get_child_n(1), newDepth, pos);
    }
    else if (p->get_type() == "function_dec") {
        parse_tree_node* param = p->get_child_n(2);
        int newPos;
        if (param->get_child_n(0)->get_type() != "void") {
            newPos = assign_variable_depth_param_list(param->get_child_n(0), depth, pos);
        }
        assign_variable_depth(p->get_child_n(3), depth + 1, pos);
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            pos = assign_variable_depth(p->get_child_n(i), depth, pos);
        }
    }
    return pos;
}
void assign_variable_depth(parse_tree_node* p) {
    assign_variable_depth(p, 0, 0);
}

symbol_tables* generate_rodata(parse_tree_node* p) {
    cout << ".section .rodata" << endl;
    cout << ".PrintIntString: .string \"%d\\n\"" << endl;
    cout << ".PrintStringString: .string \"%s\"" << endl;
    cout << ".PrintNewLineString: .string \"\\n\"" << endl;
    map<string, string> string_table = find_strings(p);
    map<string, string> symbol_table = get_global_variables(p);
    for (std::map<string, string>::iterator it = string_table.begin();
            it != string_table.end(); ++it) {
        cout << it->second << ": .string " << it->first << endl;
    }
    return new symbol_tables(symbol_table, string_table);
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

map<string, string> get_global_variables(parse_tree_node* p) {
    map<string, string> m;
    return m;
}
