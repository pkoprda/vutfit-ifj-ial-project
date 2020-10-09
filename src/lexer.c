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
                if(c == '\n' || c == ' ');

                // delenie alebo komentar
                else if(c == '/'){
                    c = getc(stdin);
                    if(c == '/' || c == '*'){
                        ungetc(c,stdin);
                        stav = COMMENTS;
                    } else{
                        ungetc(c,stdin);
                        stav = OPERATORS;
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
                        return VAR_DEF;
                    }
                    return LEX_ERROR;
                }

                // operatory
                else if(c == '+') return PLUS;
                else if(c == '-') return MINUS;
                else if(c == '*') return MUL;
                else if(c == '<'){
                    c = getc(stdin);
                    if(c == '='){
                        return LESS_EQUAL;
                    }
                    ungetc(c,stdin);
                    return LESS;
                } else if(c == '>'){
                    c = getc(stdin);
                    if(c == '='){
                        return MORE_EQUAL;
                    }
                    ungetc(c,stdin);
                    return MORE;
                } else if(c == '='){
                    c = getc(stdin);
                    if(c == '='){
                        return EQUAL;
                    }
                    return VAR_INIT;
                } else if(c == '!'){
                    c = getc(stdin);
                    if(c == '='){
                        return NOT_EQUAL;
                    }
                    return LEX_ERROR;
                }
                
                else if(c == ',') return COMMA;
                else if(c == '(') return ROUND_LBRACKET;
                else if(c == ')') return ROUND_RBRACKET;
                else if(c == '{') return CURLY_LBRACKET;
                else if(c == '}') return CURLY_RBRACKET;

                else return LEX_ERROR;
                
                break;

            // riadkove a blokove komentare
            case COMMENTS:

                if(c == '/'){
                    // Jednoriadkovy komentar
                    while((c = getc(stdin)) != '\n');
                    stav = START;
                } else if (c == '*'){
                    // Blokovy komentar
                    while(1){        
                        if((c = getc(stdin)) == '*' && (c = getc(stdin)) == '/'){ break; }

                        if(feof(stdin)){ error_exit(LEX_ERROR,"Lexikalna chyba!"); }
                    }
                }
                break;

            case IDENTIF:
                break;

            case NUMBER:
                break;
            
            case STRING:
                break;

            case OPERATORS:
                break;

            default:
                return LEX_ERROR;
        }
    }
    
    return SCANNER_OK;
}