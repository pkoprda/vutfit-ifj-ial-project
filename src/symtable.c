/*
 * IAL - hashtable
 * Creator: Viliam Holik - xholik14, Pavol Babjak - xbabja03
 */

#include "symtable.h"

// function for scatter items in hashtable
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

// initialization of hashtable for functions
void ftInit(FunTable *fun)
{
    // each item in hashtable -> NULL
    for (int i = 0; i < FTsize; i++)
    {
        (*fun)[i] = NULL;
    }
}

// initialization of hashtable for symbols
void stInit(SymTable *sym)
{
    // each item in hashtable -> NULL
    for (int i = 0; i < STsize; i++)
    {
        (*sym)[i] = NULL;
    }
}

// insert new function in hashtable of functions
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym)
{
    // is there already item with same key?
    FunTItem *tmp = ftSearch(fun, key);
    // if yes ERROR
    if (tmp != NULL)
    {
        error_exit(SEM_ERROR_UNDEF, "Function has been already defined");
    }

    FunTItem *new = malloc(sizeof(FunTItem));
    if (new == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
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

// search for function in hashtable of functions
FunTItem *ftSearch(FunTable *fun, tKey key)
{
    int hash = hashCode(key, FTsize);
    FunTItem *tmp = (*fun)[hash];
    while (tmp != NULL)
    {
        if (strcmp(tmp->key, key) == 0)
        {
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL;
}

// insert new symbol in hashtable of symbols
void newSym(tKey key, int type, char *value, int hide, int forcnt, int ifcnt, SymTable *sym)
{
    // is there already item with same key?
    SymTItem *tmp = stSearch(sym, key);
    // if yes ERROR
    if (tmp != NULL)
    {
        SymTItem *tmp2 = searchdown(tmp, hide, forcnt, ifcnt);
        if (tmp2 != NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable has been already defined");
        }
    }

    SymTItem *new = malloc(sizeof(SymTItem));
    if (new == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }
    new->key = key;
    new->type = type;
    new->value = value;
    new->hide = hide;
    new->ifcnt = ifcnt;
    new->forcnt = forcnt;
    if (tmp == NULL)
    {
        int hash = hashCode(key, STsize);
        new->down = NULL;
        new->next = (*sym)[hash];
        (*sym)[hash] = new;
        return;
    }
    // must go down in sItem
    do
    {
        if (tmp->down == NULL)
        {
            break;
        }
        tmp = tmp->down;
    } while (tmp != NULL);
    tmp->down = new;
    new->down = NULL;
    new->next = NULL;
}

// search for symbol in hashtable of symbols
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

// search items in scopes
SymTItem *searchdown(SymTItem *sym, int hide, int forcnt, int ifcnt)
{
    SymTItem *tmp = sym;
    while (tmp != NULL)
    {
        if (tmp->forcnt == forcnt && tmp->ifcnt == ifcnt && tmp->hide == hide)
        {
            return tmp;
        }
        // printf("hore-%p\n", tmp);
        tmp = tmp->down;
        // printf("dole-%p\n", tmp);
    }
    // printf("skoncil\n");
    return NULL;
}

SymTItem *searchforID(SymTItem *sym, int hide, int forcnt, int ifcnt)
{
    SymTItem *tmp = sym;
    while (tmp != NULL)
    {
        if (tmp->forcnt == forcnt && tmp->ifcnt == ifcnt && tmp->hide == hide)
        {
            return tmp;
        }
        tmp = tmp->down;
    }
    int i;
    int n = 1;
    while (hide >= n)
    {
        i = 1;
        while (i <= ifcnt || i <= forcnt)
        {
            tmp = sym;
            while (tmp != NULL)
            {
                if (tmp->forcnt == forcnt-i || tmp->ifcnt == ifcnt-i)
                {
                    if (tmp->hide == hide-n)
                    {
                        return tmp;
                    }
                }
                tmp = tmp->down;
            }
            i++;
        }
        n++;
    }
    return NULL;
}