#include "symtable.h"

//authors: xbabja03, xholik14

int old = 0;
int ifcnt = 0;
int forcnt = 0;
int hide = 0;

/*
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
*/

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
            tmp = 2;
        }
        else if (tmp1->Rptr->type == N_PARAM_IDENT_FLOAT)
        {
            tmp = 3;
        }

        types = types * 10 + tmp;
        newSym(tmp1->Rptr->value, tmp1->Rptr->type - 4, NULL, hide, forcnt, ifcnt, sym);
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
            tmp = 2;
        }
        else if (tmp2->Rptr->type == RETURN_TYPE_FLOAT)
        {
            tmp = 3;
        }

        types = types * 10 + tmp;
        tmp2 = tmp2->Lptr;
        i++;
        retvar--;
    }
    return types;
}

void underscorecheck(Tree *ast)
{
    if (strcmp(ast->value, "_")==0)
    {
        error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
    }
}

void statm(Tree *ast, SymTable *sym)
{
    //printf("value %s \n" , ast->Rptr->value);
    if (ast->Rptr->type >= N_PLUS && ast->Rptr->type <= N_DIV)
    {
        if (ast->Rptr->type == N_DIV)
        {
            if (ast->Rptr->type == N_LIT_INT)
            {
                if (strcmp(ast->Rptr->value, "0") == 0)
                {
                    error_exit(DIVISION_ZERO_ERROR, "Division by 0");
                }
            }
        }
        statm(ast->Rptr, sym);
    }

    if (ast->Lptr->type >= N_PLUS && ast->Lptr->type <= N_DIV)
    {
        if (ast->Lptr->type == N_DIV)
        {
            if (ast->Lptr->Rptr->type == N_LIT_INT)
            {
                if (strcmp(ast->Lptr->Rptr->value, "0") == 0)
                {
                    error_exit(DIVISION_ZERO_ERROR, "Division by 0");
                }
            }
        }
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
        // toto som dorobil aby prechadzal test underscore_usage_3.go
        if (strcmp(ast->Lptr->value, "_") == 0)
        {
            old = 0;
            return;
        }

        SymTItem *found = stSearch(sym, ast->Rptr->value);
        if (found == NULL )
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
        // toto som dorobil aby prechadzal test underscore_usage_3.go
        if (strcmp(ast->Lptr->value, "_") == 0)
        {
            old = 0;
            return;
        }

        SymTItem *found = stSearch(sym, ast->Lptr->value);
        if (found == NULL )
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
    if (ast == NULL)
    {
        return 0;
    }
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
        if (stSearch(sym, ast->value) != NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Func same name as variable");
        }
        FunTItem *fItem = ftSearch(fun, ast->value);
        if (fItem == NULL)
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
        if (params != getIDtype(ast->Lptr, value, sym, fun))
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
        if ((stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 2) || (stSearch(sym, ast->value) == NULL && (ast->value)[0]!='\"'))
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast);
        return 1;
        break;
    case N_SUBSTR:;
        tmp = ast->Lptr;
        while (tmp != NULL)
        {
            if (tmp->Rptr->type == N_IDENTIFIER)
            {
                if (strcmp(tmp->Rptr->value, "_")==0)
                {
                    error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
                }

            }
            tmp = tmp->Lptr;
        }

        
        return 21;
        break;
    case N_ORD:
        tmp = ast->Lptr;
        while (tmp != NULL)
        {
            if (tmp->Rptr->type == N_IDENTIFIER)
            {
                if (strcmp(tmp->Rptr->value, "_")==0)
                {
                    error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
                }
            }
            tmp = tmp->Lptr;
        }
        return 11;
        break;
    case N_CHR:
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 1)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        return 21;
        break;
    case N_INT2FLOAT:
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 1)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast);
        return 3;
        break;
    case N_FLOAT2INT:
        if (stSearch(sym, ast->value) != NULL && stSearch(sym, ast->value)->type != 3)
        {
            error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
        }
        underscorecheck(ast);
        return 1;
        break;
    case N_IDENTIFIER:;
        SymTItem *Sitem = stSearch(sym, ast->value);
        if (Sitem == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
        }
        return Sitem->type;
        break;
    }
    // value = value;
    return 0;
}

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

void InFuncGo(Tree *ast, SymTable *sym, FunTable *fun, char *fname)
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
                newSym(tmp->value, type, value, hide, forcnt, ifcnt, sym);
                break;
            }

            char help[10];
            sprintf(help, "%d", type);
            int i = 0;
            while (tmp != NULL)
            {
                if (strcmp("_", tmp->Rptr->value) == 0)
                {
                    tmp = tmp->Lptr;
                    i++;
                    continue;
                }
                newSym(tmp->Rptr->value, (help[i] - '0'), value, hide, forcnt, ifcnt, sym);
                tmp = tmp->Lptr;
                i++;
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
                if (sItem != NULL)
                {
                    sItem = searchdown(sItem, hide, forcnt, ifcnt);
                    if (sItem == NULL)
                    {
                        newSym(tmp->value, getIDtype(ast->Rptr->Rptr, value1, sym, fun), value1, hide, forcnt, ifcnt, sym);
                        break;
                    }
                }
                //printf("string %s \n", tmp->value);
                if ((strcmp(tmp->value, "_") != 0 ) && sItem == NULL)
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                if ((strcmp(tmp->value, "_") != 0 ) && (type1 != sItem->type))
                {
                    error_exit(SEM_ERROR_PARAMS, "Type of variable is not coresponding with assignment");
                }
                break;
            }

            int vartype = 0;
            while (tmp != NULL)
            {
                if (strcmp("_", tmp->Rptr->value) == 0)
                {
                    tmp = tmp->Lptr;
                    vartype = vartype * 10 + 4;
                    continue;
                }

                sItem = stSearch(sym, tmp->Rptr->value);
                if (sItem == NULL)
                {
                    error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
                }
                vartype = vartype * 10 + sItem->type;
                tmp = tmp->Lptr;
            }
            int thisDigit = 0;
            int thatDigit = 0;
            while (vartype != 0 || type1 != 0)
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
            if (stSearch(sym, ast->Rptr->value) != NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Func same name as variable");
            }
            // printf("pea\n");
            FunTItem *fItem = ftSearch(fun, ast->Rptr->value);
            if (fItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Func not defined yet");
            }
            // printf("pea\n");
            if (fItem->retvar != 0)
            {
                error_exit(SEM_ERROR_OTHERS, "Must be assigned to a variable");
            }
            value = NULL;
            // printf("pea\n");
            if (fItem->types != getIDtype(ast->Rptr->Lptr, value, sym, fun))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            break;

        case N_PRINT:;
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

        case N_RETURN:;
            value = NULL;
            int type2 = getIDtype(ast->Rptr->Lptr, value, sym, fun);
            FunTItem *Fitem = ftSearch(fun, fname);
            int returnvalue = Fitem->types;
            int retvar = Fitem->retvar;
            int kons = 1;

            while (retvar != 0)
            {
                kons *= 10;
                retvar--;
            }

            returnvalue = returnvalue - (returnvalue / kons) * kons;
            if (returnvalue != type2)
            {
                error_exit(SEM_ERROR_PARAMS, "Return type not correspoding with function definition");
            }
            break;

        case N_FOR:;
            forcnt++;
            hide++;
            tmp = ast->Rptr->Lptr;
            fname = NULL;
            if (tmp->Rptr != NULL && tmp->Rptr->type == N_IDENT_DEF)
            {
                value = NULL;
                type = getIDtype(tmp->Rptr->Rptr, value, sym, fun);
                tmp = tmp->Rptr->Lptr;
                newSym(tmp->value, type, value, hide, forcnt, ifcnt, sym);
            }
            tmp = ast->Rptr->Lptr->Lptr;
            compare(tmp->Rptr);
            if (getIDtype(tmp->Rptr->Lptr, value, sym, fun) != getIDtype(tmp->Rptr->Rptr, value, sym, fun))
            {
                error_exit(SEM_ERROR_TYPE, "Operation with different data types");
            }
            tmp = tmp->Lptr;
            if (tmp->Rptr != NULL && tmp->Rptr->type == N_IDENT_INIT)
            {
                InFuncGo(tmp, sym, fun, fname);
            }
            tmp = ast->Rptr->Rptr;
            if (tmp != NULL)
            {
                hide++;
                InFuncGo(tmp, sym, fun, fname);
                hide--;
            }
            hide--;
            break;

        case N_IF:;
            ifcnt++;
            hide++;
            tmp = ast->Rptr->Lptr;
            value = NULL;
            compare(tmp->Rptr);
            if ((getIDtype(tmp->Rptr->Lptr, value, sym, fun)) != (getIDtype(tmp->Rptr->Rptr, value, sym, fun)))
            {
                error_exit(SEM_ERROR_TYPE, "Operation with different data types");
            }
            tmp = ast->Rptr;
            if (tmp->Lptr != NULL)
            {
                InFuncGo(tmp->Lptr, sym, fun, fname);
            }
            if (tmp->Rptr != NULL)
            {
                InFuncGo(tmp->Rptr, sym, fun, fname);
            }
            hide--;
            break;
        }
    }

    if (ast->Lptr != NULL)
    {
        InFuncGo(ast->Lptr, sym, fun, fname);
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
        if (ast->Lptr == NULL)
        {
            newFun(fun, name, 0, 0, 0, sym);
            if (ast->Rptr != NULL)
            {
                InFuncGo(ast->Rptr, sym, fun, name);
            }
            return;
        }
        int retvar = cnt(ast->Lptr, -1);
        int count = cnt(ast->Lptr, retvar);
        newFun(fun, name, retvar, count, 0, sym);
        FunTItem *fItem = ftSearch(fun, name);
        int types = getTypes(ast->Lptr, retvar, count, sym);
        fItem->types = types;
    }
    if (ast->Rptr != NULL)
    {
        InFuncGo(ast->Rptr, sym, fun, name);
    }
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
    //stdout_print("===================semantika================== \n \n");
    FunTable *fun = (FunTable *)malloc(sizeof(FunTable));
    ftInit(fun);
    FillPredefFunc(fun);
    FuncLookup(ast->Rptr, fun);

    //printhashtable(fun);
    //printsymtable(fun);

    //stdout_print("===========koniec semantiky====================\n \n");

    return 0;
}