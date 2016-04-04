#ifndef GTEST
#include <gtest/gtest.h>
#endif
#ifndef GMOCK
#include <gmock/gmock.h>
#endif
#ifndef IOSTREAM
#include <iostream>
#endif
#ifndef FSTREAM
#include <fstream>
#endif
#ifndef TYPE_CHECKER
#include "type_checker.h"
#endif

TEST(TypeCheckerTest, TestCloneMap) {
    std::map<string, parse_tree_node*> m;
    internal_node x("five", 0, 0);
    internal_node y("six", 0, 0);
    m["one"] = &x;
    m["two"] = &y;
    std::map<string, parse_tree_node*> n;
    clone(n, m);
    ASSERT_EQ(m["one"], n["one"]);
    ASSERT_EQ(m["two"], n["two"]);
}
