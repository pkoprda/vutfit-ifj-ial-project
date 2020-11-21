#include "libmine.h"

bool isNextPtr (Tree *ast, int i){
    // lavy ptr, i == 0
    if (i == 0){
        return ast->Lptr != NULL;
    }
    // pravy ptr, i == 1
    else{
        return ast->Rptr != NULL;
    }
}

void semCheckIf (Tree *ast){
    Tree *tmp = ast->Lptr;
}

void ID_def (Tree *ast){
    char *name = ast->Lptr->value;
    char *value = ast->Rptr->value;
    int type = ast->Rptr->type;
}

void semCheck (Tree *ast){
    switch (ast->type){
        case SEQ:
            if (isNextPtr(ast, 0)){
                semCheck(ast->Lptr);
            }
            if (isNextPtr(ast, 1)){
                semCheck(ast->Rptr);
            }
            break;
        case PACKAGE_MAIN:
            if (isNextPtr(ast, 0)){
                semCheck(ast->Lptr);
            }
            break;
        case N_DEF_FUNC:
            if (isNextPtr(ast, 1)){
                semCheck(ast->Rptr);
            }
            if (isNextPtr(ast, 0)){
                semCheck(ast->Lptr);
            }
        case N_IF:
            semCheckIf(ast);
            break;
        case N_IDENT_DEF:
            ID_def(ast);
    }
}