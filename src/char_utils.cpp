#ifndef STRING
#include <string>
#endif

#ifndef CHAR_UTILS
#include "char_utils.h"
using std::string;
bool is_alphabetic(char c) {
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    if (c >= 'A' && c <= 'Z') {
        return true;
    }
    return false;
}

bool is_numeric(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}

bool is_punctuation(char c) {
    string punctuation = ";,[]{}()<>+-*/=%&!";
    for (string::size_type i = 0; i < punctuation.size(); ++i) {
        if (punctuation[i] == c) {
            return true;
        }
    }
    return false;
}

bool is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool is_relop(string s) {
    string relops[6] = {"<=", "<", "==", "!=", ">=", ">"};
    for (int i = 0; i < 6; ++i) {
        if (s == relops[i]) {
            return true;
        }
    }
    return false;
}

bool is_addop(string s) {
    string addops[2] = {"+", "-"};
    for (int i = 0; i < 2; ++i) {
        if (s == addops[i]) {
            return true;
        }
    }
    return false;
}

bool is_mulop(string s) {
    string mulops[3] = {"*", "/", "%"};
    for (int i = 0; i < 3; ++i) {
        if (s == mulops[i]) {
            return true;
        }
    }
    return false;
}
#endif
