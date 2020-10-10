#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include "error.h"

#ifdef DEBUG
#define debug_print(M, ...) fprintf(stderr,"[DEBUG] (%s:%d): " M "\n",__FILE__,__LINE__,##__VA_ARGS__)
#else
#define debug_print(M, ...)
#endif

#define LINE_BUFF_LEN 128

#define error_exit(ERROR, M) \
    fprintf(stderr,"[ERROR:%d] (%s:%d): " M "\n", ERROR, __FILE__, __LINE__);   \
    exit(ERROR);

typedef enum {

    // stavy automatu
    START, COMMENTS, IDENTIF, NUMBER, STRING, OPERATORS,

    // identifikator
    IDENTIF_DEF, IDENTIF_INIT,
    
    // +    -     *    /    <        <=       >      >=         ==       !=
    PLUS, MINUS, MUL, DIV, LESS, LESS_EQUAL, MORE, MORE_EQUAL, EQUAL, NOT_EQUAL,

    // ,       ;           (               )               {                }  
    COMMA, SEMICOLON, ROUND_LBRACKET, ROUND_RBRACKET, CURLY_LBRACKET, CURLY_RBRACKET,
    
    // package           if          else         for           func
    KEYWORD_PACKAGE, KEYWORD_IF, KEYWORD_ELSE, KEYWORD_FOR, KEYWORD_FUNC,
    
    // int           float64          string           return
    KEYWORD_INT, KEYWORD_FLOAT64, KEYWORD_STRING, KEYWORD_RETURN
} Types;

typedef struct tToken {
    int type;
    char* value;
} *TokenPtr;

int lexer();
char* initBuffer();
void freeBuffer(char* buffer);
TokenPtr initToken(int type, char* value);
