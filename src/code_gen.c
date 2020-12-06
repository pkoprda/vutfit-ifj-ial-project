#include "code_gen.h"


void generate()
{
    stdout_print("\n======================= GENERATING CODE =======================\n");
    //PRINT_HEADER();
    PRINT_CODE(".IFJcode20\n");
    PRINT_CODE("JUMP $$main\n");
    PRINT_NL();
    generate_label(ast->Rptr);
}

void generate_label(Tree *ast)
{
    if (ast->Rptr->type == N_DEF_FUNC)
    {
        if (strcmp(ast->Rptr->value, "main") == 0)
        {
            PRINT_CODE("LABEL $$main\n");
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
            PRINT_CODE("PUSHFRAME\n");
            if (ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
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
                if(ast->Rptr->Rptr->type != N_PLUS && ast->Rptr->Rptr->type != N_MINUS &&
                   ast->Rptr->Rptr->type != N_DIV && ast->Rptr->Rptr->type != N_MULL){
                    generate_var_init(ast->Rptr->Lptr);
                    generate_expr(ast->Rptr->Rptr);
                }
                else{
                    generate_expr(ast->Rptr->Rptr);
                    PRINT_CODE("POPS LF@%%%s\n",ast->Rptr->Lptr->value);
                    PRINT_CODE("CLEARS\n");
                }
                
            }
            else
            {
                generate_multivar_init(ast->Rptr->Lptr, ast->Rptr->Rptr);
            }

            break;

        case N_PRINT:
            if(ast->Rptr->Lptr)
            {
                generate_print(ast->Rptr->Lptr);
            }
            break;

        case N_RETURN:
            if(ast->Rptr->Lptr)
            {
                generate_return(ast->Rptr->Lptr);
            }
            PRINT_CODE("POPFRAME\n");
            PRINT_CODE("RETURN\n");
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
        case N_IDENTIFIER:
            PRINT_CODE("MOVE LF@%%%s ", ast->Lptr->value);
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            break;

        case N_FUNC:
            PRINT_CODE("CREATEFRAME\n");
            PRINT_CODE("CALL $%s\n", ast->Rptr->value);
            PRINT_CODE("MOVE LF@%%%s TF@%%retval1\n", ast->Lptr->value);
            break;
    }
}

void generate_var_init(Tree *ast)
{
    PRINT_CODE("MOVE LF@%%%s ", ast->value);
}

void generate_multivar_init(Tree *vars, Tree *expr)
{
    //PRINT_CODE("MOVE LF@%%%s ", vars->Rptr->value);
    // debug_print("%d %d", expr->Lptr->type, expr->Rptr->type);
    // if(expr->Lptr && expr->Lptr->type == N_LIT_STRING && expr->Rptr->type == N_LIT_STRING)
    // {
    //     PRINT_CODE("CONCAT LF@%%%s ", vars->Rptr->value);
    //     generate_expr(expr);
    // }
    // else
    // {
    generate_expr(expr);
    PRINT_CODE("POPS LF@%%%s\n", vars->Rptr->value);
    // }

    if(!vars->Lptr)
    {
        return;
    }
    generate_multivar_init(vars->Lptr, expr->Lptr);

}

void generate_expr(Tree *ast)
{
    //debug_print("Type: %d | Value:%s", ast->type, ast->value);
    switch (ast->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT:
            generate_constant(ast->type, ast->value);
            PRINT_NL();
            break;

        case N_PLUS:
            if(ast->Lptr && ast->Lptr->type == N_LIT_STRING && ast->Rptr->type == N_LIT_STRING)
            {
                generate_constant(ast->Lptr->type, ast->Lptr->value);
                PRINT_CODE(" ");
                generate_constant(ast->Rptr->type, ast->Rptr->value);
                PRINT_NL();
                break;
            }
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            if(ast->Lptr->type != N_LIT_INT && ast->Lptr->type != N_LIT_STRING &&
               ast->Lptr->type != N_LIT_FLOAT && ast->Lptr->type != N_IDENTIFIER)
            {
                break;
            }
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Lptr->type, ast->Lptr->value);
            PRINT_NL();
            
            PRINT_CODE("ADDS\n");
            break;

        case N_MINUS:
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            if(ast->Lptr && ast->Lptr->type != N_LIT_INT && ast->Lptr->type != N_LIT_STRING &&
               ast->Lptr->type != N_LIT_FLOAT && ast->Lptr->type != N_IDENTIFIER)
            {
                break;
            }
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Lptr->type, ast->Lptr->value);
            PRINT_NL();
            PRINT_CODE("SUBS\n");
            break;

        case N_MULL:
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            if(ast->Lptr && ast->Lptr->type != N_LIT_INT && ast->Lptr->type != N_LIT_STRING &&
               ast->Lptr->type != N_LIT_FLOAT && ast->Lptr->type != N_IDENTIFIER)
            {
                break;
            }
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Lptr->type, ast->Lptr->value);
            PRINT_NL();
            PRINT_CODE("MULS\n");
            break;

        case N_DIV:
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            if(ast->Lptr && ast->Lptr->type != N_LIT_INT && ast->Lptr->type != N_LIT_STRING &&
               ast->Lptr->type != N_LIT_FLOAT && ast->Lptr->type != N_IDENTIFIER)
            {
                break;
            }
            PRINT_CODE("PUSHS ");
            generate_constant(ast->Lptr->type, ast->Lptr->value);
            PRINT_NL();
            
            if(ast->Lptr->type == N_LIT_INT && ast->Rptr->type == N_LIT_INT)
            {
                PRINT_CODE("IDIVS\n");
            }
            else if(ast->Lptr->type == N_LIT_FLOAT && ast->Rptr->type == N_LIT_FLOAT)
            {
                PRINT_CODE("DIVS\n");
            }
            else
            {
                PRINT_CODE("DIVS\n");
                // PRINT_CODE("TYPE LF@%%numerator LF@%%%s\n", ast->Lptr->value);
                // PRINT_CODE("TYPE LF@%%denominator LF@%%%s\n", ast->Rptr->value);
            }
            
            
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

    if(ast->Lptr && ast->Lptr->type != N_LIT_INT && ast->Lptr->type != N_LIT_STRING &&
       ast->Lptr->type != N_LIT_FLOAT && ast->Lptr->type != N_IDENTIFIER){
           generate_expr(ast->Lptr);
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

        case N_IDENTIFIER:
            PRINT_CODE("LF@%%%s", value);
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

        case N_IDENTIFIER:
            PRINT_CODE("WRITE ");
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();

        default:
            break;
    }
    if (!ast->Lptr)
    {
        return;
    }
    generate_print(ast->Lptr);
}

void generate_return(Tree *ast)
{
    static int retval = 1;
    switch(ast->Rptr->type)
    {
        case N_LIT_INT:
        case N_LIT_STRING:
        case N_LIT_FLOAT:
            PRINT_CODE("DEFVAR LF@%%retval%d\n", retval);
            PRINT_CODE("MOVE LF@%%retval%d ", retval);
            generate_constant(ast->Rptr->type, ast->Rptr->value);
            PRINT_NL();
            break;

        case N_IDENTIFIER:
            PRINT_CODE("MOVE LF@%%retval%d LF@%%%s\n", retval, ast->Rptr->value);
            break;

        default:
            break;
    }

    if (!ast->Lptr)
    {
        return;
    }
    retval++;
    generate_return(ast->Lptr);
}