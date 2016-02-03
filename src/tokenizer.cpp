#ifndef TOKENIZER
#include "tokenizer.h"
#endif

tokenizer::tokenizer(string filename) :
    file_stream_(filename),
    line_tokens_("")
{}

string tokenizer::next_token() {
    if (line_tokens_.has_next_token()) {
        return line_tokens_.next_token();
    }
    if (line_tokens_.has_next_token()) {
        return line_tokens_.next_token();
    }
    throw std::out_of_range("no next token");
}

bool tokenizer::has_next_token() {
    if (line_tokens_.has_next_token()) {
        return true;
    }
    while (!line_tokens_.has_next_token()) {
        if (file_stream_.good()) {
            string line;
            file_stream_ >> line;
            line_tokens_.new_string(line);
        }
        else {
            return false;
        }
    }
    return true;
}

line_tokenizer::line_tokenizer(string line) {
    line_ = line;
    position_ = 0;
}

string line_tokenizer::next_token() {
    string cur_token = "";
    char cur_char = line_[position_];
    while (is_whitespace(cur_char)) {
        ++position_;
        cur_char = line_[position_];
    }
    if (is_punctuation(cur_char)) {
        ++position_;
        cur_token += cur_char;
        if (is_operator_continuation(cur_char, "")) {
            cur_char = line_[position_];
            if (is_operator_continuation(cur_char, cur_token)) {
                cur_token += cur_char;
                ++position_;
                return cur_token;
            }
        }
        return cur_token;
    }
    if (is_variable_char(cur_char)) {
        if (!is_numeric(cur_char)) {
            while (is_variable_char(cur_char)) {
                cur_token += cur_char;
                ++position_;
                cur_char = line_[position_];
            }
            return cur_token;
        }
        while (is_numeric(cur_char)) {
            cur_token += cur_char;
            ++position_;
            cur_char = line_[position_];
        }
        return cur_token;
    }
    throw std::range_error("illegal character");
}

bool line_tokenizer::has_next_token() {
    char cur_char = line_[position_];
    while (is_whitespace(cur_char)) {
        ++position_;
        cur_char = line_[position_];
    }
    return position_ < line_.size();
}

void line_tokenizer::new_string(string line) {
    line_ = line;
    position_ = 0;
}

bool line_tokenizer::is_punctuation(char c) {
    string punctuation = ";,[]{}()<>+-*/=%&.";
    for (int i = 0; i < punctuation.size(); ++i) {
        if (punctuation[i] == c) {
            return true;
        }
    }
    return false;
}

bool line_tokenizer::is_operator_continuation(char c, string cur_token) {
    string operators[6] = {"<=", "==", "!=", ">=", "/*", "*/"};
    if (cur_token.size() == 0) {
        for (int i = 0; i < 6; ++i) {
            if (operators[i][0] == c) {
                return true;
            }
        }
    }
    else {
        for (int i = 0; i < 6; ++i) {
            if (operators[i][0] == cur_token[0]) {
                if (operators[i][1] == c) {
                    return true;
                }
            }
        }
    }
    return false;
}

bool line_tokenizer::is_whitespace(char c) {
    return c == ' ' || c == '\t' || c == '\n';
}

bool line_tokenizer::is_alphabetic(char c) {
    if (c >= 'a' && c <= 'z') {
        return true;
    }
    if (c >= 'A' && c <= 'Z') {
        return true;
    }
    return false;
}

bool line_tokenizer::is_numeric(char c) {
    if (c >= '0' && c <= '9') {
        return true;
    }
    return false;
}
bool line_tokenizer::is_variable_char(char c) {
    if (is_alphabetic(c)) {
        return true;
    }
    if (is_numeric(c)) {
        return true;
    }
    if (c == '_') {
        return true;
    }
    return false;
}
