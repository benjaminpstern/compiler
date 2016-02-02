#include "sample.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

using ::testing::Return;
TEST(SampleTest, TestSample) {
    sample s;
    s.variable = 5;
    ASSERT_FLOAT_EQ(s.func(4), 9.1);
}
int main(int argc, char **argv) {
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
