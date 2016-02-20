#ifndef STRING
#include <string>
#endif

#ifndef PARSE_TREE
#define PARSE_TREE

#ifndef SSTREAM
#include <sstream>
#endif
using std::string;

class parse_tree_node {
    public:
        parse_tree_node();
        parse_tree_node(string type, int num_children);
        parse_tree_node(string type, int num_children, int line_num);
        parse_tree_node* get_children();
        int get_num_children();
        string get_type();
        string to_str();
        string to_indented_str();
        void set_child(int num, parse_tree_node& child);
        ~parse_tree_node();
    private:
        parse_tree_node* children_;
        int num_children_;
        int line_num_;
        void to_str(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
        void init(string type, int num_children, int line_num);
        string type_;
};

class id_node : public parse_tree_node {
    public:
        id_node(string value);
    private:
        string value_;
};

class int_node : public parse_tree_node {
    public:
        int_node(int value);
    private:
        int value_;
};
#endif
