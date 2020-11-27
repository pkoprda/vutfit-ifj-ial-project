#include "libmine.h"

int main()
{
    lexer();
    parser();
    semantics();
    generate_code();
    fprintf(stdout, "EXIT 0");
    return 0;
}