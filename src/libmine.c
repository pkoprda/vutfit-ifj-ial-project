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

char* loadWord(char* bufferPtr){
    char *bufferStart = bufferPtr;
    char c;
    char *bufferNew;
    size_t lenLineMax = LINE_BUFF_LEN, len = lenLineMax;

    // prvy charakter moze byt iba pismeno alebo podtrzitko
    c = getc(stdin);
    *bufferPtr = c;
    bufferPtr++;
    
    while(1){
        c = getc(stdin);
        if(isalpha(c) || isdigit(c) || c == '_'){
            if(--len == 0){
                len = lenLineMax;
                bufferNew = (char*) realloc(bufferStart, lenLineMax *= 2);
                if(!bufferNew){
                    free(bufferStart);
                    error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
                }

                bufferPtr = bufferNew + (bufferPtr - bufferStart);
                bufferStart = bufferNew;
            }
            *bufferPtr = c;
            bufferPtr++;
        } else { break;}
    }

    *bufferPtr = '\0';
    return bufferStart;
}

char* loadNumber(char *bufferPtr){
    char *bufferStart = bufferPtr;
    char c;
    char *bufferNew;
    size_t lenLineMax = LINE_BUFF_LEN, len = lenLineMax;

    c = getc(stdin);
    *bufferPtr = c;
    bufferPtr++;

    while(1){
        c = getc(stdin);
        if(isdigit(c)){
            if(--len == 0){
                len = lenLineMax;
                bufferNew = (char*) realloc(bufferStart, lenLineMax *= 2);
                if(!bufferNew){
                    free(bufferStart);
                    error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
                }

                bufferPtr = bufferNew + (bufferPtr - bufferStart);
                bufferStart = bufferNew;
            }
            *bufferPtr = c;
            bufferPtr++;
        } else { break;}
    }

    *bufferPtr = '\0';
    return bufferStart;
}