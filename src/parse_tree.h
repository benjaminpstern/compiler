#ifndef STRING
#include <string>
#endif

#ifndef PARSE_TREE
#define PARSE_TREE

using std::string;
class parse_tree_node {
    public:
        parse_tree_node();
        parse_tree_node* get_children();
        string type;
        ~parse_tree_node();
    private:
        parse_tree_node* children;
        int num_children;
};

#endif
