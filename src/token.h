#ifndef STRING
#include <string>
#endif

#ifndef TOKEN
#define TOKEN

const int num_allowed_types = 25;
using std::string;
class token {
    public:
        token(string type, string str, int line_num);
        string get_type();
        string get_str();
        int get_line_num();
    private:
        string type_;
        string str_;
        int line_num_;
        const string allowed_types[num_allowed_types] = {
            "int", "float", "string", "identifier", "(", ")", "{",
            "}", "[", "]", ";", "<=", ">=", ",", "!=", ">", "<", "==", "=",
            "+", "-", "*", "/", "%", "&"};
};
#endif
