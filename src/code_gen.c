#include "code_gen.h"

#define PRINT_HEADER()         \
    PRINT_CODE(".IFJcode20")    \
    PRINT_NL()



void generate_main_start()
{
    PRINT_CODE("LABEL $$main");
    PRINT_CODE("CREATEFRAME");
}

void generate_header()
{
    PRINT_HEADER();
}

void generate_main_end()
{
    PRINT_CODE("EXIT int@0");
}

void generate_print(int type, char* value)
{
    switch(type)
    {
        case TOKEN_INT:
            PRINT_CODE("WRITE int@%s", value);
            break;
        
        case TOKEN_FLOAT:
            PRINT_CODE("WRITE float@%s", value);
            break;

        case TOKEN_STRING:
            value++;
            value[strlen(value)-1] = '\0';
            
            // int newLen = 0;
            // for(int i=0; value[i] != '\0'; i++)
            // {
            //     if(value[i] == '\n' || value[i] == '\t' || value[i] == ' ' || value[i] == '#' || value[i] == '\\')
            //     {
            //         newLen += 3;
            //     }
            // }

            
            // char *result = (char*)malloc(sizeof(value) + newLen + 1);
            // while(*value)
            // {
            //     if(strchr(value, ' '))
            //     {
            //         strcpy(result, "\\032");
            //     }
            //     value++;
            // }
            // result[strlen(result)] = '\0';
           

            PRINT_CODE("WRITE string@%s", value);
            break;

        default:
            break;
    }
}