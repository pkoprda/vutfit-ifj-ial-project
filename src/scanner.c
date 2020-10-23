#include "libmine.h"


int lexer(){
    char c, firstDigit; //*buffer; //*bufferStart, *bufferNew;
    //size_t lenLineMax, len;
    int stav = STATE_START;
    string bufferHelp;
    string* buffer = &bufferHelp;

    while(!feof(stdin)){
        
        c = getc(stdin);
        switch(stav){

            // pociatocny stav automatu
            case STATE_START:

                // \r iba pre suboru s CRLF
                if(c == '\r' || c == EOF);

                // novy riadok
                else if(c == '\n') { 
                    debug_print("EOL");
                    initToken(TOKEN_EOL, NULL); 
                }

                // medzera
                else if(c == ' ' || c == '\t') { 
                    debug_print("WHITESPACE");    
                    initToken(TOKEN_WHITESPACE, NULL); 
                }
                

                // delenie alebo komentar
                else if(c == '/'){
                    c = getc(stdin);
                    if(c == '/' || c == '*'){
                        ungetc(c,stdin);
                        stav = STATE_COMMENTS;
                    } else{
                        ungetc(c,stdin);
                        debug_print("DIVISION");
                        initToken(TOKEN_DIV, NULL);
                    }
                }

                // identifikator alebo klucove slovo             
                else if(isalpha(c) || c == '_') {
                    initString(buffer);
                    buffer = initBuffer();
                    addChar(buffer, c);
                    stav = STATE_IDENTIF;
                }

                // cislo - int alebo float64
                else if(isdigit(c)) {
                    firstDigit = c;
                    initString(buffer);
                    buffer = initBuffer();
                    addChar(buffer, c);
                    stav = STATE_NUMBER;
                }

                // zaciatok stringu
                else if(c == '"') {
                    ungetc(c, stdin);
                    initString(buffer);
                    buffer = initBuffer();
                    addChar(buffer, c);
                    stav = STATE_STRING;
                }

                // definicia premennej
                else if(c == ':'){
                    c = getc(stdin);
                    if(c == '='){
                        debug_print("IDENTIF_DEF");
                        initToken(IDENTIF_DEF, NULL);
                    } else{
                        error_exit(LEX_ERROR,"Lexikalna chyba!");
                    }
                    
                }

                // operatory
                else if(c == '+'){
                    debug_print("PLUS");
                    initToken(TOKEN_PLUS, NULL);
                }
                else if(c == '-'){
                    debug_print("MINUS");
                    initToken(TOKEN_MINUS, NULL);
                }
                else if(c == '*'){
                    debug_print("MULTIPLE");
                    initToken(TOKEN_MUL, NULL);
                }
                else if(c == '<'){
                    c = getc(stdin);
                    if(c == '='){
                        debug_print("LESS EQUAL");
                        initToken(TOKEN_LESS_EQUAL, NULL);
                    } else {
                        ungetc(c,stdin);
                        debug_print("LESS");
                        initToken(TOKEN_LESS, NULL);
                    }
                    
                } else if(c == '>'){
                    c = getc(stdin);
                    if(c == '='){
                        debug_print("MORE EQUAL");
                        initToken(TOKEN_MORE_EQUAL, NULL);
                    } else {
                        ungetc(c,stdin);
                        debug_print("MORE");
                        initToken(TOKEN_MORE, NULL);
                    }
                    
                } else if(c == '='){
                    c = getc(stdin);
                    if(c == '='){
                        debug_print("EQUAL");
                        initToken(TOKEN_EQUAL, NULL);
                    } else {
                        // inicializacia premennej
                        ungetc(c,stdin);
                        debug_print("IDENTIF_INIT");
                        initToken(IDENTIF_INIT, NULL);
                    }
                    
                } else if(c == '!'){
                    c = getc(stdin);
                    if(c == '='){
                        debug_print("NOT EQUAL");
                        initToken(TOKEN_NOT_EQUAL, NULL);
                    } else {
                        error_exit(LEX_ERROR,"Lexikalna chyba!");
                    }
                    
                }
                
                // oddelovace
                else if(c == ','){
                    debug_print("COMMA");
                    initToken(TOKEN_COMMA, NULL);
                }
                else if(c == ';'){
                    debug_print("SEMICOLON");
                    initToken(TOKEN_SEMICOLON, NULL);
                }
                else if(c == '('){
                    debug_print("LEFT ROUND BRACKET");
                    initToken(TOKEN_ROUND_LBRACKET, NULL);
                }
                else if(c == ')'){
                    debug_print("RIGHT ROUND BRACKET");
                    initToken(TOKEN_ROUND_RBRACKET, NULL);
                }
                else if(c == '{'){
                    debug_print("LEFT CURLY BRACKET");
                    initToken(TOKEN_CURLY_LBRACKET, NULL);
                }
                else if(c == '}'){
                    debug_print("RIGHT CURLY BRACKET");
                    initToken(TOKEN_CURLY_RBRACKET, NULL);
                }

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
                    while(1){
                        c = getc(stdin);
                        if(c == '*'){
                            c = getc(stdin);
                            if(c == '/'){
                                stav = STATE_START;
                                break;
                            } else{
                                ungetc(c, stdin);
                            }                           
                        }

                        if(feof(stdin)){ error_exit(LEX_ERROR,"Lexikalna chyba!"); }
                    }
                }
                break;

            case STATE_IDENTIF:
                
                while(1){                    
                    if(isalpha(c) || isdigit(c) || c == '_'){
                        //debug_print("%c",c);
                        addChar(buffer, c);
                        c = getc(stdin);
                    } else {
                        ungetc(c, stdin);
                        break;
                    }
                }

                // keywordy a funkcie
                if(MIN_LEN_KEYWORD <= strlen(buffer->str) && strlen(buffer->str) <= MAX_LEN_KEYWORD){
                    if(!strcmp(buffer->str, "else"))           initToken(KEYWORD_ELSE, NULL);
                    else if(!strcmp(buffer->str, "float64"))   initToken(KEYWORD_FLOAT64, NULL);
                    else if(!strcmp(buffer->str, "for"))       initToken(KEYWORD_FOR, NULL); 
                    else if(!strcmp(buffer->str, "func"))      initToken(KEYWORD_FUNC, NULL);
                    else if(!strcmp(buffer->str, "if"))        initToken(KEYWORD_IF, NULL);
                    else if(!strcmp(buffer->str, "int"))       initToken(KEYWORD_INT, NULL);
                    else if(!strcmp(buffer->str, "package"))   initToken(KEYWORD_PACKAGE, NULL);
                    else if(!strcmp(buffer->str, "return"))    initToken(KEYWORD_RETURN, NULL);
                    else if(!strcmp(buffer->str, "string"))    initToken(KEYWORD_STRING, NULL);
                    else if(!strcmp(buffer->str, "inputs"))    initToken(FUNC_INPUTS, NULL);
                    else if(!strcmp(buffer->str, "inputi"))    initToken(FUNC_INPUTI, NULL);
                    else if(!strcmp(buffer->str, "inputf"))    initToken(FUNC_INPUTF, NULL);
                    else if(!strcmp(buffer->str, "print"))     initToken(FUNC_PRINT, NULL);
                    else if(!strcmp(buffer->str, "int2float")) initToken(FUNC_INT2FLOAT, NULL);
                    else if(!strcmp(buffer->str, "float2int")) initToken(FUNC_FLOAT2INT, NULL);
                    else if(!strcmp(buffer->str, "len"))       initToken(FUNC_LEN, NULL);
                    else if(!strcmp(buffer->str, "substr"))    initToken(FUNC_SUBSTR, NULL);
                    else if(!strcmp(buffer->str, "ord"))       initToken(FUNC_ORD, NULL);
                    else if(!strcmp(buffer->str, "chr"))       initToken(FUNC_CHR, NULL);
                    else {
                        // identifikator
                        initToken(TOKEN_IDENTIF, buffer->str);
                        debug_print("IDENTIFIER: %s", buffer->str);
                        clearBuffer(buffer);
                        freeBuffer(buffer);
                        stav = STATE_START;
                        break;
                    }
                } 
                
                // identifikator
                else {
                    initToken(TOKEN_IDENTIF, buffer->str);
                    debug_print("IDENTIFIER: %s", buffer->str);
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    stav = STATE_START;
                    break;
                }
                debug_print("KEYWORD: %s", buffer->str);
                clearBuffer(buffer);
                freeBuffer(buffer);
                stav = STATE_START;
                break;

            case STATE_NUMBER:

                if(isdigit(c)){
                    if(firstDigit == '0'){
                        error_exit(LEX_ERROR, "Lexikalna chyba!");
                    }
                    addChar(buffer, c);                    
                    stav = STATE_NUMBER;
                } else if(c == '.'){
                    addChar(buffer, c);
                    c = getc(stdin);      
                    if(isdigit(c)){
                        addChar(buffer, c);
                        stav = STATE_FLOAT_NUMBER;
                    } else {
                        error_exit(LEX_ERROR, "Lexikalna chyba!");
                    }
                } else if(c == 'e' || c == 'E'){
                    stav = STATE_EXPONENT_NUMBER;
                } else {
                    ungetc(c, stdin);
                    debug_print("NUMBER INT: %s", buffer->str);
                    initToken(TOKEN_INT, buffer->str);
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    stav = STATE_START;
                }
                
                break;

            case STATE_FLOAT_NUMBER:
                
                addChar(buffer, c);
                if(isdigit(c)){    
                    stav = STATE_FLOAT_NUMBER;
                } else if(c == 'e' || c == 'E'){
                    stav = STATE_EXPONENT_NUMBER;
                } else {
                    ungetc(c, stdin);
                    debug_print("NUMBER FLOAT: %s", buffer->str);
                    initToken(TOKEN_FLOAT, buffer->str);
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    stav = STATE_START;
                }
                break;

            case STATE_EXPONENT_NUMBER:
                if(c == '+' || c == '-'){
                    addChar(buffer, c);
                    c = getc(stdin);
                    if(isdigit(c)){
                        addChar(buffer, c);
                        stav = STATE_EXPONENT_NUMBER_FINAL;
                    } else {
                        error_exit(LEX_ERROR, "Lexikalna chyba!");
                    }
                } else if(isdigit(c)){
                    addChar(buffer, c);
                    stav = STATE_EXPONENT_NUMBER_FINAL;
                } else {
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    error_exit(LEX_ERROR, "Lexikalna chyba!");
                }
                break;
            
            case STATE_EXPONENT_NUMBER_FINAL:
                addChar(buffer, c);
                if(isdigit(c)){
                    stav = STATE_EXPONENT_NUMBER_FINAL;
                } else {      
                    ungetc(c, stdin);
                    debug_print("NUMBER FLOAT: %s", buffer->str);
                    initToken(TOKEN_FLOAT, buffer->str);
                    clearBuffer(buffer);
                    freeBuffer(buffer);
                    stav = STATE_START;
                }
                break;

            case STATE_STRING:
                while(1){   
                    c = getc(stdin);

                    // koniec stringu
                    if(c == '"'){
                        addChar(buffer, c);
                        debug_print("STRING: %s", buffer->str);
                        initToken(TOKEN_STRING, buffer->str);
                        clearBuffer(buffer);
                        freeBuffer(buffer);
                        stav = STATE_START;
                        break;
                    }
                    
                    // ASCII hodnota mensia ako 32
                    else if(c < 32){
                        clearBuffer(buffer);
                        freeBuffer(buffer);
                        error_exit(LEX_ERROR,"Lexikalna chyba!");
                    }

                    // escape sekvencie
                    else if(c == '\\'){
                        addChar(buffer, c);
                        c = getc(stdin);

                        //  '\"'  '\n'  '\t'  '\\'
                        if(c == '"' || c == 'n' || c == 't' || c == '\\'){
                            addChar(buffer, c);
                            continue;
                        } 
                        
                        // hexadecimalne escape sekvencia \xhh, kde hh je cislo od 00 do FF
                        else if(c == 'x'){
                            addChar(buffer, c);
                            c = getc(stdin);
                            if(isxdigit(c)){
                                addChar(buffer, c);
                                c = getc(stdin);
                                if(isxdigit(c)){
                                    addChar(buffer, c);
                                    continue;
                                } else {
                                    clearBuffer(buffer);
                                    freeBuffer(buffer);
                                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                                }
                            } else {
                                clearBuffer(buffer);
                                freeBuffer(buffer);
                                error_exit(LEX_ERROR,"Lexikalna chyba!");
                            }
                        } else {
                            clearBuffer(buffer);
                            freeBuffer(buffer);
                            error_exit(LEX_ERROR,"Lexikalna chyba!");
                        } 
                    } 

                    else {
                        addChar(buffer, c);
                    }
                }

                break;

            default:
                error_exit(LEX_ERROR,"Lexikalna chyba!");
        }      
    }
    
    return SCANNER_OK;
}