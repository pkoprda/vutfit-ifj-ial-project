/*
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Semanticka kontrola
 * Autori: Viliam Holik - xholik14, Pavol Babjak - xbabja03
 */

#include "symtable.h"

int ifcnt = 0;  // if pocitadlo
int forcnt = 0; // for pocitadlo
int hide = 0;   // zanorenie v scopes

int old = 0; // pomocna premenna pre kontroly typov
char *newvalue = NULL;
FunTable *ft = NULL; // hashtable pre funkcie

// pomocna funkcie na spocitanie parametrov/navratovych hodnot
int cnt(Tree *ast, int i)
{
    Tree *tmp = ast;
    // pre retvar
    if (i == -1)
    {
        i = 0;
        tmp = tmp->Rptr;
        // cyklus na prejdenie podstromu
        while (tmp != NULL)
        {
            i++;
            tmp = tmp->Lptr;
        }
    }

    // pre count
    else
    {
        tmp = tmp->Lptr;
        // cyklus na prejdenie podstromu
        while (tmp != NULL)
        {
            i++;
            tmp = tmp->Lptr;
        }
    }
    return i;
}

// funkcia na zistenie typov parametrov a navratovych hodnot
int getTypes(Tree *ast, int retvar, int count, SymTable *sym)
{
    Tree *tmp1 = ast->Lptr;
    Tree *tmp2 = ast->Rptr;
    int i = 0;
    int types = 0;

    // cyklus na prejdenie parametrov
    while (count != retvar)
    {
        types = types * 10 + (tmp1->Rptr->type - 4);
        newSym(tmp1->Rptr->value, tmp1->Rptr->type - 4, NULL, hide, forcnt, ifcnt, sym);
        tmp1 = tmp1->Lptr;
        i++;
        count--;
    }

    // cyklus na prejdenie navratovych hodnot
    while (retvar != 0)
    {
        types = types * 10 + (tmp2->Rptr->type - 10);
        tmp2 = tmp2->Lptr;
        i++;
        retvar--;
    }
    return types;
}

// kontrola ci sa podtrzitko nachadza na mieste pre parametre
void underscorecheck(Tree *ast)
{
    if (strcmp(ast->value, "_") == 0)
    {
        error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
    }
}

// pomocna funkcia na kontrolu typov vo vyraze
void stcheck(int type)
{
    if (old == 0)
    {
        old = type;
    }
    else
    {
        // ak sa predosla hodnota nerovna dalsej tak chyba
        if (old != type)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }
}

// kontrola vyrazov
void statm(Tree *ast, SymTable *sym)
{
    if (ast == NULL)
    {
        return;
    }

    SymTItem *sItem = NULL;

    // kontrola ci sa nedeli 0
    if (ast->type == N_DIV)
    {
        // ak je INT a ma hodnotu 0 -> ERROR
        if (ast->Rptr->type == N_LIT_INT)
        {
            if (strcmp(ast->Rptr->value, "0") == 0)
            {
                error_exit(DIVISION_ZERO_ERROR, "Division by 0");
            }
        }

        // ak ma symbol nulovu hodnotu -> ERROR
        else if (ast->Rptr->type == N_IDENTIFIER)
        {
            // hladanie symbolu v tabulke
            sItem = stSearch(sym, ast->Rptr->value);
            // ak sa nenasiel -> ERROR
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
            if (strcmp(sItem->value, "0") == 0 || strcmp(sItem->value, "0.0") == 0)
            {
                error_exit(DIVISION_ZERO_ERROR, "Division by 0");
            }
        }

        // ak je FLOAT a ma hodnotu 0.0 -> ERROR
        else if (ast->Rptr->type == N_LIT_FLOAT)
        {
            if (strcmp(ast->Rptr->value, "0.0") == 0)
            {
                error_exit(DIVISION_ZERO_ERROR, "Division by 0");
            }
        }
    }

    if (ast->type >= N_PLUS && ast->type <= N_DIV)
    {
        // kontrola pre stringy, moze byt pouzite len s operatorom +
        if (ast->type != N_PLUS && (ast->Rptr->type == N_LIT_STRING || ast->Lptr->type == N_LIT_STRING))
        {
            error_exit(SEM_ERROR_TYPE, "Cannot operate with string");
        }

        // kontrola praveho podstromu
        if (ast->Rptr->type >= N_LIT_INT && ast->Rptr->type <= N_LIT_FLOAT)
        {
            stcheck(ast->Rptr->type - 7);
        }
        else if (ast->Rptr->type == N_IDENTIFIER)
        {
            // kontrola podtrzitka
            if (strcmp(ast->Rptr->value, "_") == 0)
            {
                error_exit(SEM_ERROR_OTHERS, "Aritmetic operation with undescore as identifier");
            }
            // hladanie symbolu v tabulke pre symboly
            sItem = stSearch(sym, ast->Rptr->value);
            // ak sa symbol nenasiel -> ERROR
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
            // nemozno operovat so symbolom so stringom inak ako +
            if (sItem->type == 2 && ast->type != N_PLUS)
            {
                error_exit(SEM_ERROR_TYPE, "Cannot operate with string");
            }
            stcheck(sItem->type);
        }
        else
        {
            statm(ast->Rptr, sym);
        }

        // kontrola laveho podstromu
        if (ast->Lptr->type >= N_LIT_INT && ast->Lptr->type <= N_LIT_FLOAT)
        {
            stcheck(ast->Lptr->type - 7);
        }
        else if (ast->Lptr->type == N_IDENTIFIER)
        {
            // kontrola podtrzitka
            if (strcmp(ast->Lptr->value, "_") == 0)
            {
                error_exit(SEM_ERROR_OTHERS, "Aritmetic operation with undescore as identifier");
            }
            // hladanie symbolu v tabulke pre symboly
            sItem = stSearch(sym, ast->Lptr->value);
            // ak sa symbol nenasiel -> ERROR
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
            // nemozno operovat so symbolom so stringom inak ako +
            if (sItem->type == 2 && ast->type != N_PLUS)
            {
                error_exit(SEM_ERROR_TYPE, "Cannot operate with string");
            }
            stcheck(sItem->type);
        }
        else
        {
            statm(ast->Lptr, sym);
        }
    }
    else
    {
        error_exit(SEM_ERROR_TYPE, "Operation with different data types");
    }
}

//funkcia ktorá vracia číselnou hodnotou daný dátový typ
int getIDtype(Tree *ast, SymTable *sym, FunTable *fun)
{
    if (ast == NULL)
    {
        return 0;
    }
    switch (ast->type)
    {
    case N_LIT_INT:
    case N_LIT_STRING:
    case N_LIT_FLOAT:;
        newvalue = ast->value;
        return (ast->type - 7);
        break;

    case N_MINUS:
    case N_PLUS:
    case N_MULL:
    case N_DIV:;
        old = 0;
        statm(ast, sym);
        return old;
        break;

    case SEQ:;
        Tree *tmp = ast;
        int endtype = 0;
        // cyklus na prejdenie podstromu
        while (tmp != NULL)
        {
            int half = getIDtype(tmp->Rptr, sym, fun);
            // po castiach sa pridava
            endtype = endtype * 10 + half;
            tmp = tmp->Lptr;
        }
        return endtype;
        break;

    case N_FUNC:;
        if (stSearch(sym, ast->value) != NULL) //názov funkcie sa zhoduje s existujúcim identifkátorom položky tabulky SymTable
        {
            error_exit(SEM_ERROR_UNDEF, "Func same name as variable");
        }
        FunTItem *fItem = ftSearch(fun, ast->value);
        if (fItem == NULL) //funkcia sa nenašla v hashtable pre funkcie
        {
            error_exit(SEM_ERROR_UNDEF, "Func not defined yet");
        }
        int params = fItem->types;
        int all = fItem->types;
        int retval = fItem->retvar;
        int parval = retval;
        int kons = 1;
        while (parval != 0)
        {
            params = params / 10;
            parval--;
        }
        // kontrola parametrov
        if (params != getIDtype(ast->Lptr, sym, fun))
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        while (retval != 0)
        {
            kons *= 10;
            retval--;
        }
        return (all - (all / kons) * kons);
        break;

    //spracovávanie vstavaných funkcií s vopred známymi dátovými typmi
    case N_INPUTI:
        if (ast->value != NULL)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        return 11;
        break;
    case N_INPUTS:
        if (ast->value != NULL)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        return 21;
        break;
    case N_INPUTF:
        if (ast->value != NULL)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        return 31;
        break;
    case N_LEN:;
        //kontrola, ci parameter funkcie existuje a je dátového typu string
        if ((stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 2) || (stSearch(sym, ast->value) == NULL && (ast->value)[0] != '\"'))
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast); //kontrola, či zadaný parameter funkcie nie je prázdny identifikátor
        return 1;
        break;
    case N_SUBSTR:;
        tmp = ast->Lptr;
        while (tmp != NULL)
        {
            if (tmp->Rptr->type == N_IDENTIFIER)
            {
                if (strcmp(tmp->Rptr->value, "_") == 0) //pokial parameter tejto funkcie je prázdny identifikátor
                {
                    error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
                }
            }
            tmp = tmp->Lptr;
        }
        //kontrola, ci prvý parameter funkcie existuje a je dátového typu string
        if (ast->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Rptr->value) == NULL && (ast->Lptr->Rptr->value)[0] != '\"')
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if ((stSearch(sym, ast->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Rptr->value)->type != 2))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
        //kontrola, ci druhý parameter funkcie existuje a je dátového typu integer
        if (ast->Lptr->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Lptr->Rptr->value) == NULL)
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if ((stSearch(sym, ast->Lptr->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Lptr->Rptr->value)->type != 1))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
        //kontrola, ci tretí parameter funkcie existuje a je dátového typu integer
        if (ast->Lptr->Lptr->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Lptr->Lptr->Rptr->value) == NULL)
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if ((stSearch(sym, ast->Lptr->Lptr->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Lptr->Lptr->Rptr->value)->type != 1))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
        return 21;
        break;
    case N_ORD:
        tmp = ast->Lptr;
        while (tmp != NULL)
        {
            if (tmp->Rptr->type == N_IDENTIFIER) //pokial parameter tejto funkcie je prázdny identifikátor
            {
                if (strcmp(tmp->Rptr->value, "_") == 0)
                {
                    error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
                }
            }
            tmp = tmp->Lptr;
        }
        //kontrola, ci prvý parameter funkcie existuje a je dátového typu string
        if (ast->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Rptr->value) == NULL && (ast->Lptr->Rptr->value)[0] != '\"')
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if ((stSearch(sym, ast->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Rptr->value)->type != 2))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
        //kontrola, ci druhý parameter funkcie existuje a je dátového typu integer
        if (ast->Lptr->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Lptr->Rptr->value) == NULL)
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if ((stSearch(sym, ast->Lptr->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Lptr->Rptr->value)->type != 1))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
        return 11;
        break;
    case N_CHR:
        //kontrola, ci prvý parameter funkcie existuje a je dátového typu integer
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 1)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        return 21;
        break;
    case N_INT2FLOAT:
        //kontrola, ci prvý parameter funkcie existuje a je dátového typu integer
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 1)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast);
        return 3;
        break;
    case N_FLOAT2INT:
        //kontrola, ci prvý parameter funkcie existuje a je dátového typu float
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 3)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast); //kontrola prítomnosti podrtžítka v patametri
        return 1;
        break;

    case N_IDENTIFIER:;
        //kontrola prítomnosti podrtžítka ako identifikátora
        if (strcmp(ast->value, "_") == 0)
        {
            error_exit(SEM_ERROR_OTHERS, "Undescore can not be as a identifier");
        }
        SymTItem *Sitem1 = stSearch(sym, ast->value);
        // ak hladany symbol nie je v hashtable -> ERROR
        if (Sitem1 == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
        }
        // hladanie symbolu so spravnymi prvkami
        SymTItem *Sitem = searchdown(Sitem1, hide, forcnt, ifcnt);
        if (Sitem != NULL)
        {
            return Sitem->type;
        }
        // ak sa nenasiel symbol so spravnymi prvkami, hlada najblizsi "viditelny"
        Sitem = searchforID(Sitem1, hide, forcnt, ifcnt);
        return Sitem->type;
        break;
    }
    return 0;
}

//funkcia, ktorý overí, že parameter jej predaný je operátorom porovnania
void compare(Tree *ast)
{
    switch (ast->type)
    {
    case N_GREATER:
    case N_LESS:
    case N_EQ_GREATER:
    case N_EQ_LESS:
    case N_EQUAL:
    case N_NOT_EQUAL:
        return;
        break;

    default:
        error_exit(SEM_ERROR_TYPE, "No comparison");
        return;
        break;
    }
}

// navštívenie funkcie a vykonanie sémantických kontrol
void InFuncGo(Tree *ast, SymTable *sym, FunTable *fun, char *fname)
{
    if (ast->type == SEQ)
    {
        switch (ast->Rptr->type) //podla typu načítaného nodu sa vykonáva jednotlivý case
        {
        case N_IDENT_DEF:;
            int type = getIDtype(ast->Rptr->Rptr, sym, fun); //získanie dátového typu definície identifkátora
            Tree *tmp = ast->Rptr->Lptr;
            if (tmp->type != SEQ)
            {
                newSym(tmp->value, type, newvalue, hide, forcnt, ifcnt, sym); //vlozenie noveho symbolu do hashtable pre symboly
                newvalue = NULL;
                break;
            }

            char help[10];             //pomocná premenná
            sprintf(help, "%d", type); //prevod type do pomocnej premennej help
            int i = 0;
            while (tmp != NULL)
            {
                if (strcmp("_", tmp->Rptr->value) == 0)
                {
                    tmp = tmp->Lptr; //pokial sme načítali ako identifkátor _ , posunieme sa v AST doľava
                    i++;
                    continue;
                }
                newSym(tmp->Rptr->value, (help[i] - '0'), newvalue, hide, forcnt, ifcnt, sym); //vytvorenie nového symbolu do tabulky symbolov
                newvalue = NULL;
                tmp = tmp->Lptr;
                i++;
            }
            break;

        case N_IDENT_INIT:;
            tmp = ast->Rptr->Lptr;
            int type1 = getIDtype(ast->Rptr->Rptr, sym, fun); //získanie dátového typu identifkátora
            SymTItem *sItem;

            if (tmp->type != SEQ)
            {
                sItem = stSearch(sym, tmp->value); //pokúšame sa nájsť identifikátor v tabulke symbolov
                if (sItem != NULL)
                {
                    sItem = searchdown(sItem, hide, forcnt, ifcnt);
                    if (sItem == NULL)
                    {
                        newSym(tmp->value, getIDtype(ast->Rptr->Rptr, sym, fun), newvalue, hide, forcnt, ifcnt, sym); //uloženie symbolu do tabulky symbolov
                        newvalue = NULL;
                        break;
                    }
                }
                if ((strcmp(tmp->value, "_") != 0) && sItem == NULL) //pokial sme nenasli symbol v tabulke a sucasne sa nejedna o prazdny identifikator
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                if ((strcmp(tmp->value, "_") != 0) && (type1 != sItem->type)) //pokial sa symbol v tabulke nezhoduje s priradenim a sucasne sa nejedna o prazdny identifikator
                {
                    error_exit(SEM_ERROR_PARAMS, "Type of variable is not coresponding with assignment");
                }
                break;
            }

            int vartype = 0;
            while (tmp != NULL)
            {
                if (strcmp("_", tmp->Rptr->value) == 0) //prázdny identifkátor bude mať číselnú hodnotu 4
                {
                    tmp = tmp->Lptr;
                    vartype = vartype * 10 + 4;
                    continue;
                }

                sItem = stSearch(sym, tmp->Rptr->value);
                if (sItem == NULL) //pokial sme nenasli symbol v tabulke symbolov
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                vartype = vartype * 10 + sItem->type; //podla dátového typu si nastavíme vartype danej položky sItem
                tmp = tmp->Lptr;
            }
            //pomocne premenne pre vypocet spravnosti priradenia
            int thisDigit = 0;
            int thatDigit = 0;
            while (vartype != 0 || type1 != 0) //cyklus pre porovnanie spravnosti datovych typov priradenia
            {
                thatDigit = type1 % 10;
                type1 = type1 / 10;
                thisDigit = vartype % 10;
                vartype = vartype / 10;
                if (thisDigit == 0 || thatDigit == 0)
                {
                    error_exit(SEM_ERROR_OTHERS, "Number of assignment is not correct");
                }
                if (thisDigit != thatDigit && thisDigit != 4)
                {
                    error_exit(SEM_ERROR_TYPE, "Type of variable is not coresponding with assignment");
                }
            }
            break;

        case N_FUNC:;
            if (stSearch(sym, ast->Rptr->value) != NULL) //názov funkcie sa zhoduje s existujúcim identifkátorom položky tabulky SymTable
            {
                error_exit(SEM_ERROR_UNDEF, "Func same name as variable");
            }
            FunTItem *fItem = ftSearch(fun, ast->Rptr->value);
            if (fItem == NULL) //funkcia sa nenašla v hashtable pre funkcie
            {
                error_exit(SEM_ERROR_UNDEF, "Func not defined yet");
            }
            if (fItem->retvar != 0) //ak funkcia obsahuje nejaké návratové hodnoty musia byť následne priradené
            {
                error_exit(SEM_ERROR_OTHERS, "Must be assigned to a variable");
            }
            if (fItem->types != getIDtype(ast->Rptr->Lptr, sym, fun)) //nesprávne parametre predávané pri volaní funkcie
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            break;

        case N_PRINT:;
            tmp = ast->Rptr->Lptr;
            while (tmp != SEQ)
            {
                if (tmp->Rptr->type == N_IDENTIFIER) //ak sa v printe vyskytne ako parameter identifikátor
                {
                    sItem = stSearch(sym, tmp->Rptr->value); //hladáme existenciu symbolu v tabulke Symtable, ktorý má vypísať funkcia print
                    if (sItem == NULL)
                    {
                        error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                    }
                }
                tmp = tmp->Lptr; //posun dolava v AST pre pripad, ze print obsahuje viac parametrov
            }
            break;

        case N_RETURN:;
            int type2 = getIDtype(ast->Rptr->Lptr, sym, fun); //ziskanie datoveho typu daneho nodu
            FunTItem *Fitem = ftSearch(fun, fname); //hladanie funkcie v hashtable pre funkcie
            int returnvalue = Fitem->types; //priradenie return types funkcie do premennej
            int retvar = Fitem->retvar; //priradenie poctu navratovych hodnot do premennej
            int kons = 1;

            while (retvar != 0)
            {
                kons *= 10;
                retvar--;
            }

            returnvalue = returnvalue - (returnvalue / kons) * kons;
            if (returnvalue != type2) //porovnanie, či sa zhoduje dátový typ returnu s definíciou funkcie
            {
                error_exit(SEM_ERROR_PARAMS, "Return type not correspoding with function definition");
            }
            break;

        case N_FOR:;
            forcnt++; // navysenie pocitadla
            hide++;   // zanorenie
            tmp = ast->Rptr->Lptr;
            fname = NULL;

            // inicializacia pre if
            if (tmp->Rptr != NULL && tmp->Rptr->type == N_IDENT_DEF)
            {
                type = getIDtype(tmp->Rptr->Rptr, sym, fun);
                tmp = tmp->Rptr->Lptr;
                // "novy" symbol s danymi pocitadlami
                newSym(tmp->value, type, newvalue, hide, forcnt, ifcnt, sym);
                newvalue = NULL;
            }

            // testovacia podmienka
            tmp = ast->Rptr->Lptr->Lptr;
            // je tam test?
            compare(tmp->Rptr);
            // kontrola typov
            if (getIDtype(tmp->Rptr->Lptr, sym, fun) != getIDtype(tmp->Rptr->Rptr, sym, fun))
            {
                error_exit(SEM_ERROR_TYPE, "Operation with different data types");
            }

            // inkrementacia
            tmp = tmp->Lptr;
            if (tmp->Rptr != NULL && tmp->Rptr->type == N_IDENT_INIT)
            {
                InFuncGo(tmp, sym, fun, fname);
            }

            // telo foru
            tmp = ast->Rptr->Rptr;
            if (tmp != NULL)
            {
                hide++; // zanorenie + 1
                InFuncGo(tmp, sym, fun, fname);
                hide--; // vynorenie - 1
            }
            hide--; // vynorenie
            break;

        case N_IF:;
            ifcnt++; // navysenie pocitadla
            hide++;  // zanorenie

            // testovacia podmienka
            tmp = ast->Rptr->Lptr;
            // je tam test?
            compare(tmp->Rptr);
            // kontrola typov
            if ((getIDtype(tmp->Rptr->Lptr, sym, fun)) != (getIDtype(tmp->Rptr->Rptr, sym, fun)))
            {
                error_exit(SEM_ERROR_TYPE, "Operation with different data types");
            }

            tmp = ast->Rptr->Rptr;
            // true vetva
            if (tmp->Rptr != NULL)
            {
                InFuncGo(tmp->Rptr, sym, fun, fname);
            }

            // false vetva
            if (tmp->Lptr != NULL)
            {
                ifcnt++; // navysenie pocitadla
                InFuncGo(tmp->Lptr, sym, fun, fname);
            }
            hide--; // vynorenie
            break;
        }
    }
    // pokracovanie po nodoch
    if (ast->Lptr != NULL)
    {
        InFuncGo(ast->Lptr, sym, fun, fname);
    }
}

//uloženie atribútov funkcie do tabulky symbolov
void FUN_def(Tree *ast, FunTable *fun)
{
    char *name = ast->value;
    SymTable *sym = (SymTable *)malloc(sizeof(SymTable)); //alokácia pamäte
    if (sym == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }
    stInit(sym); //inicializácia tabulky symbolov

    if (strcmp(name, "main") == 0) //pokial sa jedná o funciu main, nastavíme predom známe atribúty do tabulky symbolov
    {
        newFun(fun, name, 0, 0, 0, sym);
    }
    else
    {
        if (ast->Lptr == NULL)
        {
            newFun(fun, name, 0, 0, 0, sym);
            return;
        }
        int retvar = cnt(ast->Lptr, -1);    //počet návratových hodnôt
        int count = cnt(ast->Lptr, retvar); //suma parametrov a návratových hodnôť
        newFun(fun, name, retvar, count, 0, sym);
        FunTItem *fItem = ftSearch(fun, name);
        int types = getTypes(ast->Lptr, retvar, count, sym); //types - číselne vyjadrená hodnota parametrov a returtypes podla dátového typu
        fItem->types = types;
    }
}

// prechadzanie v jednotlivych funkciach
void FUN_in(Tree *ast, FunTable *fun)
{
    char *name = ast->value;
    FunTItem *fItem = ftSearch(fun, name); //vyhladanie funkcie v tabulke pre funkcie
    if (ast->Rptr != NULL)
    {
        InFuncGo(ast->Rptr, fItem->sym, fun, name); //navštívenie funkcie a vykonanie sémantických kontrol
    }
}

//navštívnenie jednotlivých funkcíí a následné vykonanie sémantických kontrol
void FuncVisit(Tree *ast, FunTable *fun)
{
    if (ast->Lptr == NULL) //pokial neexistuje žiadny node vľavo
    {
        return;
    }
    ast = ast->Lptr;

    if (ast->type == SEQ)
    {
        if (ast->Rptr->type == N_DEF_FUNC) //narazili sme na node definície funkcie
        {
            //nastavenie counterov pre používanie funkcíí v scopoch
            ifcnt = 0;
            forcnt = 0;
            hide = 0;
            newvalue = NULL;
            FUN_in(ast->Rptr, fun);
        }
        FuncVisit(ast, fun);
    }
}

//funkcia ktorá vyhľadá všetky funkcie a uloží ich do hashtable
void FuncLookup(Tree *ast, FunTable *fun)
{
    if (ast->Lptr == NULL) //pokial neexistuje žiadny node vľavo
    {
        return;
    }
    ast = ast->Lptr;

    if (ast->type == SEQ)
    {
        if (ast->Rptr->type == N_DEF_FUNC) //narazili sme na node definície funkcie
        {
            //nastavenie counterov pre používanie funkcíí v scopoch
            ifcnt = 0;
            forcnt = 0;
            hide = 0;
            newvalue = NULL;
            FUN_def(ast->Rptr, fun);
        }
        FuncLookup(ast, fun);
    }
}
//vloženie vstavaných funkcíí do tabulky
void FillPredefFunc(FunTable *fun)
{
    newFun(fun, "inputs", 2, 2, 21, NULL);
    newFun(fun, "inputi", 2, 2, 11, NULL);
    newFun(fun, "inputf", 2, 2, 31, NULL);
    newFun(fun, "int2float", 1, 2, 13, NULL);
    newFun(fun, "float2int", 1, 2, 31, NULL);
    newFun(fun, "len", 1, 2, 21, NULL);
    newFun(fun, "substr", 2, 5, 21121, NULL);
    newFun(fun, "ord", 2, 4, 2111, NULL);
    newFun(fun, "chr", 2, 3, 121, NULL);
}

int semantics()
{
    //alokácia pamäte pre Funtable
    FunTable *fun = (FunTable *)malloc(sizeof(FunTable));
    if (fun == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }
    ftInit(fun);                //inicializácia hashtable
    FillPredefFunc(fun);        //vloženie vstavaných funkcíí do tabulky
    FuncLookup(ast->Rptr, fun); //funkcia ktorá vyhľadá všetky funkcie a uloží ich do hashtable
    FuncVisit(ast->Rptr, fun);  //navštívnenie jednotlivých funkcíí a následné vykonanie sémantických kontrol
    ft = fun;
    return 0;
}