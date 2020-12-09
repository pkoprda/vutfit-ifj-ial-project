/*
 * IAL - hashtable
 * Autori: Viliam Holik - xholik14, Pavol Babjak - xbabja03
 */

#include "symtable.h"

// funkcia na rozptylovanie po tabulke
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

// inicializacia hashtable pre funkcie
void ftInit(FunTable *fun)
{
    // kazdy prvok nastavime na NULL
    for (int i = 0; i < FTsize; i++)
    {
        (*fun)[i] = NULL;
    }
}

// inicializacia hashtable pre symboly
void stInit(SymTable *sym)
{
    // kazdy prvok nastavime na NULL
    for (int i = 0; i < STsize; i++)
    {
        (*sym)[i] = NULL;
    }
}

// vlozenie novej funkcie do hashtable pre funkcie
void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym)
{
    // je tam uz funkcia s rovnakym menom?
    FunTItem *tmp = ftSearch(fun, key);
    // ak ano -> ERROR
    if (tmp != NULL)
    {
        error_exit(SEM_ERROR_UNDEF, "Function has been already defined");
    }

    // alokacia pamate pre novu funkciu
    FunTItem *new = malloc(sizeof(FunTItem));
    if (new == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }

    // nastavenie prvok struktury
    new->key = key;
    new->count = count;
    new->retvar = retvar;
    new->types = types;
    new->sym = sym;

    // vlozenie do tabulky na spravne miesto
    int hash = hashCode(key, FTsize);
    new->next = (*fun)[hash];
    (*fun)[hash] = new;
}

// hladanie funkcie na zaklade nazvu v hashtable pre funkcie
FunTItem *ftSearch(FunTable *fun, tKey key)
{
    int hash = hashCode(key, FTsize);
    FunTItem *tmp = (*fun)[hash];
    while (tmp != NULL)
    {
        // je nazov funkcie totozny s hladanou?
        if (strcmp(tmp->key, key) == 0)
        {
            // vrati danu funkciu
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // ak sa nenasla taka funkcia vrati NULL
}

// vlozenie noveho symbolu do hashtable pre symboly
void newSym(tKey key, int type, char *value, int hide, int forcnt, int ifcnt, SymTable *sym)
{
    // je tam uz symbol s rovnakym nazvom?
    SymTItem *tmp = stSearch(sym, key);
    if (tmp != NULL)
    {
        // je tam uz symbol s rovnakymi atributami?
        SymTItem *tmp2 = searchdown(tmp, hide, forcnt, ifcnt);
        // ak ano -> ERROR
        if (tmp2 != NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable has been already defined");
        }
    }

    // alokacia pamate pre novy symbol
    SymTItem *new = malloc(sizeof(SymTItem));
    if (new == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }

    // nastavenie prvok struktury
    new->key = key;
    new->type = type;
    new->value = value;
    new->hide = hide;
    new->ifcnt = ifcnt;
    new->forcnt = forcnt;

    // ak nie je symbol s hladanym nazvom v tabulke
    if (tmp == NULL)
    {
        int hash = hashCode(key, STsize);
        new->down = NULL;
        new->next = (*sym)[hash];
        (*sym)[hash] = new;
        return;
    }

    // musi sa ist dole pri konkretnom symbole
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

// hladanie symbolu na zaklade nazvu v hashtable pre symboly
SymTItem *stSearch(SymTable *sym, tKey key)
{
    int hash = hashCode(key, STsize);
    SymTItem *tmp = (*sym)[hash];
    while (tmp != NULL)
    {
        // je nazov symbolu totozny s hladanym?
        if (strcmp(tmp->key, key) == 0)
        {
            // ak ano vrati ho
            return tmp;
        }
        tmp = tmp->next;
    }
    return NULL; // ak sa nenasiel symbol vrati NULL
}

// hladanie konkretneho symbolu podla scopu v ktorom je
SymTItem *searchdown(SymTItem *sym, int hide, int forcnt, int ifcnt)
{
    SymTItem *tmp = sym;
    while (tmp != NULL)
    {
        if (tmp->forcnt == forcnt && tmp->ifcnt == ifcnt && tmp->hide == hide)
        {
            // ak sedia vsetky pocitadla vrati prvok
            return tmp;
        }
        tmp = tmp->down;
    }
    return NULL; // ak sa nenasiel prvok s tymito pocitadlami vrati NULL
}

// hladanie predosleho vyskytu symbolu v tabulke
SymTItem *searchforID(SymTItem *sym, int hide, int forcnt, int ifcnt)
{
    SymTItem *tmp = searchdown(sym, hide, forcnt, ifcnt);
    if (tmp != NULL)
    {
        return tmp;
    }
    int i;
    int n = 1;
    // cyklus pokial je zanorenie vacsie alebo rovne ako pomocna premenna n
    while (hide >= n)
    {
        i = 1;
        // cyklus na prejdenie vsetkych ifov a forov
        while (i <= ifcnt || i <= forcnt)
        {
            tmp = sym;
            // cyklus na prejdenie symbolu smerom dole
            while (tmp != NULL)
            {
                // ak sa najde symbol so spravnymi prvkami
                if (tmp->hide == hide - n && (tmp->forcnt == forcnt - i || tmp->ifcnt == ifcnt - i))
                {
                    return tmp;
                }
                tmp = tmp->down;
            }
            i++;
        }
        n++;
    }
    return NULL; // ak sa nenajde vrati NULL
}