#include <gtest/gtest.h>
#include <gmock/gmock.h>
#ifndef TOKENIZER
#include "tokenizer.h"
#endif

void create_file(string filename, string contents) {
}

void delete_file(string filename) {
}
TEST(TokenizerTest, TestEmpty) {
    create_file("test", "");
    tokenizer t("test");
    ASSERT_BOOL_EQUALS(t.has_next_token, false);
}

int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
