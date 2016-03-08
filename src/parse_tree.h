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
        parse_tree_node* get_child_n(int n);
        int get_num_children();
        string get_type();
        virtual string to_str() = 0;
        virtual string to_indented_str() = 0;
        virtual void add_to_stream(std::stringstream& s) = 0;
        virtual void to_indented_str(std::stringstream& s, int depth) = 0;
        void set_child(int num, parse_tree_node* child);
        virtual ~parse_tree_node();
    protected:
        parse_tree_node** children_;
        int num_children_;
        int line_num_;
        void init(string type, int num_children, int line_num);
        string type_;
};

class terminal_node : public parse_tree_node {
    public:
        terminal_node(int line_num);
        terminal_node();
        string to_indented_str();
        void add_to_stream(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
};

class id_node : public terminal_node {
    public:
        id_node(string value, int line_num);
        id_node(string value);
        string to_str();
    private:
        string value_;
};

class type_node : public terminal_node {
    public:
        type_node(string type, int line_num);
        type_node(string type);
        string to_str();
    private:
        string type_;
};

class op_node : public terminal_node {
    public:
        op_node(string op, int line_num);
        op_node(string op);
        string to_str();
    private:
        string op_;
};

class int_node : public terminal_node {
    public:
        int_node(int value, int line_num);
        int_node(int value);
        string to_str();
    private:
        int value_;
};

class float_node : public terminal_node {
    public:
        float_node(float value, int line_num);
        float_node(float value);
        string to_str();
    private:
        float value_;
};

class string_node : public terminal_node {
    public:
        string_node(string value, int line_num);
        string_node(string value);
        string to_str();
    private:
        string value_;
};
class internal_node : public parse_tree_node {
    public:
        internal_node(string type, int num_children, int line_num);
        internal_node(string type, int num_children);
        string to_str();
        string to_indented_str();
        void add_to_stream(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
    private:
        int value_;
};
#endif
