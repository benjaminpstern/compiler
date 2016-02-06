#ifndef TOKENIZER
#include "tokenizer.h"
#endif

#ifndef IOSTREAM
#include <iostream>
#endif

tokenizer::tokenizer(string filename) :
    file_stream_(filename),
    line_tokens_("")
{
    line_num_ = 0;
}

token tokenizer::next_token() {
    if (line_tokens_.has_next_token()) {
        return identify_token(line_tokens_.next_token_str());
    }
    if (line_tokens_.has_next_token()) {
        return identify_token(line_tokens_.next_token_str());
    }
    throw std::out_of_range("no next token");
}

bool tokenizer::has_next_token() {
    if (line_tokens_.has_next_token()) {
        return true;
    }
    while (!line_tokens_.has_next_token()) {
        if (file_stream_.good()) {
            std::getline(file_stream_, line_);
            while (remove_comments()){
                ;
            }
            line_tokens_.add_string(line_);
            ++line_num_;
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

string line_tokenizer::next_token_str() {
    string cur_token = "";
    char cur_char = line_[position_];
    while (is_whitespace(cur_char)) {
        ++position_;
        cur_char = line_[position_];
    }
    if (cur_char == '"') {
        cur_token += cur_char;
        ++position_;
        bool done = false;
        while (position_ < line_.size() && !done) {
            cur_char = line_[position_];
            cur_token += cur_char;
            if (cur_char == '"') {
                done = true;
            }
            ++position_;
        }
        if (cur_char != '"') {
            throw std::range_error("No matching \"");
        }
        return cur_token;
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
        int num_periods = 0;
        while (is_numeric(cur_char) || (cur_char == '.' && num_periods < 1)) {
            cur_token += cur_char;
            ++position_;
            if (cur_char == '.') {
                ++num_periods;
            }
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

void line_tokenizer::add_string(string line) {
    line_ = line_.substr(position_);
    line_ += line;
    position_ = 0;
}


bool line_tokenizer::is_operator_continuation(char c, string cur_token) {
    string operators[4] = {"<=", "==", "!=", ">="};
    if (cur_token.size() == 0) {
        for (int i = 0; i < 4; ++i) {
            if (operators[i][0] == c) {
                return true;
            }
        }
    }
    else {
        for (int i = 0; i < 4; ++i) {
            if (operators[i][0] == cur_token[0]) {
                if (operators[i][1] == c) {
                    return true;
                }
            }
        }
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

bool tokenizer::remove_comments() {
    string::size_type comment_pos = line_.find("/*");
    if (comment_pos == string::npos) {
        return false;
    }

    string::size_type comment_end_pos = line_.find("*/", comment_pos + 2);
    if (comment_end_pos == string::npos) {
        line_ = line_.substr(0, comment_pos);
        line_tokens_.add_string(line_);
        end_comment();
    }
    else {
        string newline = line_.substr(0, comment_pos);
        if (comment_end_pos + 1 < line_.size()) {
            newline += line_.substr(comment_end_pos + 2);
            line_ = newline;
        }
    }
    return true;
}
void tokenizer::end_comment() {
    while (line_.find("*/") == string::npos) {
        if (!file_stream_.good()) {
            throw std::range_error("No matching */");
        }
        std::getline(file_stream_, line_);
    }
    line_ = line_.substr(line_.find("*/") + 2);
}

token tokenizer::identify_token(string str) {
    string type;
    char first_char = str[0];
    if (first_char == '"') {
        type = "string";
    }
    if (is_alphabetic(first_char)) {
        type = "identifier";
    }
    if (is_numeric(first_char)) {
        if (str.find('.') != string::npos) {
            type = "float";
        }
        else {
            type = "int";
        }
    }
    if (is_punctuation(first_char)) {
        type = str;
    }
    token return_token(type, str, line_num_);
    return return_token;
}
