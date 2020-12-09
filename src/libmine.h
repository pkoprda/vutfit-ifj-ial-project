/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Rozhranie pre cely prekladac
 * Autori: Peter Koprda - xkoprd00, Daniel Paul - xpauld00,
 *         Viliam Holik - xholik14, Pavol Babjak - xbabja03 
 */


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
#define STACK_SIZE 128
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

    // klucove slova
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

// typ uzlu v abstraktnom syntaktickom strome
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

/* Štruktúra reprezentujúca precedenčnú tabuľku */
typedef struct
{
    /* Hodnota enumu tokenu */
    int token;
    /* Hodnota precedencie */
    int precedence;
    /* Hodnota enumu uzlu */
    int node;
} attrTable;
extern attrTable table[];

/* Štruktúra reprezentujúca uzol v abstraktnom syntaktickom strome */
typedef struct Tree
{
    /* Typ uzlu reprezentovaný enumom Nodes */
    int type;
    /* Hodnota uzlu */
    char *value;
    /* Pravý a ľavý pointer */
    struct Tree *Lptr;
    struct Tree *Rptr;
} Tree;

/* Štruktúra reprezentujúca token */
typedef struct tToken
{
    /* Typ tokenu reprezentovaný enumom types */
    int type;
    /* Hodnota tokenu (ak nejakú má) */
    char *value;
} * TokenPtr;

/* Štruktúra reprezentujúca zásobník tokenov */
typedef struct
{   
    /* Pole pointerov na tokeny*/
    void **value;
    /* Vrchol zásobníku */
    int top;
    /* Veľkosť zásobníku */
    int size;
} Stack;

/* Štruktúra reprezentujúca string */
typedef struct
{
    char *str;
    unsigned int length;
    unsigned int allocSize;
} string;

/* Funkcie na prácu s abstraktným dátovým typom - zásobníkom */

/* Inicializácia zásobníku */
void initStack(Stack *s);
/* Vrátenie tokenu na vrchole zásobníku */
void *stackTop(Stack *s);
/* Popnutie zo zásobníku */
void stackPop(Stack *s);
/* Uloženie tokenu na zásobník */
void stackPush(Stack *s, void *p);
/* Uvoľnenie z pamäte */
void stackFree(Stack *s);
/* Otočenie prvkov na zásobníku */
void stackFlip(Stack *s);
/* Skontorlovanie, či je zásobník prázdny */
int stackEmpty(Stack *s);

/* Deep copy stringu */
char *my_strdup(const char *str);
int lexer();
/* Funckie pre buffer */
string *initBuffer();
void clearBuffer(string *buffer);
void freeBuffer(string *buffer);

/* Vytvorenie tokenu*/
TokenPtr initToken(int type, char *value);
/* Zobranie tokenu z vrcholu zásobníka */
TokenPtr getToken(Stack *s);
/* Vrátenie tokenu na zásobník */
void ungetToken(Stack *s);

/* Funkcie na prácu so stringom */
string *addChar(string *buffer, char c);
void initString(string *s);
void convertString(char* val);

/* Funkcie pre pravidlá gramatiky v syntaktickej analýze */
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

/* Vytvorenie uzlu v abstraktnom syntaktickom strome */
Tree *createNode(int type, Tree *Lptr, Tree *Rptr);
/* Vytvorenie listu v abstraktnom syntaktickom strome */
Tree *createLeaf(int type, char *value);

void prt_ast(Tree *t);
void Print_tree2(Tree *TempTree, char *sufix, char fromdir);
void Print_tree(Tree *TempTree);

int semantics();

extern Stack stack;
extern Tree *ast;

void generate();
