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
        virtual string to_str() = 0;
        virtual string to_indented_str() = 0;
        virtual void to_str(std::stringstream& s) = 0;
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

class id_node : public parse_tree_node {
    public:
        id_node(string value, int line_num);
        id_node(string value);
        string to_str();
        string to_indented_str();
        void to_str(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
    private:
        string value_;
};

class type_node : public parse_tree_node {
    public:
        type_node(string type, int line_num);
        type_node(string type);
        string to_str();
        string to_indented_str();
        void to_str(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
    private:
        string type_;
};

class int_node : public parse_tree_node {
    public:
        int_node(int value, int line_num);
        int_node(int value);
        string to_str();
        string to_indented_str();
        void to_str(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
    private:
        int value_;
};

class internal_node : public parse_tree_node {
    public:
        internal_node(string type, int num_children, int line_num);
        internal_node(string type, int num_children);
        string to_str();
        string to_indented_str();
        void to_str(std::stringstream& s);
        void to_indented_str(std::stringstream& s, int depth);
    private:
        int value_;
};
#endif
