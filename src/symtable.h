/*
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: IAL - tabulka s rozptylenymi polozkami, hlavickovy subor pre symtable.c 
 * Autori: Viliam Holik - xholik14, Pavol Babjak - xbabja03
 */

#include "libmine.h"

// velkost tabuliek
#define FTsize 19
#define STsize 11

typedef char *tKey;

// struktura pre jednotlive symboly
typedef struct SymTItem
{
    tKey key;
    int type; // int(1) string(2) double(3)
    char *value;
    int hide; // zanorenie
    int forcnt;
    int ifcnt;
    struct SymTItem *down; // pre symboly s rovnakym menom ale v inych scopes
    struct SymTItem *next;
} SymTItem;

// hashtable pre symboly
typedef SymTItem *SymTable[STsize];

// struktura pre jednotlive funkcie
typedef struct FunTItem
{
    tKey key;
    int types;     // func priklad(int a, double b) (int, string) = 1312
    int retvar;    // pocet return hodnot
    int count;     // pocet return hodnot + parametre
    SymTable *sym; // ukazatel na hashtable pre symboly
    struct FunTItem *next;
} FunTItem;

// hashtable pre funkcie
typedef FunTItem *FunTable[FTsize];

void ftInit(FunTable *fun);
void stInit(SymTable *sym);
int hashCode(tKey key, int size);
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym);
void newSym(tKey key, int type, char *value, int hide, int forcnt, int ifcnt, SymTable *sym);
FunTItem *ftSearch(FunTable *fun, tKey key);
SymTItem *stSearch(SymTable *sym, tKey key);
SymTItem *searchdown(SymTItem *sym, int hide, int forcnt, int ifcnt);
SymTItem *searchforID(SymTItem *sym, int hide, int forcnt, int ifcnt);

extern FunTable *ft;