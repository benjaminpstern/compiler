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
    ASSERT_EQ("stuff: (\n\tleft: ()\n\tright: ()\n)", node_str);
}
class string_tokenizer : public tokenizer_interface {
    private:
        line_tokenizer line_tokens_;
        token* cached_token_;

    public:
        string_tokenizer(string s) : line_tokens_(s) {
            cached_token_ = NULL;
        }
        bool has_next_token() {
            return line_tokens_.has_next_token();
        }
        token next_token() {
            if (cached_token_ != NULL) {
                token t = *cached_token_;
                delete cached_token_;
                cached_token_ = NULL;
                return t;
            }
            return identify_token(line_tokens_.next_token_str(), 0);
        }
        token peek_token() {
            if (cached_token_ != NULL) {
                token t = *cached_token_;
                return t;
            }
            token t = identify_token(line_tokens_.next_token_str(), 0);
            cached_token_ = new token("int", "0", 0);
            *cached_token_ = t;
            return t;
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
    string_tokenizer tokens("x;");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (statement: (expression statement: (expression: "
            "(id: x))))", node->to_str());
    delete node;
}

TEST(ParseTreeTest, TestParseCompoundStatement) {
    string_tokenizer tokens("{x; y; z;}");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (statement: (compound statement: (statement list: "
            "(statement: (expression statement: (expression: (id: x))), "
            "statement list: (statement: (expression statement: (expression: "
            "(id: y))), statement list: (statement: (expression statement: "
            "(expression: (id: z))), empty: ()))))))",
            node->to_str());
    delete node;
}

TEST(ParseTreeTest, TestParseIfStatement) {
    string_tokenizer tokens("if (x) y;");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (statement: (if statement: (expression: (id: x), "
            "statement: (expression statement: (expression: (id: y))))))",
            node->to_str());
    delete node;
}

TEST(ParseTreeTest, TestParseWhileStatement) {
    string_tokenizer tokens("while (x) y;");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (statement: (while statement: (expression: (id: x), "
            "statement: (expression statement: (expression: (id: y))))))",
            node->to_str());
    delete node;
}

TEST(ParseTreeTest, TestParseIfAndCompoundStatement) {
    string_tokenizer tokens("if (x) {y; z;}");
    parser p(tokens);
    internal_node* node = p.parse();
    ASSERT_EQ("program: (statement: (if statement: (expression: (id: x), "
            "statement: (compound statement: (statement list: (statement: "
            "(expression statement: (expression: (id: y))), statement list: "
            "(statement: (expression statement: (expression: (id: z))), "
            "empty: ())))))))",
            node->to_str());
    delete node;
}
