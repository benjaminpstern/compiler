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

string TEST_FILE_DIR = "test/testFiles/typeCheckerPrograms";
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

void assert_type_check_works(string filename) {
    tokenizer t(filename);
    parser p(t);
    internal_node* n = p.parse();
    type_check(n);
    ASSERT_TRUE(true);
    delete n;
}

void assert_type_check_fails(string filename, string message) {
    tokenizer t(filename);
    parser p(t);
    internal_node* n = p.parse();
    try {
        type_check(n);
        delete n;
        FAIL() << "expected std::range_error";
    }
    catch(std::range_error const & err) {
        EXPECT_EQ(err.what(), message);
    }
    catch(...) {
        FAIL() << "Expected std::range_error";
    }
}

TEST(TypeCheckerTest, TestTypeCheckSimple) {
    assert_type_check_works(TEST_FILE_DIR + "/simpleProgram.bpl");
}

TEST(TypeCheckerTest, TestTypeCheckCatchesUndefinedVariables) {
    string filename = TEST_FILE_DIR + "/undefinedVariable.bpl";
    string failure_message = "Error: no declaration for variable y on line 3\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestTypeCheckCatchesVariableOutOfScope) {
    string filename = TEST_FILE_DIR + "/variableOutOfScope.bpl";
    string failure_message = "Error: no declaration for variable y on line 6\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestTypeCheckImplicitlyCastsToFloat) {
    string filename = TEST_FILE_DIR + "/castToFloat.bpl";
    string failure_message = "Error: cannot resolve type float with expected type int on line 4\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestTypeCheckReturnNothing) {
    string filename = TEST_FILE_DIR + "/returnNothing.bpl";
    assert_type_check_works(filename);
}

TEST(TypeCheckerTest, TestTypeCatchesReturnWrongThing) {
    string filename = TEST_FILE_DIR + "/returnWrongThing.bpl";
    string failure_message = "Error: cannot resolve type string with expected type int on line 6\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestTypeCatchesFunctionReturnType) {
    string filename = TEST_FILE_DIR + "/wrongReturnType.bpl";
    string failure_message = "Error: cannot resolve type int with expected type float on line 6\n";
    assert_type_check_fails(filename, failure_message);
}
TEST(TypeCheckerTest, TestTypeConvertsPointer) {
    string filename = TEST_FILE_DIR + "/pointer.bpl";
    string failure_message = "Error: cannot resolve type int with expected type int* on line 5\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestGetsAddressOfParenExp) {
    string filename = TEST_FILE_DIR + "/addressOfParen.bpl";
    assert_type_check_works(filename);
}

TEST(TypeCheckerTest, TestDoesntGetAddressOfIntLiteral) {
    string filename = TEST_FILE_DIR + "/addressOf25.bpl";
    string failure_message = "Error: cannot get address of rvalue on line 3\n";
    assert_type_check_fails(filename, failure_message);
}

TEST(TypeCheckerTest, TestDoesntGetAddressOfRValue) {
    string filename = TEST_FILE_DIR + "/addressOfxPlus1.bpl";
    string failure_message = "Error: cannot get address of rvalue on line 4\n";
    assert_type_check_fails(filename, failure_message);
}
