#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"

#ifdef DEBUG
#define debug_print(M, ...) fprintf(stderr, "[DEBUG] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#else
#define debug_print(M, ...)
#endif

#define SIZE_STRING_INC 8
#define LINE_BUFF_LEN 128
#define MIN_LEN_KEYWORD 2
#define MAX_LEN_KEYWORD 10

#define error_exit(ERROR, M)                                                   \
    fprintf(stderr, "[ERROR:%d] (%s:%d): " M "\n", ERROR, __FILE__, __LINE__); \
    exit(ERROR);

typedef enum
{
    // identifikator, definicia, inicializacia
    TOKEN_IDENTIF,
    IDENTIF_DEF,
    IDENTIF_INIT,

    // literaly
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_STRING,

    // operatory
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_LESS,
    TOKEN_LESS_EQUAL,
    TOKEN_MORE,
    TOKEN_MORE_EQUAL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    // oddelovace
    TOKEN_COMMA,
    TOKEN_SEMICOLON,
    TOKEN_ROUND_LBRACKET,
    TOKEN_ROUND_RBRACKET,
    TOKEN_CURLY_LBRACKET,
    TOKEN_CURLY_RBRACKET,

    // keywordy
    KEYWORD_PACKAGE,
    KEYWORD_IF,
    KEYWORD_ELSE,
    KEYWORD_FOR,
    KEYWORD_FUNC,
    KEYWORD_INT,
    KEYWORD_FLOAT64,
    KEYWORD_STRING,
    KEYWORD_RETURN,

    // funkcie
    FUNC_MAIN,
    FUNC_INPUTS,
    FUNC_INPUTI,
    FUNC_INPUTF,
    FUNC_PRINT,
    FUNC_INT2FLOAT,
    FUNC_FLOAT2INT,
    FUNC_LEN,
    FUNC_SUBSTR,
    FUNC_ORD,
    FUNC_CHR,

    // stavy automatu
    STATE_START,
    STATE_COMMENTS,
    STATE_IDENTIF,
    STATE_STRING,
    STATE_NUMBER,
    STATE_FLOAT_NUMBER,
    STATE_EXPONENT_NUMBER,
    STATE_EXPONENT_NUMBER_FINAL

} Types;

typedef struct tToken
{
    int type;
    char *value;
} * TokenPtr;

typedef struct
{
    void **value;
    int top;
    int size;
} Stack;

typedef struct
{
    char *str;
    unsigned int length;
    unsigned int allocSize;
} string;

void initStack(Stack *s);
void *stackTop(Stack *s);
void stackPop(Stack *s);
void stackPush(Stack *s, void *p);
void stackFree(Stack *s);
void stackFlip(Stack *s);
void stackPrint(Stack *s, char *displayToken[]);

char *my_strdup(const char *str);
int lexer();
string *initBuffer();
void clearBuffer(string *buffer);
void freeBuffer(string *buffer);
TokenPtr initToken(int type, char *value);
string *addChar(string *buffer, char c);
void initString(string *s);
