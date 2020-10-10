#include "libmine.h"


int lexer(){
    char c;
    int stav = START;
    while(!feof(stdin)){
        
        c = getc(stdin);
                
        switch(stav){

            // pociatocny stav automatu
            case START:

                // prazdne riadky preskakujem
                if(c == '\n' || c == ' ' || c == EOF);

                // delenie alebo komentar
                else if(c == '/'){
                    c = getc(stdin);
                    if(c == '/' || c == '*'){
                        ungetc(c,stdin);
                        stav = COMMENTS;
                    } else{
                        ungetc(c,stdin);
                        initToken(DIV, NULL);
                    }
                }

                // identifikator alebo klucove slovo             
                else if(isalpha(c) || c == '_') stav = IDENTIF;

                // cislo - int alebo float64
                else if(isdigit(c)) stav = NUMBER;

                // zaciatok stringu
                else if(c == '"') stav = STRING;

                // definicia premennej
                else if(c == ':'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(IDENTIF_DEF, NULL);
                    }
                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                }

                // operatory
                else if(c == '+') initToken(PLUS, NULL);
                else if(c == '-') initToken(MINUS, NULL);
                else if(c == '*') initToken(MUL, NULL);
                else if(c == '<'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(LESS_EQUAL, NULL);
                    }
                    ungetc(c,stdin);
                    initToken(LESS, NULL);
                } else if(c == '>'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(MORE_EQUAL, NULL);
                    }
                    ungetc(c,stdin);
                    initToken(MORE, NULL);
                } else if(c == '='){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(EQUAL, NULL);
                    }
                    // inicializacia premennej
                    ungetc(c,stdin);
                    initToken(IDENTIF_INIT, NULL);
                } else if(c == '!'){
                    c = getc(stdin);
                    if(c == '='){
                        initToken(NOT_EQUAL, NULL);
                    }
                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                }
                
                // oddelovace
                else if(c == ',') initToken(COMMA, NULL);
                else if(c == ';') initToken(SEMICOLON, NULL);
                else if(c == '(') initToken(ROUND_LBRACKET, NULL);
                else if(c == ')') initToken(ROUND_RBRACKET, NULL);
                else if(c == '{') initToken(CURLY_LBRACKET, NULL);
                else if(c == '}') initToken(CURLY_RBRACKET, NULL);

                // nezname znaky
                else { error_exit(LEX_ERROR,"Lexikalna chyba!"); }

                break;

            // riadkove a blokove komentare
            case COMMENTS:

                if(c == '/'){
                    // Jednoriadkovy komentar
                    while((c = getc(stdin)) != '\n');
                    stav = START;
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

            case IDENTIF:
                initBuffer();
                ungetc(c, stdin);
                break;

            case NUMBER:
                break;
            
            case STRING:
                while(1){   

                    // koniec stringu
                    if(c == '"'){ break;}
                    
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
                    c = getc(stdin);
                }
                break;

            case OPERATORS:
                break;

            default:
                error_exit(LEX_ERROR,"Lexikalna chyba!");
        }
    }
    
    return SCANNER_OK;
}