#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include "error.h"

#ifdef DEBUG
    #define debug_print(M, ...)  fprintf(stderr, "[DEBUG] (%s:%d): " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
    #define stdout_print(M, ...) fprintf(stdout, "" M "\n", ##__VA_ARGS__)
#else
    #define debug_print(M, ...)
    #define stdout_print(M, ...)
#endif

#define UNUSED(x) (void)(x)
#define SIZE_STRING_INC 8
#define LINE_BUFF_LEN 128
#define MIN_LEN_KEYWORD 2
#define MAX_LEN_KEYWORD 10
#define LPTR 0
#define RPTR 1
#define FTsize 19
#define STsize 11

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
    TOKEN_GREATER,
    TOKEN_GREATER_EQUAL,
    TOKEN_EQUAL,
    TOKEN_NOT_EQUAL,

    // oddelovace
    TOKEN_EOL,
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

typedef enum
{
    SEQ,
    PACKAGE_MAIN,
    N_PARAMS_RETURNTYPES,
    N_DEF_FUNC,
    N_IDENTIFIER,
    N_PARAM_IDENT_INT,
    N_PARAM_IDENT_STR,
    N_PARAM_IDENT_FLOAT,
    N_LIT_INT,
    N_LIT_STRING,
    N_LIT_FLOAT,
    RETURN_TYPE_INT,
    RETURN_TYPE_STR,
    RETURN_TYPE_FLOAT,
    N_PRINT,
    N_INPUTI,
    N_INPUTS,
    N_INPUTF,
    N_IDENT_DEF,
    N_IDENT_INIT,
    N_FUNC,
    N_PLUS,
    N_MINUS,
    N_MULL,
    N_DIV,
    N_GREATER,
    N_LESS,
    N_EQ_GREATER,
    N_EQ_LESS,
    N_EQUAL,
    N_NOT_EQUAL,
    N_IF,
    N_ELSE,
    N_RETURN,
    N_LEN,
    N_SUBSTR,
    N_ORD,
    N_CHR,
    N_FOR,
    N_INT2FLOAT,
    N_FLOAT2INT,

} Nodes;

typedef struct
{
    int token;
    int precedence;
    int node;
} attrTable;
extern attrTable table[];

typedef struct Tree
{
    int type;
    char *value;
    struct Tree *Lptr;
    struct Tree *Rptr;
} Tree;

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

typedef char* tKey;

typedef struct SymTItem
{
    tKey key;
    int type;
    char *value;
    struct SymTItem *next;
} SymTItem;

typedef SymTItem *SymTable[STsize];

typedef struct FunTItem
{
    tKey key;
    int types;          // func example(int a, double b) (int, string) = 1312
    int retvar;         // number of return values
    int count;          // number of return values + params
    SymTable *sym;
    struct FunTItem *next;
} FunTItem;

typedef FunTItem *FunTable[FTsize];

void initStack(Stack *s);
void *stackTop(Stack *s);
void stackPop(Stack *s);
void stackPush(Stack *s, void *p);
void stackFree(Stack *s);
void stackFlip(Stack *s);
void stackPrint(Stack *s, char *displayToken[]);
int stackEmpty(Stack *s);

char *my_strdup(const char *str);
int lexer();
string *initBuffer();
void clearBuffer(string *buffer);
void freeBuffer(string *buffer);
TokenPtr initToken(int type, char *value);
string *addChar(string *buffer, char c);
void initString(string *s);
TokenPtr getToken(Stack *s);
void ungetToken(Stack *s);
void convertString(char* val);

int parser();
Tree *parse();
Tree *prolog();
Tree *program();
Tree *stmt();
Tree *stmt2();
Tree *stmt3();
Tree *stmt4_5();
Tree *params();
Tree *getRT();
Tree *rt();
Tree *rt2();
Tree *terms();
Tree *terms2();
Tree *expr(int precedence);
Tree *arg_coma();
void expectToken(int type);
void optionalEOL();
void checkForExcessEOL();

Tree *createNode(int type, Tree *Lptr, Tree *Rptr);
Tree *createLeaf(int type, char *value);

void prt_ast(Tree *t);
void Print_tree2(Tree *TempTree, char *sufix, char fromdir);
void Print_tree(Tree *TempTree);

int semantics();
void ftInit(FunTable *fun);
void stInit(SymTable *sym);
int hashCode(tKey key, int size);
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym);
void newSym(tKey key, int type, char *value, SymTable *sym);
FunTItem *ftSearch(FunTable *fun, tKey key);
SymTItem *stSearch(SymTable *sym, tKey key);


extern Stack stack;
extern Tree *ast;

void generate();
void generate_label(Tree *ast);
void generate_function(Tree *ast);
void generate_var_def(Tree *ast);
void generate_var_init(Tree *ast);
void generate_print(Tree *ast);