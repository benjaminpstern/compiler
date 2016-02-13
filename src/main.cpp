#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef IOMANIP
#include <iomanip>
#endif

#ifndef TOKENIZER
#include "tokenizer.h"
#endif

using std::cout;
using std::endl;
using std::setw;
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Please enter a filename" << endl;
        return 1;
    }
    tokenizer t(argv[1]);
    while (t.has_next_token()) {
        token tok = t.next_token();
        cout << "token string:" << setw(15) << tok.get_str() << setw(15)
            << "token type:" << setw(20) << tok.get_type() << setw(15) 
            << "line num:" << setw(4) << tok.get_line_num() << endl;
    }
    return 0;
}
