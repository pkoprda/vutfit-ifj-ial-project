#include "code_gen.h"


#define PRINT_HEADER()          \
    PRINT_CODE(".IFJcode20")    \
    PRINT_NL()

void generate_code()
{
    PRINT_HEADER();
    generate_main();

    PRINT_CODE("JUMP $$main");
}

void generate_main()
{
    PRINT_CODE("LABEL $$main");
    PRINT_CODE("CREATEFRAME");
}