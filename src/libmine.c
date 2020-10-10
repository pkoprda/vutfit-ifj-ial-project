#include "libmine.h"

char* initBuffer(){
    char* buffer = (char*) calloc(LINE_BUFF_LEN, LINE_BUFF_LEN * sizeof(char));
    if(!buffer){
        error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
    }
    return buffer;
}

void freeBuffer(char* buffer){
    free(buffer);
    buffer = NULL;
}

TokenPtr initToken(int type, char* value){
    TokenPtr token = malloc(sizeof(struct tToken));
    if(!token){
        error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
    }
    token->type = type;
    token->value = value;
    return token;
}