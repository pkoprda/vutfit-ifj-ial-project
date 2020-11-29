/*
 * IAL - hashtable
 * Header file for symtable.c 
 * Creator: Viliam Holik - xholik14
 */

#include "libmine.h"

// size of hashtable for funcions and symbols
#define FTsize 19
#define STsize 11

typedef char *tKey;

typedef struct SymTItem
{ // item in hashtable of symbols
    tKey key;
    int type; // int(1) string(2) double(3)
    char *value;
    struct SymTItem *next;
} SymTItem;

// hashtable for symbols
typedef SymTItem *SymTable[STsize];

typedef struct FunTItem
{ // item in hashtable of functions
    tKey key;
    int types;     // func example(int a, double b) (int, string) = 1312
    int retvar;    // number of return values
    int count;     // number of return values + params
    SymTable *sym; // ptr to hashtable of symbols
    struct FunTItem *next;
} FunTItem;

// hashtable for functions
typedef FunTItem *FunTable[FTsize];

void ftInit(FunTable *fun);
void stInit(SymTable *sym);
int hashCode(tKey key, int size);
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym);
void newSym(tKey key, int type, char *value, SymTable *sym);
FunTItem *ftSearch(FunTable *fun, tKey key);
SymTItem *stSearch(SymTable *sym, tKey key);