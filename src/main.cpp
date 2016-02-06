#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef TOKENIZER
#include "tokenizer.h"
#endif

using std::cout;
using std::endl;
int main(int argc, char** argv) {
    if (argc < 2) {
        cout << "Please enter a filename" << endl;
        return 1;
    }
    tokenizer t(argv[1]);
    while (t.has_next_token()) {
        cout << t.next_token().get_str() << endl;
    }
    return 0;
}
