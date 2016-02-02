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

#ifndef TOKENIZER
#define TOKENIZER
using std::string;
using std::stringstream;
using std::ifstream;

class line_tokenizer {
    public:
        line_tokenizer(string line);
        string next_token();
        bool has_next_token();
        // new string replaces the old string.
        // therefore you should verify that has_next_token returns false
        void new_string(string line);
    private:
        bool is_punctuation(char c);
        bool is_whitespace(char c);
        bool is_variable_char(char c);
        bool is_operator_continuation(char c, string cur_token);
        string line_;
        int position_;
};

class tokenizer {
    public:
        tokenizer(string filename);
        string next_token();
        bool has_next_token();
    private:
        ifstream file_stream_;
        line_tokenizer line_tokens_;
};
#endif
