#include "code_gen.h"

#define PRINT_HEADER()         \
    PRINT_CODE(".IFJcode20")    \
    PRINT_NL()



void generate()
{
    stdout_print("\n======================= GENERATING CODE =======================\n");
    PRINT_HEADER();
    generate_label(ast->Rptr);
}

void generate_label(Tree *ast)
{
    if(ast->Rptr->type == N_DEF_FUNC)
    {
        if(strcmp(ast->Rptr->value, "main") == 0)
        {
            PRINT_CODE("LABEL $$%s", ast->Rptr->value);
            PRINT_CODE("CREATEFRAME");
            PRINT_CODE("PUSHFRAME");
            if(ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
            PRINT_CODE("POPFRAME");
            PRINT_CODE("EXIT int@0");
            PRINT_NL();
        }
        else
        {
            PRINT_CODE("LABEL $%s", ast->Rptr->value);
            PRINT_CODE("CREATEFRAME");
            PRINT_CODE("PUSHFRAME");
            if(ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
            PRINT_CODE("POPFRAME");
            PRINT_NL();
        }
        
    }
    if(!ast->Lptr)
    {
        return;
    }
    generate_label(ast->Lptr);
}

void generate_function(Tree *ast)
{
    stdout_print("Type: %d\tValue: %s", ast->Rptr->type, ast->Rptr->value);
    switch(ast->Rptr->type)
    {
        case N_IDENT_DEF:
            generate_var_def(ast->Rptr);
            break;
        
        case N_IDENT_INIT:
            stdout_print("HERE Type: %d Value: %s", ast->Rptr->type, ast->Rptr->value);
            generate_var_init(ast->Rptr->Lptr);
            break;

        case N_PRINT:
            generate_print(ast->Rptr->Lptr);
            break;

        default:
            break;
    }

    if(!ast->Lptr)
    {
        return;
    }
    generate_function(ast->Lptr);
}

void generate_var_def(Tree *ast)
{
    PRINT_CODE("DEFVAR LF@%%%s", ast->Lptr->value);
    switch(ast->Rptr->type)
    {
        case N_LIT_INT:
            PRINT_CODE("MOVE LF@%%%s int@%s", ast->Lptr->value, ast->Rptr->value);
            break;
        case N_LIT_STRING:
            PRINT_CODE_WITHOUT_NL("MOVE LF@%%%s string@", ast->Lptr->value);
            convertString(ast->Rptr->value);
            PRINT_NL();
            break;
        case N_LIT_FLOAT:
            PRINT_CODE("MOVE LF@%%%s float@%a", ast->Lptr->value, strtod(ast->Rptr->value, NULL));
            break;
        case N_IDENTIFIER:
            break;
    }
}

void generate_var_init(Tree *ast)
{
    PRINT_CODE_WITHOUT_NL("MOVE LF@%%%s ", ast->Rptr->value);
    PRINT_NL();
}

void generate_print(Tree *ast)
{
    switch(ast->Rptr->type)
    {
        case N_LIT_INT:
            PRINT_CODE("WRITE int@%s", ast->Rptr->value);
            break;

        case N_LIT_STRING:
            PRINT_CODE_WITHOUT_NL("WRITE string@");
            convertString(ast->Rptr->value);
            PRINT_NL();
            break;

        case N_LIT_FLOAT:
            PRINT_CODE("WRITE float@%a", strtod(ast->Rptr->value, NULL));
            break;


        default:
            break;
    }
    if(!ast->Lptr)
    {
        return;
    }
    generate_print(ast->Lptr);
}