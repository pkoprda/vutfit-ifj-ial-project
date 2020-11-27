#include "libmine.h"

int old = 0;

void printhashtable(FunTable *fun)
{
    stdout_print("------------HASH TABLE--------------\n");

    for (int i = 0; i < FTsize; i++)
    {
        stdout_print("%i:", i);
        FunTItem *ptr = (*fun)[i];

        while (ptr != NULL)
        {
            stdout_print(" (%s,%d,%d,%d)", ptr->key, ptr->count, ptr->retvar, ptr->types);
            ptr = ptr->next;
        }
        stdout_print("\n");
    }

    stdout_print("------------------------------------\n");
}

void printsymtable(FunTable *fun)
{
    stdout_print("------------SYMTABLE--------------\n");

    for (int i = 0; i < STsize; i++)
    {
        stdout_print("%i:", i);

        FunTItem *tmp = ftSearch(fun, "factorial");
        SymTable *sym = tmp->sym;
        SymTItem *ptr = (*sym)[i];

        while (ptr != NULL)
        {
            stdout_print(" (%s,%s,%d)", ptr->key, ptr->value, ptr->type);
            ptr = ptr->next;
        }
    }

    stdout_print("------------------------------------\n");
}

int cnt(Tree *ast, int i)
{
    Tree *tmp = ast;
    if (i == -1)
    {
        i = 0;
        tmp = tmp->Rptr;
        while (tmp != NULL)
        {
            i++;
            tmp = tmp->Lptr;
        }
    }
    else
    {
        tmp = tmp->Lptr;
        while (tmp != NULL)
        {
            i++;
            tmp = tmp->Lptr;
        }
    }
    return i;
}

int getTypes(Tree *ast, int retvar, int count, SymTable *sym)
{
    Tree *tmp1 = ast->Lptr;
    Tree *tmp2 = ast->Rptr;
    int i = 0;
    int types = 0;
    int tmp = 0;

    while (count != retvar)
    {
        if (tmp1->Rptr->type == N_PARAM_IDENT_INT)
        {
            tmp = 1;
        }
        else if (tmp1->Rptr->type == N_PARAM_IDENT_STR)
        {
            tmp = 3;
        }
        else if (tmp1->Rptr->type == N_PARAM_IDENT_FLOAT)
        {
            tmp = 2;
        }

        types = types * 10 + tmp;
        newSym(tmp1->Rptr->value, tmp1->Rptr->type - 4, NULL, sym);
        tmp1 = tmp1->Lptr;
        i++;
        count--;
    }

    while (retvar != 0)
    {
        if (tmp2->Rptr->type == RETURN_TYPE_INT)
        {
            tmp = 1;
        }
        else if (tmp2->Rptr->type == RETURN_TYPE_STR)
        {
            tmp = 3;
        }
        else if (tmp2->Rptr->type == RETURN_TYPE_FLOAT)
        {
            tmp = 2;
        }

        types = types * 10 + tmp;
        tmp2 = tmp2->Lptr;
        i++;
        retvar--;
    }
    return types;
}

void statm(Tree *ast, SymTable *sym)
{
    if (ast->Rptr->type >= N_PLUS && ast->Rptr->type <= N_DIV)
    {
        if (ast->Rptr->type == N_DIV)
        {
            if (ast->Rptr->Rptr->type == N_LIT_INT)
            {
                if (strcmp(ast->Rptr->Rptr->value, "0") == 0)
                {
                    error_exit(DIVISION_ZERO_ERROR, "Division by 0");
                }
            }
        }
        statm(ast->Rptr, sym);
    }

    if (ast->Lptr->type >= N_PLUS && ast->Lptr->type <= N_DIV)
    {
        statm(ast->Lptr, sym);
    }

    if (ast->Rptr->type >= N_LIT_INT && ast->Rptr->type <= N_LIT_STRING)
    {
        int tmp = old;
        old = (ast->Rptr->type) - 7;
        if (tmp != old && tmp != 0)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }

    if (ast->Lptr->type >= N_LIT_INT && ast->Lptr->type <= N_LIT_STRING)
    {
        int tmp = old;
        old = (ast->Lptr->type - 7);
        if (tmp != old && tmp != 0)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }
    if (ast->Rptr->type == N_IDENTIFIER)
    {
        SymTItem *found = stSearch(sym, ast->Rptr->value);
        if (found == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
        }
        int tmp = old;
        old = found->type;
        if (tmp != old && tmp != 0)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }

    if (ast->Lptr->type == N_IDENTIFIER)
    {
        SymTItem *found = stSearch(sym, ast->Lptr->value);
        if (found == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
        }
        int tmp = old;
        old = found->type;
        if (tmp != old && tmp != 0)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }
}

int getIDtype(Tree *ast, char *value, SymTable *sym, FunTable *fun)
{
    switch (ast->type)
    {
    case N_LIT_INT:
    case N_LIT_STRING:
    case N_LIT_FLOAT:;
        value = ast->value;
        return (ast->type - 7);
        break;

    case N_MINUS:
    case N_PLUS:
    case N_MULL:
    case N_DIV:;
        statm(ast, sym);
        int type = old;
        old = 0;
        return type;
        break;

    case SEQ:;
        // return getIDtype(ast->Rptr, value, sym, fun);
        Tree *tmp = ast;
        int endtype = 0;
        while (tmp != NULL)
        {
            int half = getIDtype(tmp->Rptr, value, sym, fun);
            endtype = endtype * 10 + half;
            tmp = tmp->Lptr;
        }
        return endtype;
        break;

    case N_FUNC:;
        FunTItem *fItem = ftSearch(fun, ast->value);
        if (fItem == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Func not defined yet");
        }
        int all = fItem->types;
        int retval = fItem->retvar;
        int kons = 1;
        while (retval != 0)
        {
            kons *= 10;
            retval--;
        }
        return (all - (all / kons) * kons);
        break;

    case N_INPUTI:
        return 11;
        break;
    case N_INPUTS:
        return 21;
        break;
    case N_INPUTF:
        return 31;
        break;
    case N_LEN:
        return 1;
        break;
    case N_SUBSTR:
        return 21;
        break;
    case N_ORD:
        return 11;
        break;
    case N_CHR:
        return 21;
        break;
    case N_INT2FLOAT:
        return 3;
        break;
    case N_FLOAT2INT:
        return 1;
        break;
    }
    value = value;
    return 0;
}

void InFuncGo(Tree *ast, SymTable *sym, FunTable *fun)
{
    if (ast->type == SEQ)
    {
        switch (ast->Rptr->type)
        {
        case N_IDENT_DEF:;
            char *value = NULL;
            int type = getIDtype(ast->Rptr->Rptr, value, sym, fun);
            Tree *tmp = ast->Rptr->Lptr;
            if (tmp->type != SEQ)
            {
                newSym(tmp->value, type, value, sym);
                break;
            }

            while (tmp != NULL)
            {
                newSym(tmp->Rptr->value, type, value, sym);
                tmp = tmp->Lptr;
            }
            break;

        case N_IDENT_INIT:;
            char *value1 = NULL;
            tmp = ast->Rptr->Lptr;
            int type1 = getIDtype(ast->Rptr->Rptr, value1, sym, fun);
            SymTItem *sItem;

            if (tmp->type != SEQ)
            {
                sItem = stSearch(sym, tmp->value);
                if (sItem == NULL)
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                if (type1 != sItem->type)
                {
                    error_exit(SEM_ERROR_TYPE, "Type of variable is not equal to statement");
                }
                break;
            }

            int vartype = 0;
            while (tmp != NULL)
            {
                sItem = stSearch(sym, tmp->Rptr->value);
                if (sItem == NULL)
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                vartype = vartype * 10 + sItem->type;
                tmp = tmp->Lptr;
            }
            if (vartype != type1)
            {
                error_exit(SEM_ERROR_TYPE, "Type of variable is not coresponding with assignment");
            }
            break;

        case N_PRINT:
            tmp = ast->Rptr->Lptr;
            while (tmp != SEQ)
            {
                if (tmp->Rptr->type == N_IDENTIFIER)
                {
                    sItem = stSearch(sym, tmp->Rptr->value);
                    if (sItem == NULL)
                    {
                        error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                    }
                }
                tmp = tmp->Lptr;
            }
            break;
        }
    }

    if (ast->Lptr != NULL)
    {
        InFuncGo(ast->Lptr, sym, fun);
    }
}

void FUN_def(Tree *ast, FunTable *fun)
{
    char *name = ast->value;
    SymTable *sym = (SymTable *)malloc(sizeof(SymTable));
    stInit(sym);

    if (strcmp(name, "main") == 0)
    {
        newFun(fun, name, 0, 0, 0, sym);
    }
    else
    {
        int retvar = cnt(ast->Lptr, -1);
        int count = cnt(ast->Lptr, retvar);
        newFun(fun, name, retvar, count, 0, sym);
        FunTItem *fItem = ftSearch(fun, name);
        int types = getTypes(ast->Lptr, retvar, count, sym);
        fItem->types = types;
    }

    InFuncGo(ast->Rptr, sym, fun);
}

void FuncLookup(Tree *ast, FunTable *fun)
{
    if (ast->Lptr == NULL)
    {
        return;
    }
    ast = ast->Lptr;

    if (ast->type == SEQ)
    {
        if (ast->Rptr->type == N_DEF_FUNC)
        {
            FUN_def(ast->Rptr, fun);
        }
        FuncLookup(ast, fun);
    }
}

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
    stdout_print("===================semantika================== \n \n");
    FunTable *fun = (FunTable *)malloc(sizeof(FunTable));
    ftInit(fun);
    FillPredefFunc(fun);
    FuncLookup(ast->Rptr, fun);

    printhashtable(fun);
    //printsymtable(fun);

    stdout_print("===========koniec semantiky====================\n \n");

    return 0;
}