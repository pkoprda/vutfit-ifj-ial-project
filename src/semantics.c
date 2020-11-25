#include "libmine.h"

FunTable *fun;

bool isNextPtr (Tree *ast, int i){
    // lavy ptr, i == 0
    if (i == LPTR){
        return ast->Lptr != NULL;
    }
    // pravy ptr, i == 1
    else{
        return ast->Rptr != NULL;
    }
}

// void semCheckIf (Tree *ast, FunTable *fun){
//     Tree *tmp = ast->Lptr;
// }

// void findParRet(Tree *ast){
//     int i = 0;
//     char name[] = "000000";
//     if (ast->Lptr->type == N_PARAMS_RETURNTYPES){
//         Tree *tmp = ast->Lptr;
//         while (tmp != NULL){

//         }
//     }
// }

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

int getTypes(Tree *ast, int retvar, int count, FunTable *fun){
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
        newSym(fun, tmp1->Rptr->value, tmp1->Rptr->type);
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

void FUN_def (Tree *ast, FunTable *fun){
    char *name = ast->value;
    FunTablePtr *tmp = searchFun(fun, name);
    if (tmp != NULL){
        error_exit(3, "Function is defined yet");
    }
    newFun(fun, name);
    if (strcmp(name, "main") == 0){
        fun->act->count = 0;
        fun->act->retvar = 0;
        fun->act->types = 0;
    } else{
        fun->act->retvar = cnt(ast->Lptr, -1);
        fun->act->count = cnt(ast->Lptr, fun->act->retvar);
        fun->act->types = getTypes(ast->Lptr, fun->act->retvar, fun->act->count, fun);
    }
}

void ID_def (Tree *ast, FunTable *fun){
    char *name = ast->Lptr->value;
    int type = ast->Rptr->type;
    SymTablePtr *tmp = searchSym(fun, name);
    if (tmp == NULL){
        newSym(fun, name, type);
    } else{
        error_exit(3, "Variable is defined yet");
    }
}

// void ID_init (Tree *ast, FunTable *fun){
    
// }

void semCheck (Tree *ast){
    switch (ast->type){
        case SEQ:
            if (isNextPtr(ast, LPTR)){
                semCheck(ast->Lptr);
            }
            if (isNextPtr(ast, RPTR)){
                semCheck(ast->Rptr);
            }
            break;
        case PACKAGE_MAIN:
            if (isNextPtr(ast, LPTR)){
                semCheck(ast->Lptr);
            }
            break;
        case N_DEF_FUNC:
            // if (isNextPtr(ast, RPTR)){
            //     semCheck(ast->Rptr);
            // }
            // if (isNextPtr(ast, LPTR)){
            //     semCheck(ast->Lptr);
            // }
            // FUN_def(ast, fun);
            break;
        case N_IF:
            // msemCheckIf(ast, fun);
            break;
        case N_IDENT_DEF:
            ID_def(ast, fun);
            semCheck(ast->Lptr);
            break;
        case N_IDENT_INIT:
            //ID_init(ast, fun);
            break;
    }
}

int semantics(){
    semCheck(ast);
    return 0;
}