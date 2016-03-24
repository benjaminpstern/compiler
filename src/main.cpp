#ifndef IOSTREAM
#include <iostream>
#endif

#ifndef IOMANIP
#include <iomanip>
#endif

#ifndef TOKENIZER
#include "tokenizer.h"
#endif

#ifndef PARSER
#include "parser.h"
#endif

#ifndef STDEXCEPT
#include <stdexcept>
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
    parser p(t);
    try {
        internal_node* node = p.parse();
        cout << node->to_indented_str() << endl;
        delete node;
    }
    catch (std::range_error& e) {
        cout << e.what() << endl;
    }
    return 0;
}
