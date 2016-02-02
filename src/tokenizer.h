#ifndef SSTREAM
#include <sstream>
#endif

#ifndef FSTREAM
#include <fstream>
#endif

#ifndef STRING
#include <string>
#endif

#ifndef TOKENIZER
#define TOKENIZER
using std::string;
using std::stringstream;
using std::fstream;
class tokenizer {
    public:
        tokenizer(string filename);
        string next_token();
        bool has_next_token();
    private:
        bool is_legal_char(char c);
        fstream file_stream;

};
#endif
