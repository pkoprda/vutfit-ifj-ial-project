#include "libmine.h"

#define PRINT_CODE(M, ...)               \
    fprintf(stdout, "" M "", ##__VA_ARGS__) 

#define PRINT_NL() \
    fprintf(stdout, "\n")

void generate()
{
    stdout_print("\n======================= GENERATING CODE =======================\n");
    //PRINT_HEADER();
    PRINT_CODE(".IFJcode20\n");
    PRINT_NL();
    generate_label(ast->Rptr);
}

void generate_label(Tree *ast)
{
    if (ast->Rptr->type == N_DEF_FUNC)
    {
        if (strcmp(ast->Rptr->value, "main") == 0)
        {
            PRINT_CODE("LABEL $$%s\n", ast->Rptr->value);
            PRINT_CODE("CREATEFRAME\n");
            PRINT_CODE("PUSHFRAME\n");
            if (ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
            PRINT_CODE("POPFRAME\n");
            PRINT_CODE("EXIT int@0\n");
            PRINT_NL();
        }
        else
        {
            PRINT_CODE("\nLABEL $%s\n", ast->Rptr->value);
            PRINT_CODE("CREATEFRAME\n");
            PRINT_CODE("PUSHFRAME\n");
            if (ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
            PRINT_CODE("POPFRAME\n");
            PRINT_NL();
        }
    }
    if (!ast->Lptr)
    {
        return;
    }
    generate_label(ast->Lptr);
}

void generate_function(Tree *ast)
{
    switch (ast->Rptr->type)
    {
        case N_IDENT_DEF:
            generate_var_def(ast->Rptr);
            break;

        case N_IDENT_INIT:
            if (ast->Rptr->Lptr->type != SEQ)
            {
                generate_var_init(ast->Rptr->Lptr);
                generate_expr(ast->Rptr->Rptr);
            }
            else
            {
                generate_multivar_init(ast->Rptr->Lptr, ast->Rptr->Rptr);
            }

            break;

        case N_PRINT:
            generate_print(ast->Rptr->Lptr);
            break;

        default:
            break;
    }

    if (!ast->Lptr)
    {
        return;
    }
    generate_function(ast->Lptr);
}

void generate_var_def(Tree *ast)
{
    PRINT_CODE("DEFVAR LF@%%%s", ast->Lptr->value);
    PRINT_NL();
    switch (ast->Rptr->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT:
            PRINT_CODE("MOVE LF@%%%s ", ast->Lptr->value);
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            break;

        case N_IDENTIFIER:
            break;
    }
}

void generate_var_init(Tree *ast)
{
    PRINT_CODE("MOVE LF@%%%s ", ast->value);
}

void generate_multivar_init(Tree *vars, Tree *expr)
{
    PRINT_CODE("MOVE LF@%%%s ", vars->Rptr->value);
    generate_expr(expr);
    if(!vars->Lptr)
    {
        return;
    }
    generate_multivar_init(vars->Lptr, expr->Lptr);
}

void generate_expr(Tree *ast)
{
    switch (ast->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT:
            generate_constant(ast->type, ast->value);
            PRINT_NL();
            break;

        case N_PLUS:
        case N_MINUS:
        case N_MULL:
        case N_DIV:
            break;

        case SEQ:
            if(!ast->Rptr)
            {
                return;
            }
            generate_expr(ast->Rptr); 
            break;

        default:
            break;
    }

}

void generate_constant(int type, char* value)
{
    switch(type)
    {
        case N_LIT_INT:
            PRINT_CODE("int@%s", value);
            break;

        case N_LIT_STRING:
            PRINT_CODE("string@");
            convertString(value);
            break;

        case N_LIT_FLOAT:
            PRINT_CODE("float@%a", strtod(value, NULL));
            break;
        
        default:
            break;
    }
}

void generate_print(Tree *ast)
{
    switch (ast->Rptr->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT:
            
            PRINT_CODE("WRITE ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            break;

        default:
            break;
    }
    if (!ast->Lptr)
    {
        return;
    }
    generate_print(ast->Lptr);
}