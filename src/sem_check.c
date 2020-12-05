#include "symtable.h"

//authors: xbabja03, xholik14

int old = 0;
int ifcnt = 0;
int forcnt = 0;
int hide = 0;
char *newvalue = NULL;

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

void stcheck(int type)
{
    if (old == 0)
    {
        old = type;
    }
    else
    {
        if (old != type)
        {
            error_exit(SEM_ERROR_TYPE, "Operation with different data types");
        }
    }
}

void statm(Tree *ast, SymTable *sym)
{
    if (ast == NULL)
    {
        return;
    }
    
    SymTItem *sItem = NULL;

    // divide by 0
    if (ast->type == N_DIV)
    {
        if (ast->Rptr->type == N_LIT_INT)
        {
            if (strcmp(ast->Rptr->value, "0") == 0)
            {
                error_exit(DIVISION_ZERO_ERROR, "Division by 0");
            }
        }
        else if (ast->Rptr->type == N_IDENTIFIER)
        {
            sItem = stSearch(sym, ast->Rptr->value);
            // printf("--%s\n", sItem->value);
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
            if (strcmp(sItem->value, "0") == 0 || strcmp(sItem->value, "0.0") == 0)
            {
                error_exit(DIVISION_ZERO_ERROR, "Division by 0");
            }
        }
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
        // check string statment, it can be only with +
        if (ast->type != N_PLUS && (ast->Rptr->type == N_LIT_STRING || ast->Lptr->type == N_LIT_STRING))
        {
            error_exit(SEM_ERROR_TYPE, "Cannot operate with string");
        }

        if (ast->Rptr->type >= N_LIT_INT && ast->Rptr->type <= N_LIT_FLOAT)
        {
            stcheck(ast->Rptr->type - 7);
        }
        else if (ast->Rptr->type == N_IDENTIFIER)
        {
            if (strcmp(ast->Rptr->value, "_") ==0)
            {
                error_exit(SEM_ERROR_OTHERS, "Aritmetic operation with undescore as identifier");
            }
            sItem = stSearch(sym, ast->Rptr->value);
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
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

        if (ast->Lptr->type >= N_LIT_INT && ast->Lptr->type <= N_LIT_FLOAT)
        {
            stcheck(ast->Lptr->type - 7);
        }
        else if (ast->Lptr->type == N_IDENTIFIER)
        {
            if (strcmp(ast->Lptr->value, "_") ==0)
            {
                error_exit(SEM_ERROR_OTHERS, "Aritmetic operation with undescore as identifier");
            }
            sItem = stSearch(sym, ast->Lptr->value);
            if (sItem == NULL)
            {
                error_exit(SEM_ERROR_UNDEF, "Variable has not been already defined");
            }
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
        // printf("+++%s\n", value);
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
        while (tmp != NULL)
        {
            int half = getIDtype(tmp->Rptr, sym, fun);
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
        if (ast->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Rptr->value) == NULL && (ast->Lptr->Rptr->value)[0]!='\"')
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if((stSearch(sym, ast->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Rptr->value)->type != 2))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
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
            if (tmp->Rptr->type == N_IDENTIFIER)
            {
                if (strcmp(tmp->Rptr->value, "_")==0)
                {
                    error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
                }
            }
            tmp = tmp->Lptr;
        }
        if (ast->Lptr->Rptr->type == N_IDENTIFIER)
        {
            if (stSearch(sym, ast->Lptr->Rptr->value) == NULL && (ast->Lptr->Rptr->value)[0]!='\"')
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
            if((stSearch(sym, ast->Lptr->Rptr->value) != NULL && stSearch(sym, ast->Lptr->Rptr->value)->type != 2))
            {
                error_exit(SEM_ERROR_PARAMS, "Params not corresponding with call values");
            }
        }
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
        if (strcmp(ast->value, "_") ==0)
        {
            error_exit(SEM_ERROR_OTHERS, "Undescore can not be as a identifier");
         }
        SymTItem *Sitem = stSearch(sym, ast->value);
        if (Sitem == NULL)
        {
            error_exit(SEM_ERROR_UNDEF, "Variable not defined yet");
        }
        return Sitem->type;
        break;
    }
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
            int type = getIDtype(ast->Rptr->Rptr, sym, fun);
            Tree *tmp = ast->Rptr->Lptr;
            if (tmp->type != SEQ)
            {
                // printf("///%s\n", newvalue);
                newSym(tmp->value, type, newvalue, hide, forcnt, ifcnt, sym);
                newvalue = NULL;
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
                newSym(tmp->Rptr->value, (help[i] - '0'), newvalue, hide, forcnt, ifcnt, sym);
                newvalue = NULL;
                tmp = tmp->Lptr;
                i++;
            }
            break;

        case N_IDENT_INIT:;
            tmp = ast->Rptr->Lptr;
            int type1 = getIDtype(ast->Rptr->Rptr, sym, fun);
            SymTItem *sItem;

            if (tmp->type != SEQ)
            {
                sItem = stSearch(sym, tmp->value);
                if (sItem != NULL)
                {
                    sItem = searchdown(sItem, hide, forcnt, ifcnt);
                    if (sItem == NULL)
                    {
                        newSym(tmp->value, getIDtype(ast->Rptr->Rptr, sym, fun), newvalue, hide, forcnt, ifcnt, sym);
                        newvalue = NULL;
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
            // printf("pea\n");
            if (fItem->types != getIDtype(ast->Rptr->Lptr, sym, fun))
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
            int type2 = getIDtype(ast->Rptr->Lptr, sym, fun);
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
                type = getIDtype(tmp->Rptr->Rptr, sym, fun);
                tmp = tmp->Rptr->Lptr;
                newSym(tmp->value, type, newvalue, hide, forcnt, ifcnt, sym);
                newvalue = NULL;
            }
            tmp = ast->Rptr->Lptr->Lptr;
            compare(tmp->Rptr);
            if (getIDtype(tmp->Rptr->Lptr, sym, fun) != getIDtype(tmp->Rptr->Rptr, sym, fun))
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
            compare(tmp->Rptr);
            if ((getIDtype(tmp->Rptr->Lptr, sym, fun)) != (getIDtype(tmp->Rptr->Rptr, sym, fun)))
            {
                error_exit(SEM_ERROR_TYPE, "Operation with different data types");
            }
            tmp = ast->Rptr->Rptr;
            if (tmp->Rptr != NULL)
            {
                InFuncGo(tmp->Rptr, sym, fun, fname);
            }
            if (tmp->Lptr != NULL)
            {
                ifcnt++;
                InFuncGo(tmp->Lptr, sym, fun, fname);
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
            ifcnt = 0;
            forcnt = 0;
            hide = 0;
            newvalue = NULL;
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