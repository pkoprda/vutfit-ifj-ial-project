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
//char *A[FTsize] = {NULL};

int cnt(Tree *ast, int i)
{
    Tree *tmp = ast;
    if(i == -1)
    {
        i = 0;
        tmp = tmp->Rptr;
        while(tmp != NULL)
        {
            i++;
            tmp = tmp->Lptr;
        }
    } 
    else
    {
        tmp = tmp->Lptr;
        while(tmp != NULL){
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
    
    while(count != retvar)
    {
        if(tmp1->Rptr->type == N_PARAM_IDENT_INT){
            tmp = 1;
        } else if(tmp1->Rptr->type == N_PARAM_IDENT_STR){
            tmp = 3;
        } else if(tmp1->Rptr->type == N_PARAM_IDENT_FLOAT){
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

int getIDtype(Tree *ast, char *value, SymTable *sym)
{
    switch(ast->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT: ;
            value = ast->value;
            return(ast->type - 7);
            break;

        case N_MINUS:
        case N_PLUS:
        case N_MULL:
        case N_DIV: ;
            //statm(ast, sym);
            int type = old;
            old = 0;
            return type;
            break;

        case N_INPUTI:
        case N_INPUTS:
        case N_INPUTF: ;
            stdout_print("sem\n");
            return(ast->type - 14);
            break;

        case SEQ:
            return getIDtype(ast->Rptr, value, sym);
            break;
    }

    value = value;
    return 0;
 }

void FUN_def(Tree *ast, FunTable *fun)
{
    char *name = ast->value;
    SymTable *sym = (SymTable*) malloc(sizeof(SymTable));
    stInit(sym);

    if(strcmp(name, "main") == 0)
    {
        newFun(fun, name, 0, 0, 0, sym);
    } 
    else
    {
        int retvar = cnt(ast->Lptr, -1);
        int count = cnt(ast->Lptr, retvar);
        newFun(fun, name, retvar, count, 0, sym);
        FunTItem *fItem= ftSearch(fun, name);
        int types = getTypes(ast->Lptr, retvar, count, sym);
        fItem->types = types;
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

/*void printhashtable(FunTable *fun)
{
    stdout_print("------------HASH TABLE--------------\n");

    for(int i = 0; i < FTsize; i++ )
    {
        stdout_print("%i:",i);
        FunTItem* ptr = (*fun)[i];

        while (ptr != NULL)
        {
            stdout_print("(%s,%d,%d,%d)",ptr->key,ptr->count,ptr->retvar,ptr->types);
            A[i] = ptr->key; 
            printf("xx %s \n", A[i]);
            ptr = ptr->next;
        }
        //stdout_print("\n");
    }

    stdout_print("------------------------------------\n");
} */
 /*
void printsymtable(FunTable *fun)
{
    


    stdout_print("------------SYMTABLE--------------\n");

    for ( int i = 0; i < STsize; i++ ) {
        stdout_print("%i:", i); 
        for(int x=0;x<FTsize; x++)
        {
        //printf("A je %s \n", A[x]);
        if (A[x] != NULL) 
        {
        FunTItem *tmp = ftSearch(fun, A[x]);
        SymTable *sym = tmp->sym;
        SymTItem* ptr = (*sym)[i];

        while(ptr != NULL){
            stdout_print(" (%s,%s,%d)",ptr->key, ptr->value, ptr->type);
            ptr = ptr->next;
        }
        sym = NULL;
        tmp = NULL;
        ptr = NULL;
        }   
        

        }
        //FunTItem *tmp = ftSearch(fun, "factorial");
        //SymTable *sym = tmp->sym;
       // SymTItem* ptr = (*sym)[i];

        //while(ptr != NULL){
         //   stdout_print(" (%s,%s,%d)",ptr->key, ptr->value, ptr->type);
         //   ptr = ptr->next;
        }
    }   

    stdout_print("------------------------------------\n");

}   */ 

int semantics(){
    //stdout_print("===================semantika================== \n \n");
    FunTable *fun = (FunTable*) malloc(sizeof(FunTable));
    ftInit(fun);
    FuncLookup(ast->Rptr, fun);

    //printhashtable(fun);
    //printsymtable(fun);

    //stdout_print("===========koniec semantiky====================\n \n");

    return 0;
   
}