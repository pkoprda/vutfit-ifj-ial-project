#include "libmine.h"


int lexer(){
    char c;
    while(!feof(stdin)){
        c = getc(stdin);
        switch(c){

            // Identifikátory
            case 'a' ... 'z':
            case 'A' ... 'Z':
            case '_':
                break;
             
            case ' ':
                break;

            // Komentáre
            case '/':
                c = getc(stdin);
                
                if(c == '/'){
                    // Jednoriadkovy komentar
                    while((c = getc(stdin)) != '\n');
                    break;
                } else if(c == '*'){
                    // Viacradkovy komentar
                    while(1){                     
                        if((c = getc(stdin)) == '*' && (c = getc(stdin)) == '/'){
                            break;
                        }
                        if(feof(stdin)){
                            error_exit(LEX_ERROR,"Lexikalna chyba!");
                        }
                    }
                    break;
                    
                } else{
                    error_exit(LEX_ERROR,"Lexikalna chyba!");
                }

            default:
                break;
        }
    }
    
    return SCANNER_OK;
}