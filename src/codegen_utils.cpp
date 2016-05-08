#ifndef CODEGEN_UTILS
#include "codegen_utils.h"
#endif
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
        parse_tree_node* brackets = child0->get_child_n(3);
        if (brackets->get_type() != "empty") {
            int size = ((int_node*)brackets->get_child_n(0))->get_value();
            pos += (size-1);
            child0->set_variable_pos(pos);
            pos++;
        }
        else {
            child0->set_variable_pos(pos);
            pos++;
        }
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
        p->set_variable_pos(pos);
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
    int offset = get_array_offset(declaration);
    return std::to_string(offset) + "(%rbx)";
}

int get_array_offset(parse_tree_node* p) {
    return p->get_variable_pos() * -8 - 8;
}
int max_pos(parse_tree_node* p) {
    if (p->get_type() == "var_dec") {
        if (p->get_variable_depth() > 1) {
            return p->get_variable_pos();
        }
    }
    int max = -1;
    for (int i = 0; i < p->get_num_children(); i++) {
        int pos = max_pos(p->get_child_n(i));
        if (pos > max) {
            max = pos;
        }
    }
    return max;
}
vector<string> register_names() {
    vector<string> v;
    string regs[16] = { "%rax", "%rbx", "%rcx", "%rdx", "%rbp", "%rsp", "%rsi",
        "%rdi", "%r8", "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15"};
    for (int i = 0; i < 16; i++) {
        v.push_back(regs[i]);
    }
    return v;
}
