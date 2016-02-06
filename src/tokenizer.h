#ifndef SSTREAM
#include <sstream>
#endif

#ifndef FSTREAM
#include <fstream>
#endif

#ifndef STRING
#include <string>
#endif

#ifndef STDEXCEPT
#include <stdexcept>
#endif

#ifndef TOKEN
#include "token.h"
#endif

#ifndef CHAR_UTILS
#include "char_utils.h"
#endif

#ifndef TOKENIZER
#define TOKENIZER
using std::string;
using std::stringstream;
using std::ifstream;

class line_tokenizer {
    public:
        line_tokenizer(string line);
        string next_token_str();
        bool has_next_token();
        void add_string(string line);
    private:
        bool is_variable_char(char c);
        bool is_operator_continuation(char c, string cur_token);
        string line_;
        int position_;
};


class tokenizer {
    public:
        tokenizer(string filename);
        token next_token();
        bool has_next_token();
    private:
        ifstream file_stream_;
        line_tokenizer line_tokens_; token identify_token(string str);
        int line_num_;
        string line_;
        bool remove_comments(); // returns true if it removed something
        void end_comment();
};
#endif
