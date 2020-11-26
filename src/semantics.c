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
        newSym(tmp1->Rptr->value, tmp1->Rptr->type, NULL, sym);
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
}

// void ID_def (Tree *ast, FunTable *fun){
//     char *name = ast->Lptr->value;
//     int type = ast->Rptr->type;
//     SymTablePtr *tmp = searchSym(fun, name);
//     if (tmp == NULL){
//         newSym(fun, name, type);
//     } else{
//         error_exit(3, "Variable is defined yet");
//     }
// }

// void semCheck (Tree *ast){
//     switch (ast->type){
//         case SEQ:
//             if (isNextPtr(ast, LPTR)){
//                 semCheck(ast->Lptr);
//             }
//             if (isNextPtr(ast, RPTR)){
//                 semCheck(ast->Rptr);
//             }
//             break;
//         case PACKAGE_MAIN:
//             if (isNextPtr(ast, LPTR)){
//                 semCheck(ast->Lptr);
//             }
//             break;
//         case N_DEF_FUNC:
//             FUN_def(ast, fun);
//             break;
//         case N_IF:
//             // msemCheckIf(ast, fun);
//             break;
//         case N_IDENT_DEF:
//             ID_def(ast, fun);
//             semCheck(ast->Lptr);
//             break;
//         case N_IDENT_INIT:
//             break;
//             //ID_init(ast, fun);
//             break;
//     }
// }

void go(Tree *ast, FunTable *fun){
    if (ast->Lptr == NULL){
        return;
    }
    ast = ast->Lptr;
    if (ast->type == SEQ){
        if (ast->Rptr->type == N_DEF_FUNC){
            FUN_def(ast->Rptr, fun);
            go(ast, fun);
        }
    }
}

int semantics(){
    FunTable *fun = (FunTable*) malloc(sizeof(FunTable));
    ftInit(fun);
    go(ast->Rptr, fun);
    return 0;
}