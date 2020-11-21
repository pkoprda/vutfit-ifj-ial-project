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

void semCheckIf (Tree *ast, FunTable *fun){
    Tree *tmp = ast->Lptr;
}

void ID_def (Tree *ast, FunTable *fun){
    char *name = ast->Lptr->value;
    int type = ast->Rptr->type;
    if (ast->Rptr->type >= 8 && ast->Rptr->type <= 8){
        char *value = ast->Rptr->value;

    }
}

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
            if (isNextPtr(ast, RPTR)){
                semCheck(ast->Rptr);
            }
            if (isNextPtr(ast, LPTR)){
                semCheck(ast->Lptr);
            }
        case N_IF:
            semCheckIf(ast, fun);
            break;
        case N_IDENT_DEF:
            ID_def(ast);
            break;
    }
}