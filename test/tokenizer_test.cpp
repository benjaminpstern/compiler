#ifndef GTEST
#include <gtest/gtest.h>
#endif
#ifndef GMOCK
#include <gmock/gmock.h>
#endif
#ifndef TOKENIZER
#include "tokenizer.h"
#endif
#ifndef TOKEN
#include "token.h"
#endif
#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef FSTREAM
#include <fstream>
#endif

void assert_line_tokenizer_output(
        string line_tokens,
        string expected [],
        int expected_size) {
    line_tokenizer t(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token_str(), expected[i]);
    }
    ASSERT_FALSE(t.has_next_token());
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

TEST(TokenizerTest, TestTokenizeFloats) {
    string expected [3] = {"10.23", ".2"};
    assert_line_tokenizer_output("10.23.2", expected, 2);
}

TEST(TokenizerTest, TestDoesntTokenizeWrongOperators) {
    string expected [4] = {"var", "<", "/", "5"};
    assert_line_tokenizer_output("var</5", expected, 4);
}

TEST(TokenizerTest, TestDoesntTokenizeNotEqualOperator) {
    string expected [3] = {"var", "!=", "5"};
    assert_line_tokenizer_output("var != 5", expected, 3);
}
TEST(TokenizerTest, TestTokenizeStrings) {
    string expected [2] = {"\"one two three\"", "four"};
    assert_line_tokenizer_output("\"one two three\" four", expected, 2);
}

TEST(TokenizerTest, TestUnmatchedQuoteException) {
    line_tokenizer t("one \"two three");
    ASSERT_EQ(t.next_token_str(), "one");
    try {
        ASSERT_TRUE(t.has_next_token());
        t.next_token_str();
        FAIL() << "expected std::range_error";
    }
    catch(std::range_error const & err) {
        EXPECT_EQ(err.what(),std::string("No matching \""));
    }
    catch(...) {
        FAIL() << "Expected std::range_error";
    }
}

TEST(TokenizerTest, TestAllowsVariablesWithUnderscores) {
    string expected [2] = {"a_b", "c_d"};
    assert_line_tokenizer_output("a_b c_d", expected, 2);
}

TEST(TokenizerTest, TestDoesntAllowVariablesStartingWithNumbers) {
    string expected [4] = {"5", "var", "<", "5"};
    assert_line_tokenizer_output("5var<5", expected, 4);
}

TEST(TokenizerTest, TestWhitespaceAtEndOfLine) {
    string expected [4] = {"var", "<", "/", "5"};
    assert_line_tokenizer_output("var</5 ", expected, 4);
}

TEST(TokenizerTest, TestAllowsFloatWithLeadingPeriod) {
    string expected [1] = {".050"};
    assert_line_tokenizer_output(".050", expected, 1);
}

TEST(TokenizerTest, TestNewString) {
    string line_tokens = "this is one line";
    string expected [4] = {"this", "is", "one", "line"};
    int expected_size = 4;
    line_tokenizer t(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token_str(), expected[i]);
    }
    ASSERT_FALSE(t.has_next_token());
    line_tokens = "this is another line";
    string expected2 [4] = {"this", "is", "another", "line"};
    expected_size = 4;
    t.add_string(line_tokens);
    for (int i = 0; i < expected_size; ++i) {
        ASSERT_EQ(t.next_token_str(), expected2[i]);
    }
    ASSERT_FALSE(t.has_next_token());
}

void testTokenizeFile(string filename, string tokenizedFileName) {
    ifstream expected;
    expected.open(tokenizedFileName);
    string line;
    tokenizer t(filename);
    ASSERT_TRUE(expected.is_open());
    while (expected >> line) {
        ASSERT_TRUE(t.has_next_token());
        ASSERT_EQ(t.next_token().get_str(), line);
    }
    ASSERT_FALSE(t.has_next_token());
    ASSERT_FALSE(expected.good());
}

TEST(TokenizerTest, TestPeek) {
    tokenizer t("test/testFiles/program.txt");
    ASSERT_TRUE(t.has_next_token());
    ASSERT_EQ("int", t.peek_token().get_str());
    ASSERT_EQ("int", t.peek_token().get_str());
    ASSERT_EQ("int", t.next_token().get_str());
    ASSERT_EQ("factorial", t.peek_token().get_str());

}

TEST(TokenizerTest, TestSingleComment) {
    testTokenizeFile(
            "test/testFiles/programWithOneLineComment.txt",
            "test/testFiles/tokenizedProgram.txt"
            );
}

TEST(TokenizerTest, TestMultiLineComment) {
    testTokenizeFile(
            "test/testFiles/programWithMultiLineComment.txt",
            "test/testFiles/tokenizedProgram.txt"
            );
}

TEST(TokenizerTest, TestTrickyComment) {
    testTokenizeFile(
            "test/testFiles/programWithTrickyComment.txt",
            "test/testFiles/tokenizedProgram.txt"
            );
}

TEST(TokenizerTest, TestTwoCommentsInOneLine) {
    testTokenizeFile(
            "test/testFiles/programWithTwoCommentsInALine.txt",
            "test/testFiles/tokenizedProgram.txt"
            );
}

TEST(TokenizerTest, TestUnmatchedComment) {
    try {
        tokenizer t("test/testFiles/programWithUnmatchedComment.txt");
        ASSERT_TRUE(t.has_next_token());
        t.next_token();
        t.next_token();
        FAIL() << "expected std::range_error";
    }
    catch(std::range_error const & err) {
        EXPECT_EQ(err.what(),std::string("No matching */"));
    }
    catch(...) {
        FAIL() << "Expected std::range_error";
    }
}
