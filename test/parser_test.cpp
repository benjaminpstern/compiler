#ifndef GTEST
#include <gtest/gtest.h>
#endif
#ifndef GMOCK
#include <gmock/gmock.h>
#endif

#ifndef PARSE_TREE
#include "parse_tree.h"
#endif

#ifndef TOKENIZER
#include "tokenizer.h"
#endif

#ifndef PARSER
#include "parser.h"
#endif

TEST(ParseTreeTest, TestPrintSingleNode) {
    internal_node node("stuff", 0);
    string node_str = node.to_str();
    ASSERT_EQ("stuff: ()", node_str);
}

TEST(ParseTreeTest, TestPrintNodeWithChildren) {
    internal_node node("stuff", 2);
    internal_node* left_child = new internal_node("left", 0);
    internal_node* right_child = new internal_node("right", 0);
    node.set_child(0, left_child);
    node.set_child(1, right_child);
    string node_str = node.to_str();
    ASSERT_EQ("stuff: (left: (), right: ())", node_str);
}

TEST(ParseTreeTest, TestPrintIndentedNodeWithChildren) {
    internal_node node("stuff", 2);
    internal_node* left_child = new internal_node("left", 0);
    internal_node* right_child = new internal_node("right", 0);
    node.set_child(0, left_child);
    node.set_child(1, right_child);
    string node_str = node.to_indented_str();
    ASSERT_EQ("Line 0: stuff: (\n  Line 0: left: ()\n  Line 0: right: ()\n)", node_str);
}
class string_tokenizer : public tokenizer_interface {
    private:
        line_tokenizer line_tokens_;
        std::vector<token> token_queue_;
    public:
        string_tokenizer(string s) : line_tokens_(s), token_queue_() {
        }
        bool has_next_token() {
            return line_tokens_.has_next_token() || !token_queue_.empty();
        }
        token next_token() {
            if (!token_queue_.empty()) {
                token t = token_queue_.front();
                token_queue_.erase(token_queue_.begin());
                return t;
            }
            return identify_token(line_tokens_.next_token_str(), 0);
        }
        token peek_token() {
            if (!token_queue_.empty()) {
                token t = token_queue_.front();
                return t;
            }
            token t = identify_token(line_tokens_.next_token_str(), 0);
            token_queue_.push_back(t);
            return t;
        }
        void unget_token(token t) {
            token_queue_.insert(token_queue_.begin(), t);
        }
};

TEST(StringTokenizerTest, TestStringTokenizerOverridesMethods) {
    string_tokenizer tokens("This is a tokenizer{stuff}");
    string token_strings[7] = {
        "This", "is", "a", "tokenizer", "{", "stuff", "}"};
    for (int i = 0; i < 7; ++i) {
        ASSERT_TRUE(tokens.has_next_token());
        ASSERT_EQ(token_strings[i], tokens.peek_token().get_str());
        token tok = tokens.next_token();
        ASSERT_EQ(token_strings[i], tok.get_str());
    }
    ASSERT_FALSE(tokens.has_next_token());
}

TEST(StringTokenizerTest, TestStringTokenizerUngets) {
    string_tokenizer tokens("This is a tokenizer{stuff}");
    string token_strings[7] = {
        "This", "is", "a", "tokenizer", "{", "stuff", "}"};

    token tok("int", "", 0);
    for (int i = 0; i < 7; ++i) {
        ASSERT_TRUE(tokens.has_next_token());
        ASSERT_EQ(token_strings[i], tokens.peek_token().get_str());
        tok = tokens.next_token();
        ASSERT_EQ(token_strings[i], tok.get_str());
    }
    tokens.unget_token(tok);
    ASSERT_EQ(token_strings[6], tokens.next_token().get_str());
    ASSERT_FALSE(tokens.has_next_token());
}

TEST(StringTokenizerTest, TestStringTokenizerWorksWithCompoundStatements) {
    string_tokenizer tokens("{x;y;z;}");
    string token_strings[8] = {"{", "x", ";", "y", ";", "z", ";", "}"};
    for (int i = 0; i < 8; ++i) {
        ASSERT_TRUE(tokens.has_next_token());
        ASSERT_EQ(token_strings[i], tokens.peek_token().get_str());
        token tok = tokens.next_token();
        ASSERT_EQ(token_strings[i], tok.get_str());
    }
    ASSERT_FALSE(tokens.has_next_token());
}
TEST(ParseTreeTest, TestParseSingleStatement) {
    string_tokenizer tokens("int x;");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (declaration list: (declaration: (var_dec: (type: int, empty: (), id: x, empty: ())), empty: ()))", node->to_str());
    delete node;
}

TEST(ParseTreeTest, TestParseFunction) {
    string_tokenizer tokens("void x(int y) {int x; z;} int x;");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (declaration list: (declaration: (function_dec: (type: void, id: x, params: (param list: (param: (type: int, empty: (), id: y, empty: ()), empty: ())), compound statement: (local decs: (var_dec: (type: int, empty: (), id: x, empty: ()), empty: ()), statement list: (statement: (expression statement: (expression: (comp exp: (E: (empty: (), empty: (), T: (empty: (), empty: (), F: (empty: (), factor: (id: z)))), empty: (), empty: ())))), empty: ())))), declaration list: (declaration: (var_dec: (type: int, empty: (), id: x, empty: ())), empty: ())))",
            node->to_str());
    delete node;
}
