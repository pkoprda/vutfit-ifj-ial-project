#include "libmine.h"


#define PRINT_CODE(M, ...)                      \
    fprintf(stdout, "" M "\n", ##__VA_ARGS__);
    
#define PRINT_CODE_WITHOUT_NL(M, ...)           \
    fprintf(stdout, "" M "", ##__VA_ARGS__);
    
#define PRINT_NL()                              \
    fprintf(stdout, "\n");


