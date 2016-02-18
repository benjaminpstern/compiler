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
    parse_tree_node node("stuff", 0);
    string node_str = node.to_str();
    ASSERT_EQ("<stuff, ()>", node_str);
}

TEST(ParseTreeTest, TestPrintNodeWithChildren) {
    parse_tree_node node("stuff", 2);
    parse_tree_node left_child("left", 0);
    parse_tree_node right_child("right", 0);
    node.set_child(0, left_child);
    node.set_child(1, right_child);
    string node_str = node.to_str();
    ASSERT_EQ("<stuff, (<left, ()><right, ()>)>", node_str);
}

TEST(ParseTreeTest, TestPrintIndentedNodeWithChildren) {
    parse_tree_node node("stuff", 2);
    parse_tree_node left_child("left", 0);
    parse_tree_node right_child("right", 0);
    node.set_child(0, left_child);
    node.set_child(1, right_child);
    string node_str = node.to_indented_str();
    ASSERT_EQ("<stuff, (\n\t<left, ()>\n\t<right, ()>\n)>", node_str);
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
    string_tokenizer tokens("This is a tokenizer");
    string token_strings[4] = {"This", "is", "a", "tokenizer"};
    for (int i = 0; i < 4; ++i) {
        ASSERT_TRUE(tokens.has_next_token());
        token tok = tokens.next_token();
        ASSERT_EQ(token_strings[i], tok.get_str());
    }
    ASSERT_FALSE(tokens.has_next_token());
}

TEST(ParseTreeTest, TestParseSingleStatement) {
    string_tokenizer tokens("x;");
    parser p(tokens);
    parse_tree_node node = p.parse();
    ASSERT_EQ("<program, (<statement, (<expression statement, "
            "(<expression, (<x, ()>)>)>)>)>", node.to_str());
}

TEST(ParseTreeTest, TestParseCompoundStatement) {
    string_tokenizer tokens("{x; y; z;}");
    parser p(tokens);
    parse_tree_node node = p.parse();
    ASSERT_EQ("<program, (<statement, (<compound statement, "
            "(<statement list, (<statement, (<expression statement, "
            "(<expression, (<x, ()>)>)>)><statement list, (<statement, "
            "(<expression statement, (<expression, (<y, ()>)>)>)>"
            "<statement list, (<statement, (<expression statement, "
            "(<expression, (<z, ()>)>)>)><empty, ()>)>)>)>)>)>)>",
            node.to_str());
}
