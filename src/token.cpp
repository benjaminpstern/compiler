#ifndef TOKEN
#include "token.h"
#endif

#ifndef STDEXCEPT
#include <stdexcept>
#endif
token::token(string type, string str, int line_num) {
    bool found = false;
    for (int i = 0; i < num_allowed_types && !found; ++i) {
        if (type == allowed_types[i]) {
            found = true;
        }
    }
    if (!found) {
        throw std::range_error("token type does not exist");
    }

    type_ = type;
    str_ = str;
    line_num_ = line_num;
}

string token::get_type() { return type_; }
string token::get_str() { return str_; }
int token::get_line_num() { return line_num_; }
