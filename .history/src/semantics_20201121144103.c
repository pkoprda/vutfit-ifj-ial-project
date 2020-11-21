#include "libmine.h"

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
            semCheckIf(ast);
            break;
        case N_IDENT_DEF:
            ID_def(ast);
            break;
    }
}