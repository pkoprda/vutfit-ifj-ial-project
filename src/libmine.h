#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"
#include "lexer.h"

#ifdef DEBUG
#define debug_print(M, ...) fprintf(stderr,"[DEBUG] (%s:%d): " M "\n",__FILE__,__LINE__,##__VA_ARGS__)
#else
#define debug_print(M, ...)
#endif

#define error_exit(ERROR, M) \
    fprintf(stderr,"[ERROR:%d] (%s:%d): " M "\n", ERROR, __FILE__, __LINE__);   \
    exit(ERROR);


int lexer();