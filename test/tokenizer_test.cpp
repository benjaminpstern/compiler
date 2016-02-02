#include <gtest/gtest.h>
#include <gmock/gmock.h>
#ifndef TOKENIZER
#include "tokenizer.h"
#endif

void assert_line_tokenizer_output(
        string line_tokens,
        string expected [],
        int expected_size) {
    line_tokenizer t(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token(), expected[i]);
    }
    ASSERT_FALSE(t.has_next_token());
}
void create_file(string filename, string contents) {
}

void delete_file(string filename) {
}
TEST(TokenizerTest, TestEmpty) {
    string expected [0] = { };
    assert_line_tokenizer_output("", expected, 0);
}
TEST(TokenizerTest, TestFirstElement) {
    string expected [1] = {"one"};
    assert_line_tokenizer_output("one", expected, 1);
}
TEST(TokenizerTest, TestTwoElements) {
    string expected [2] = {"one", "two"};
    assert_line_tokenizer_output("one two", expected, 2);
}
TEST(TokenizerTest, TestTokenizeParens) {
    string expected [3] = {"(", "one", ")"};
    assert_line_tokenizer_output("(one)", expected, 3);
}

TEST(TokenizerTest, TestTokenizeAlphanumeric) {
    string expected [2] = {"one123", "two234"};
    assert_line_tokenizer_output("one123 two234", expected, 2);
}

TEST(TokenizerTest, TestTokenizeOperator) {
    string expected [3] = {"var", "<=", "5"};
    assert_line_tokenizer_output("var<=5", expected, 3);
}

TEST(TokenizerTest, TestDoesntTokenizeWrongOperators) {
    string expected [4] = {"var", "<", "/", "5"};
    assert_line_tokenizer_output("var</5", expected, 4);
}

TEST(TokenizerTest, TestWhitespaceAtEndOfLine) {
    string expected [4] = {"var", "<", "/", "5"};
    assert_line_tokenizer_output("var</5 ", expected, 4);
}

TEST(TokenizerTest, TestNewString) {
    string line_tokens = "this is one line";
    string expected [4] = {"this", "is", "one", "line"};
    int expected_size = 4;
    line_tokenizer t(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token(), expected[i]);
    }
    ASSERT_FALSE(t.has_next_token());
    line_tokens = "this is another line";
    string expected2 [4] = {"this", "is", "another", "line"};
    expected_size = 4;
    t.new_string(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token(), expected2[i]);
    }
    ASSERT_FALSE(t.has_next_token());
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
