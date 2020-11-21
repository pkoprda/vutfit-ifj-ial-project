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

void semCheckFunc(Tree *ast){
    
}

void semCheck(Tree *ast){
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
            semCheckFunc(ast);


    }
}