#ifndef STRING
#include <string>
#endif

#ifndef TOKEN
#define TOKEN

using std::string;
const int num_allowed_types = 27;
const int num_keywords = 11;
extern const string keywords[];
extern const string allowed_types[];
class token {
    public:
        token(string type, string str, int line_num);
        string get_type();
        string get_str();
        int get_line_num();
        ~token();
    private:
        string type_;
        string str_;
        int line_num_;
};
#endif
