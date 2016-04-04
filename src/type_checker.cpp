#ifndef TYPE_CHECKER
#include "type_checker.h"
#endif

#ifndef IOSTREAM
#include <iostream>
#endif
bool DEBUG = true;

void type_check(parse_tree_node* p) {
    top_down_pass(p);
    check_type(p);
}

void top_down_pass(parse_tree_node* p) {
    map<string, parse_tree_node*> symbol_table;
    top_down_pass(p, symbol_table);
}

void add_var_to_symbol_table(parse_tree_node* p,
        map<string, parse_tree_node*>& symbol_table) {
    id_node* n = (id_node*)p->get_child_n(2);
    string id = n->get_value();
    symbol_table[id] = p;
    if (DEBUG) {
        std::cout << "Found declaration of variable "
            << id
            << " on line "
            << p->get_line_num()
            << std::endl;
    }
}

void add_function_to_symbol_table(parse_tree_node* p,
        map<string, parse_tree_node*>& symbol_table) {
    id_node* n = (id_node*)p->get_child_n(1);
    string id = n->get_value();
    symbol_table[id] = p;
    if (DEBUG) {
        std::cout << "Found declaration of function "
            << id
            << " on line "
            << p->get_line_num()
            << std::endl;
    }
}

string get_type_of_var_dec(parse_tree_node* p) {
    type_node* t = (type_node*)p->get_child_n(0);
    string type = t->get_value();
    if (p->get_child_n(1)->get_type() != "empty") {
        type += "*";
    }
    if (p->get_child_n(3)->get_type() != "empty") {
        type += "[]";
    }
    return type;
}

string get_type_of_var(parse_tree_node* p) {
    parse_tree_node* child1 = p->get_child_n(1);
    string type = get_type_of_var_dec(child1->get_declaration());
    if (p->get_child_n(0)->get_type() != "empty") {
        type = type.substr(1); // remove *
    }
    if (p->get_child_n(2)->get_type() != "empty") {
        type = type.substr(0, type.length() - 2);
    }
    return type;
}

void top_down_pass(parse_tree_node* p,
        map<string, parse_tree_node*>& symbol_table) {
    string type = p->get_type();
    if (type == "id") {
        id_node* i = (id_node*)p;
        string s = i->get_value();
        parse_tree_node* declaration = symbol_table[s];
        if (declaration == NULL) {
            std::stringstream message;
            message << "Error: no declaration for variable ";
            message << s;
            message << " On line ";
            message << i->get_line_num();
            message << std::endl;
            throw std::range_error(message.str());
        }
        if (DEBUG) {
            std::cout << "Id "
                << s
                << " on line "
                << p->get_line_num()
                << " is declared on line "
                << declaration->get_line_num()
                << std::endl;
        }
        p->set_declaration(declaration);
    }
    else if (type == "compound statement") {
        map<string, parse_tree_node*> new_table;
        clone(new_table, symbol_table);
        for (int i = 0; i < p->get_num_children(); i++) {
            top_down_pass(p->get_child_n(i), new_table);
        }
    }
    else if (type == "var_dec") {
        add_var_to_symbol_table(p, symbol_table);
    }
    else if (type == "param") {
        add_var_to_symbol_table(p, symbol_table);
    }
    else if (type == "function_dec") {
        add_function_to_symbol_table(p, symbol_table);
        map<string, parse_tree_node*> new_table;
        clone(new_table, symbol_table);
        for (int i = 2; i < p->get_num_children(); i++) {
            top_down_pass(p->get_child_n(i), new_table);
        }
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            top_down_pass(p->get_child_n(i), symbol_table);
        }
    }
}

string check_type(parse_tree_node* p) {
    string type = p->get_type();
    if (type == "if statement") {
        return type_check_if_statement(p);
    }
    else if (type == "while statement") {
        return type_check_while_statement(p);
    }
    else if (type == "expression") {
        return type_check_expression(p);
    }
    else if (type == "fun call") {
        return type_check_function_call(p);
    }
    else {
        for (int i = 0; i < p->get_num_children(); i++) {
            check_type(p->get_child_n(i));
        }
    }
    return "none";
}

string type_check_if_statement(parse_tree_node* p) {
    string exp = type_check_expression(p->get_child_n(0));
    assert_type("int", exp, p->get_line_num());
    if (DEBUG) {
        std::cout << "Verified that condition of if statement on line "
            << p->get_line_num()
            << " is of type int"
            << std::endl;
    }
    check_type(p->get_child_n(1));
    check_type(p->get_child_n(2));
    return "none";
}

string type_check_while_statement(parse_tree_node* p) {
    string exp = type_check_expression(p->get_child_n(0));
    assert_type("int", exp, p->get_line_num());
    if (DEBUG) {
        std::cout << "Verified that condition of while statement on line "
            << p->get_line_num()
            << " is of type int"
            << std::endl;
    }
    check_type(p->get_child_n(1));
    return "none";
}

string type_check_comp_exp(parse_tree_node* p) {
    parse_tree_node* comp_node = p->get_child_n(1);
    if (comp_node->get_type() == "empty") {
        return type_check_E(p->get_child_n(0));
    }
    string op = ((op_node*)comp_node)->get_op();
    string left_type = type_check_E(p->get_child_n(0));
    string right_type = type_check_E(p->get_child_n(2));
    if (op == "==" || op == "!=") {
        if (left_type == "int") {
            assert_type("int", right_type, p->get_line_num());
        }
        else if (right_type == "float") {
            assert_type("float", right_type, p->get_line_num());
        }
        else {
            // can't compare arrays or pointers
            assert_type("int", left_type, p->get_line_num());
        }
    }
    else {
        if (left_type == "int" || left_type == "float") {
            if (right_type == "int" || right_type == "float") {
                if (DEBUG) {
                    std::cout << "Verified that comp_exp on line "
                        << p->get_line_num()
                        << " is comparable"
                        << std::endl;
                }
            }
            else {
                assert_type("int", right_type, p->get_line_num());
            }
        }
        else {
            assert_type("int", left_type, p->get_line_num());
        }
    }
    return "int";
}

string type_check_E(parse_tree_node* p) {
    parse_tree_node* add_node = p->get_child_n(1);
    if (add_node->get_type() == "empty") {
        return type_check_T(p->get_child_n(2));
    }
    string left_type = type_check_E(p->get_child_n(0));
    string right_type = type_check_T(p->get_child_n(2));
    if (left_type != "float" && left_type != "int") {
        // can't add pointers, arrays, or strings
        assert_type("int", left_type, p->get_line_num());
    }
    else if (right_type != "float" && right_type != "int") {
        // can't add pointers, arrays, or strings
        assert_type("int", right_type, p->get_line_num());
    }
    else {
        if (DEBUG) {
            std::cout << "Verified that add_exp on line "
                << p->get_line_num()
                << " is addable"
                << std::endl;
        }
        if (left_type == "float" || right_type == "float") {
            return "float";
        }
    }
    return "int";
}

string type_check_T(parse_tree_node* p) {
    parse_tree_node* mul_node = p->get_child_n(1);
    if (mul_node->get_type() == "empty") {
        return type_check_F(p->get_child_n(2));
    }
    string left_type = type_check_T(p->get_child_n(0));
    string right_type = type_check_F(p->get_child_n(2));
    if (left_type != "float" && left_type != "int") {
        // can't add pointers, arrays, or strings
        assert_type("int", left_type, p->get_line_num());
    }
    else if (right_type != "float" && right_type != "int") {
        // can't add pointers, arrays, or strings
        assert_type("int", right_type, p->get_line_num());
    }
    else {
        if (DEBUG) {
            std::cout << "Verified that mult_exp on line "
                << p->get_line_num()
                << " is multiplyable"
                << std::endl;
        }
        if (left_type == "float" || right_type == "float") {
            return "float";
        }
    }
    return "int";
}

string type_check_F(parse_tree_node* p) {
    parse_tree_node* first_child = p->get_child_n(0);
    if (first_child->get_type() == "empty") {
        return type_check_factor(p->get_child_n(1));
    }
    op_node* op = (op_node*)first_child;
    string operation = op->get_op();
    if (operation == "-") {
        return type_check_F(p->get_child_n(1));
    }
    else if (operation == "*") {
        string type = type_check_factor(p->get_child_n(1));
        char last = type[type.length() - 1];
        if (last != '*') {
            std::stringstream msg;
            msg << "Error: cannot dereference expression of type ";
            msg << type;
            msg << " on line ";
            msg << p->get_line_num();
            msg << std::endl;
            throw std::range_error(msg.str());
        }
    }
    else if (operation == "&") {
        string type = type_check_factor(p->get_child_n(1));
        char last = type[type.length() - 1];
        if (last == '*' || last == ']') {
            std::stringstream msg;
            msg << "Error: cannot get address of expression of type ";
            msg << type;
            msg << " on line ";
            msg << p->get_line_num();
            msg << std::endl;
            throw std::range_error(msg.str());
        }
    }
    return "int";
}

string type_check_factor(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "paren_exp") {
        return type_check_expression(child->get_child_n(0));
    }
    else if (child->get_type() == "id") {
        id_node* id = (id_node*)child;
        return get_type_of_var_dec(id->get_declaration());
    }
    else if (child->get_type() == "array index") {
        return type_check_array_index(child);
    }
    else if (child->get_type() == "fun call") {
        return type_check_function_call(child);
    }
    else if (child->get_type() == "read") {
        return "int";
    }
    return child->get_type();
}

string type_check_array_index(parse_tree_node* p) {
    id_node* id = (id_node*)p->get_child_n(0);
    string type = get_type_of_var_dec(id->get_declaration());
    char last = type[type.length() - 1];
    if (last != ']') {
        stringstream msg;
        msg << "Error: cannot index expression of type "
            << type
            << " : line "
            << p->get_line_num()
            << std::endl;
        throw std::range_error(msg.str());
    }
    string index_type = type_check_expression(p->get_child_n(1));
    assert_type("int", index_type, p->get_line_num());
    if (DEBUG) {
        std::cout << "Verified that array index expression on line "
            << p->get_line_num()
            << " is of type int"
            << std::endl;
    }
    string new_type = type.substr(0, type.length() - 2); // remove []
    return new_type;
}

string type_check_function_call(parse_tree_node* p) {
    return "int";
}

string type_check_assignment(parse_tree_node* p) {
    parse_tree_node* left = p->get_child_n(0);
    id_node* id = (id_node*)left->get_child_n(1);
    parse_tree_node* right = p->get_child_n(1);
    string type = get_type_of_var(left);
    assert_type(type, type_check_expression(right), p->get_line_num());
    if (DEBUG) {
        std::cout << "Verified that assignment of variable "
            << id->get_value()
            << " on line "
            << p->get_line_num()
            << " is of type "
            << type
            << std::endl;
    }
    return type;
}

string type_check_expression(parse_tree_node* p) {
    parse_tree_node* child = p->get_child_n(0);
    if (child->get_type() == "assignment") {
        return type_check_assignment(child);
    }
    else if (child->get_type() == "comp exp") {
        return type_check_comp_exp(child);
    }
    return "int";
}

void assert_type(string expected, string actual, int line_num) {
    if (actual != expected) {
        std::stringstream s;
        s << "Error: cannot resolve type ";
        s << actual;
        s << " with expected type ";
        s << expected;
        s << " on line ";
        s << line_num;
        s << std::endl;
        throw std::range_error(s.str());
    }
}
void clone(map<string, parse_tree_node*>& dest,
        map<string, parse_tree_node*>& source) {
    for (std::map<string, parse_tree_node*>::iterator i = source.begin();
            i != source.end(); ++i) {
        string s = i->first;
        dest[s] = source[s];
    }
}
