#include "libmine.h"


int lexer(){
    char c, *buffer;
    int stav = STATE_START;
    while(!feof(stdin)){
        
        c = getc(stdin);

        switch(stav){

            // pociatocny stav automatu
            case STATE_START:

                // prazdne riadky preskakujem
                if(c == '\n' || c == '\t' || c == ' ' || c == EOF);

                // delenie alebo komentar
                else if(c == '/'){
                    c = getc(stdin);
                    if(c == '/' || c == '*'){
                        ungetc(c,stdin);
                        stav = STATE_COMMENTS;
                    } else{
                        ungetc(c,stdin);
                        initToken(TOKEN_DIV, NULL);
                    }
                }

                // identifikator alebo klucove slovo             
                else if(isalpha(c) || c == '_') {
                    ungetc(c, stdin);
                    stav = STATE_IDENTIF;
                }

                // cislo - int alebo float64
                else if(isdigit(c)) {
                    ungetc(c, stdin);
                    stav = STATE_NUMBER;
                }

                // zaciatok stringu
                else if(c == '"') {
                    ungetc(c, stdin);
                    stav = STATE_STRING;
                }

                // definicia premennej
                else if(c == ':'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(IDENTIF_DEF, NULL);
                    }
                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                }

                // operatory
                else if(c == '+') initToken(TOKEN_PLUS, NULL);
                else if(c == '-') initToken(TOKEN_MINUS, NULL);
                else if(c == '*') initToken(TOKEN_MUL, NULL);
                else if(c == '<'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(TOKEN_LESS_EQUAL, NULL);
                    }
                    ungetc(c,stdin);
                    initToken(TOKEN_LESS, NULL);
                } else if(c == '>'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(TOKEN_MORE_EQUAL, NULL);
                    }
                    ungetc(c,stdin);
                    initToken(TOKEN_MORE, NULL);
                } else if(c == '='){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(TOKEN_EQUAL, NULL);
                    }
                    // inicializacia premennej
                    ungetc(c,stdin);
                    initToken(IDENTIF_INIT, NULL);
                } else if(c == '!'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(TOKEN_NOT_EQUAL, NULL);
                    }
                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                }
                
                // oddelovace
                else if(c == ',') initToken(TOKEN_COMMA, NULL);
                else if(c == ';') initToken(TOKEN_SEMICOLON, NULL);
                else if(c == '(') initToken(TOKEN_ROUND_LBRACKET, NULL);
                else if(c == ')') initToken(TOKEN_ROUND_RBRACKET, NULL);
                else if(c == '{') initToken(TOKEN_CURLY_LBRACKET, NULL);
                else if(c == '}') initToken(TOKEN_CURLY_RBRACKET, NULL);

                // nezname znaky
                else { 
                    debug_print("\n%c %d",c,c);
                    error_exit(LEX_ERROR,"Lexikalna chyba!"); 
                }

                break;

            // riadkove a blokove komentare
            case STATE_COMMENTS:

                if(c == '/'){
                    // Jednoriadkovy komentar
                    while((c = getc(stdin)) != '\n');
                    stav = STATE_START;
                } else if (c == '*'){
                    // Blokovy komentar
                    char nextChar;
                    while((c = getc(stdin))){
                        nextChar = getc(stdin);
                        if(c == '*' && nextChar == '/'){ break; }

                        if(feof(stdin)){ error_exit(LEX_ERROR,"Lexikalna chyba!"); }
                    }
                }
                break;

            case STATE_IDENTIF:
                ungetc(c, stdin);
                buffer = initBuffer();
                buffer = loadWord(buffer);

                // keywordy a funkcie
                if(MIN_LEN_KEYWORD <= strlen(buffer) && strlen(buffer) <= MAX_LEN_KEYWORD){
                    if(!strcmp(buffer, "else"))           initToken(KEYWORD_ELSE, NULL);
                    else if(!strcmp(buffer, "float64"))   initToken(KEYWORD_FLOAT64, NULL);
                    else if(!strcmp(buffer, "for"))       initToken(KEYWORD_FOR, NULL); 
                    else if(!strcmp(buffer, "func"))      initToken(KEYWORD_FUNC, NULL);
                    else if(!strcmp(buffer, "if"))        initToken(KEYWORD_IF, NULL);
                    else if(!strcmp(buffer, "int"))       initToken(KEYWORD_INT, NULL);
                    else if(!strcmp(buffer, "package"))   initToken(KEYWORD_PACKAGE, NULL);
                    else if(!strcmp(buffer, "return"))    initToken(KEYWORD_RETURN, NULL);
                    else if(!strcmp(buffer, "string"))    initToken(KEYWORD_STRING, NULL);
                    else if(!strcmp(buffer, "inputs"))    initToken(FUNC_INPUTS, NULL);
                    else if(!strcmp(buffer, "inputi"))    initToken(FUNC_INPUTI, NULL);
                    else if(!strcmp(buffer, "inputf"))    initToken(FUNC_INPUTF, NULL);
                    else if(!strcmp(buffer, "print"))     initToken(FUNC_PRINT, NULL);
                    else if(!strcmp(buffer, "int2float")) initToken(FUNC_INT2FLOAT, NULL);
                    else if(!strcmp(buffer, "float2int")) initToken(FUNC_FLOAT2INT, NULL);
                    else if(!strcmp(buffer, "len"))       initToken(FUNC_LEN, NULL);
                    else if(!strcmp(buffer, "substr"))    initToken(FUNC_SUBSTR, NULL);
                    else if(!strcmp(buffer, "ord"))       initToken(FUNC_ORD, NULL);
                    else if(!strcmp(buffer, "chr"))       initToken(FUNC_CHR, NULL);
                    else{
                        // identifikator
                        initToken(TOKEN_IDENTIF, buffer);
                        debug_print("IDENTIFIER: %s", buffer);
                        break; 
                    }
                    debug_print("KEYWORD: %s", buffer);
                } 
                
                // identifikator
                else{
                    initToken(TOKEN_IDENTIF, buffer);
                    debug_print("IDENTIFIER: %s", buffer);
                }
                stav = STATE_START;
                break;

            case STATE_NUMBER:
                ungetc(c, stdin);          
                buffer = initBuffer();
                buffer = loadNumber(buffer);
                initToken(TOKEN_INT, buffer);
                debug_print("NUMBER: %s", buffer);
                stav = STATE_START;
                break;
            
            case STATE_STRING:
                buffer = initBuffer();
                while(1){   
                    c = getc(stdin);

                    // koniec stringu
                    if(c == '"'){ 
                        stav = STATE_START;
                        break;
                    }
                    
                    // ASCII hodnota mensia ako 32
                    else if(c < 32){
                        error_exit(LEX_ERROR,"Lexikalna chyba!");
                    }

                    // escape sekvencie
                    else if(c == '\\'){
                        c = getc(stdin);

                        //  '\"'  '\n'  '\t'  '\\'
                        if(c == '"' || c == 'n' || c == 't' || c == '\\'){
                            continue;
                        } 
                        
                        // hexadecimalne escape sekvencia \xhh, kde hh je cislo od 00 do FF
                        else if(c == 'x'){
                            c = getc(stdin);
                            if(isxdigit(c)){
                                c = getc(stdin);
                                if(isxdigit(c)){
                                    continue;
                                } else {
                                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                                }
                            } else {
                                error_exit(LEX_ERROR,"Lexikalna chyba!");
                            }
                        } else {
                            error_exit(LEX_ERROR,"Lexikalna chyba!");
                        } 
                    }
                }
                break;

            default:
                error_exit(LEX_ERROR,"Lexikalna chyba!");
        }
    }
    
    return SCANNER_OK;
}