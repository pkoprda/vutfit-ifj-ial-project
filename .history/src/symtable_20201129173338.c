/*
 * IAL - hastable
 * Creator: Viliam Holik - xholik14
 */

#include "symtable.h"

// function for scatter items in hastable
int hashCode(tKey key, int size)
{
    int retval = 1;
    int keylen = strlen(key);
    for (int i = 0; i < keylen; i++)
    {
        retval += key[i];
    }
    return (retval % size);
}

// initialization of hastable for functions
void ftInit(FunTable *fun)
{
    // each item in hashtable -> NULL
    for (int i = 0; i < FTsize; i++)
    {
        (*fun)[i] = NULL;
    }
}

// initialization of hastable for symbols
void stInit(SymTable *sym)
{
    // each item in hashtable -> NULL
    for (int i = 0; i < STsize; i++)
    {
        (*sym)[i] = NULL;
    }
}

// insert new function in hastable of functions
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym)
{
    // is there already item with same key?
    FunTItem *tmp = ftSearch(fun, key);
    // if yes ERROR
    if (tmp != NULL)
    {
        error_exit(3, "Function has been already defined");
    }

    FunTItem *new = malloc(sizeof(FunTItem));
    if (new == NULL)
    {
        error_exit(99, "Failed to allocate memory");
    }
    new->key = key;
    new->count = count;
    new->retvar = retvar;
    new->types = types;
    new->sym = sym;
    int hash = hashCode(key, FTsize);
    new->next = (*fun)[hash];
    (*fun)[hash] = new;
}

// search for function in hastable of functions
FunTItem *ftSearch(FunTable *fun, tKey key)
{
    int hash = hashCode(key, FTsize);
    FunTItem *tmp = (*fun)[hash];
    while (tmp != NULL)
    {
        if (!strcmp(tmp->key, key))
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

// insert new symbol in hastable of symbols
void newSym(tKey key, int type, char *value, SymTable *sym)
{
    // is there already item with same key?
    SymTItem *tmp = stSearch(sym, key);
    // if yes ERROR
    if (tmp != NULL)
    {
        error_exit(3, "Variable has been already defined");
    }

    SymTItem *new = malloc(sizeof(SymTItem));
    if (new == NULL)
    {
        error_exit(99, "Failed to allocate memory");
    }
    new->key = key;
    new->type = type;
    new->value = value;
    int hash = hashCode(key, STsize);
    new->next = (*sym)[hash];
    (*sym)[hash] = new;
}

// search for symbol in hastable of symbols
SymTItem *stSearch(SymTable *sym, tKey key)
{
    int hash = hashCode(key, STsize);
    SymTItem *tmp = (*sym)[hash];
    while (tmp != NULL)
    {
        if (!strcmp(tmp->key, key))
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}