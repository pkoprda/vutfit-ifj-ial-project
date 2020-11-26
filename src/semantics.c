#include "libmine.h"

// bool isNextPtr (Tree *ast, int i){
//     // lavy ptr, i == 0
//     if (i == LPTR){
//         return ast->Lptr != NULL;
//     }
//     // pravy ptr, i == 1
//     else{
//         return ast->Rptr != NULL;
//     }
// }

int old = 0;

void printhashtable(FunTable *fun){
    printf ("------------HASH TABLE--------------\n");
    for ( int i=0; i<FTsize; i++ ) {
    printf ("%i:",i);
    FunTItem* ptr = (*fun)[i];
    while ( ptr != NULL ) {
        printf (" (%s,%d,%d,%d)",ptr->key,ptr->count,ptr->retvar,ptr->types);
        /*if ( ptr != UNDEFPTR )
            cnt++;*/
        ptr = ptr->next;
    }
    printf ("\n");

    }
    printf ("------------------------------------\n");
    }

void printsymtable(FunTable *fun)

{
FunTItem *tmp = ftSearch(fun, "factorial");
    SymTable *sym = tmp->sym;
    printf ("------------SYMTABLE--------------\n");
    for ( int i=0; i<STsize; i++ ) {
    printf ("%i:",i);
    SymTItem* ptr = (*sym)[i];
    while ( ptr != NULL ) {
        printf (" (%s,%s,%d)",ptr->key,ptr->value,ptr->type);

        ptr = ptr->next;
    }
    printf ("\n"); 
    }
    printf ("------------------------------------\n");
    tmp = ftSearch(fun, "main");
    sym = tmp->sym;
    printf ("------------SYMTABLE--------------\n");
    for ( int i=0; i<STsize; i++ ) {
    printf ("%i:",i);
    SymTItem* ptr = (*sym)[i];
    while ( ptr != NULL ) {
        printf (" (%s,%s,%d)",ptr->key,ptr->value,ptr->type);

        ptr = ptr->next;
    }
    printf ("\n"); 

    }
    printf ("------------------------------------\n");
}

int cnt(Tree *ast, int i){
    Tree *tmp = ast;
    if (i == -1){
        i = 0;
        tmp = tmp->Rptr;
        while (tmp != NULL){
            i++;
            tmp = tmp->Lptr;
        }
    } else{
        tmp = tmp->Lptr;
        while (tmp != NULL){
            i++;
            tmp = tmp->Lptr;
        }
    }
    return i;
}

int getTypes(Tree *ast, int retvar, int count, SymTable *sym){
    Tree *tmp1 = ast->Lptr;
    Tree *tmp2 = ast->Rptr;
    int i = 0;
    int types = 0;
    int tmp = 0;
    while (count != retvar){
        if (tmp1->Rptr->type == N_PARAM_IDENT_INT){
            tmp = 1;
        } else if (tmp1->Rptr->type == N_PARAM_IDENT_STR){
            tmp = 3;
        } else if (tmp1->Rptr->type == N_PARAM_IDENT_FLOAT){
            tmp = 2;
        }
        types = types * 10 + tmp;
        newSym(tmp1->Rptr->value, tmp1->Rptr->type-4, NULL, sym);
        tmp1 = tmp1->Lptr;
        i++;
        count--;
    }
    while (retvar != 0){
        if (tmp2->Rptr->type == RETURN_TYPE_INT){
            tmp = 1;
        } else if (tmp2->Rptr->type == RETURN_TYPE_STR){
            tmp = 3;
        } else if (tmp2->Rptr->type == RETURN_TYPE_FLOAT){
            tmp = 2;
        }
        types = types * 10 + tmp;
        tmp2 = tmp2->Lptr;
        i++;
        retvar--;
    }
    return types;
}

// int statm(Tree *ast, )
// {
//     int ret = 0;
//     Tree *tmp = ast->Lptr;
//     while (tmp != NULL){
//         int ret = statm(ast);
//     }

//     return ret;
// }

void statm(Tree *ast, SymTable *sym)
{
    if (ast->Rptr->type >= N_PLUS && ast->Rptr->type <= N_DIV){
        statm(ast->Rptr, sym);
    }
    if (ast->Lptr->type >= N_PLUS && ast->Lptr->type <= N_DIV){
        statm(ast->Lptr, sym);
    }
    if (ast->Rptr->type >= N_LIT_INT && ast->Rptr->type <= N_LIT_STRING){
        int tmp = old;
        old = (ast->Rptr->type) - 7;
        if (tmp != old && tmp != 0){
            error_exit(5, "Operation with different data types");
        }
    }
    if (ast->Lptr->type >= N_LIT_INT && ast->Lptr->type <= N_LIT_STRING){
        int tmp = old;
        old = (ast->Lptr->type - 7);
        if (tmp != old && tmp != 0){
            error_exit(5, "Operation with different data types");
        }
    }
    if (ast->Rptr->type == N_IDENTIFIER){
        SymTItem *found = stSearch(sym, ast->Rptr->value);
        if (found == NULL){
            error_exit(3, "Variable not defined yet");
        }
        int tmp = old;
        old = found->type;
        if (tmp != old && tmp != 0){
            error_exit(5, "Operation with different data types");
        }
    }
    if (ast->Lptr->type == N_IDENTIFIER){
        SymTItem *found = stSearch(sym, ast->Lptr->value);
        if (found == NULL){
            error_exit(3, "Variable not defined yet");
        }
        int tmp = old;
        old = found->type;
        if (tmp != old && tmp != 0){
            error_exit(5, "Operation with different data types");
        }
    }
}

int getIDtype (Tree *ast, char *value, SymTable *sym)
{
    switch (ast->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT: ;
            value = ast->value;
            return (ast->type - 7);
            break;

        case N_MINUS:
        case N_PLUS:
        case N_MULL:
        case N_DIV: ;
            statm(ast, sym);
            int type = old;
            old = 0;
            return type;
            break;
        case N_INPUTI:
        case N_INPUTS:
        case N_INPUTF: ;
            printf("sem\n");
            return (ast->type - 14);
            break;
        case SEQ:
            return getIDtype(ast->Rptr, value, sym);
            break;
    }
    value = value;
    return 0;
 }

void InFuncGo (Tree *ast, SymTable *sym)
{
    if (ast->type == SEQ)
    {
        switch(ast->Rptr->type)
        {
            case N_IDENT_DEF: ;
                char *value = NULL;
                int type = getIDtype (ast->Rptr->Rptr, value, sym);
                Tree *tmp = ast->Rptr->Lptr;
                printf("tmp---%d\n", tmp->type);
                if (tmp->type != SEQ){
                    printf("ide ide--v if\n");
                    newSym(tmp->value, type,value,sym);
                    break;
                }
                while (tmp != NULL){
                    printf("ide ide--v seq\n");
                    newSym(tmp->Rptr->value, type, value, sym);
                    tmp = tmp->Lptr;
                }
                printf("ide ide--\n");
                break;
            case N_IDENT_INIT: ;
                char *value1 = NULL;
                Tree *tmp1 = ast->Rptr->Lptr;
                int type1 = getIDtype(ast->Rptr->Rptr, value1, sym);
                printf("type1----%d\n", type1);
                SymTItem *sItem;
                if (tmp1->type != SEQ){
                    sItem = stSearch(sym, tmp1->value);
                    if (sItem == NULL){
                        error_exit(3, "Variable not defined yet");
                    }
                    if (type1 != sItem->type){
                        error_exit(5, "Type of variable is not equal to statement");
                    }
                    break;
                }
                while (tmp1 != NULL){
                    printf("ide ide--v seq\n");
                    sItem = stSearch(sym, tmp1->Rptr->value);
                    if (type1 != sItem->type){
                        error_exit(5, "Type of variable is not equal to statement");
                    }
                    tmp1 = tmp1->Lptr;
                }
                break;
        }
    }
    if (ast->Lptr != NULL){
        InFuncGo(ast->Lptr, sym);
    } else{
        return;
    }
}

void FUN_def (Tree *ast, FunTable *fun){
    char *name = ast->value;
    SymTable *sym = (SymTable*) malloc(sizeof(SymTable));
    stInit(sym);
    if (strcmp(name, "main") == 0){
        newFun(fun, name, 0, 0, 0, sym);
    } else{
        int retvar = cnt(ast->Lptr, -1);
        int count = cnt(ast->Lptr, retvar);
        newFun(fun, name, retvar, count, 0, sym);
        FunTItem *fItem= ftSearch(fun, name);
        int types = getTypes(ast->Lptr, retvar, count, sym);
        fItem->types = types;
    }
    InFuncGo(ast->Rptr,sym);
}

void FuncLookup(Tree *ast, FunTable *fun){
    if (ast->Lptr == NULL){
        return;
    }
    ast = ast->Lptr;
    if (ast->type == SEQ){
        if (ast->Rptr->type == N_DEF_FUNC){
            FUN_def(ast->Rptr, fun);
        }
        FuncLookup(ast, fun);
    }
}

   

int semantics(){
    printf ("===================semantika================== \n \n");
    FunTable *fun = (FunTable*) malloc(sizeof(FunTable));
    ftInit(fun);
    FuncLookup(ast->Rptr, fun);

    printhashtable(fun);
    printsymtable(fun);
	
    printf ("===========koniec semantiky====================\n \n");

    return 0;

    
}