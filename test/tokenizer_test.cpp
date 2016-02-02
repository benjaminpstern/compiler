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

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
