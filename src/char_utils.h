#ifndef STRING
#include <string>
#endif
#ifndef CHAR_UTILS
#define CHAR_UTILS
bool is_alphabetic(char c);
bool is_numeric(char c);
bool is_punctuation(char c);
bool is_whitespace(char c);
bool is_relop(std::string s);
bool is_addop(std::string s);
bool is_mulop(std::string s);
#endif
