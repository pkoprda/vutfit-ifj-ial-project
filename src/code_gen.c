#include "code_gen.h"

int input_count = 1;
int substr_label_cnt = 1;
bool runtime_error = false;
bool infor = false;

char *function;
int if_s, for_s;

#define GEN_INT2FLOAT(var, symb, type)     \
    PRINT_CODE("INT2FLOAT LF@%%%s ", var); \
    generate_constant(type, symb);         \
    PRINT_NL()

#define GEN_FLOAT2INT(var, symb, type)     \
    PRINT_CODE("FLOAT2INT LF@%%%s ", var); \
    generate_constant(type, symb);         \
    PRINT_NL()

#define GEN_STRLEN(var, symb, type)     \
    PRINT_CODE("STRLEN LF@%%%s ", var); \
    generate_constant(type, symb);      \
    PRINT_NL();

#define GEN_INT2CHAR(var, err, symb, type)                                         \
    PRINT_CODE("MOVE LF@%%%s int@1\n", err);                                       \
    PRINT_CODE("GT GF@%%downlimit ");                                              \
    generate_constant(type, symb);                                                 \
    PRINT_CODE(" int@-1\n");                                                       \
    PRINT_CODE("LT GF@%%toplimit ");                                               \
    generate_constant(type, symb);                                                 \
    PRINT_CODE(" int@256\n");                                                      \
    PRINT_CODE("AND GF@%%inInterval GF@%%downlimit GF@%%toplimit\n");              \
    PRINT_CODE("JUMPIFEQ continue%%%d GF@%%inInterval bool@false\n", input_count); \
    PRINT_CODE("MOVE LF@%%%s int@0\n", err);                                       \
    PRINT_CODE("INT2CHAR LF@%%%s ", var);                                          \
    generate_constant(type, symb);                                                 \
    PRINT_NL();                                                                    \
    PRINT_CODE("LABEL continue%%%d\n", input_count);                               \
    input_count++;

#define GEN_STRI2INT(var, err, symb1, symb2, type1, type2)                         \
    PRINT_CODE("STRLEN GF@%%strlenMax ");                                          \
    generate_constant(type1, symb1);                                               \
    PRINT_CODE("\nMOVE LF@%%%s int@1\n", err);                                     \
    PRINT_CODE("GT GF@%%downlimit ");                                              \
    generate_constant(type2, symb2);                                               \
    PRINT_CODE(" int@-1\n");                                                       \
    PRINT_CODE("LT GF@%%toplimit ");                                               \
    generate_constant(type2, symb2);                                               \
    PRINT_CODE(" GF@%%strlenMax\n");                                               \
    PRINT_CODE("AND GF@%%inInterval GF@%%downlimit GF@%%toplimit\n");              \
    PRINT_CODE("JUMPIFEQ continue%%%d GF@%%inInterval bool@false\n", input_count); \
    PRINT_CODE("MOVE LF@%%%s int@0\n", err);                                       \
    PRINT_CODE("STRI2INT LF@%%%s ", var);                                          \
    generate_constant(type1, symb1);                                               \
    PRINT_CODE(" ");                                                               \
    generate_constant(type2, symb2);                                               \
    PRINT_NL();                                                                    \
    PRINT_CODE("LABEL continue%%%d\n", input_count);                               \
    input_count++;

#define GEN_SUBSTR(var, err, symb1, symb2, symb3, type1, type2, type3)              \
    PRINT_CODE("STRLEN GF@%%strlenMax ");                                           \
    generate_constant(type1, symb1);                                                \
    PRINT_CODE("\nMOVE LF@%%%s int@1\n", err);                                      \
    PRINT_CODE("GT GF@%%downlimit ");                                               \
    generate_constant(type2, symb2);                                                \
    PRINT_CODE(" int@-1\n");                                                        \
    PRINT_CODE("LT GF@%%toplimit ");                                                \
    generate_constant(type2, symb2);                                                \
    PRINT_CODE(" GF@%%strlenMax\n");                                                \
    PRINT_CODE("AND GF@%%inInterval GF@%%downlimit GF@%%toplimit\n");               \
    PRINT_CODE("GT GF@%%downlimit ");                                               \
    generate_constant(type3, symb3);                                                \
    PRINT_CODE(" int@-1\n");                                                        \
    PRINT_CODE("AND GF@%%inInterval GF@%%downlimit GF@%%inInterval\n");             \
    PRINT_CODE("JUMPIFEQ continue%%%d GF@%%inInterval bool@false\n", input_count);  \
    PRINT_CODE("MOVE LF@%%%s int@0\n", err);                                        \
    PRINT_CODE("MOVE LF@%%%s string@\n", var);                                      \
    PRINT_CODE("CREATEFRAME\n");                                                    \
    PRINT_CODE("DEFVAR TF@%%counter\n");                                            \
    PRINT_CODE("MOVE TF@%%counter int@0\n");                                        \
    PRINT_CODE("DEFVAR TF@%%index\n");                                              \
    PRINT_CODE("MOVE TF@%%index ");                                                 \
    generate_constant(type2, symb2);                                                \
    PRINT_CODE("\nLABEL substr%%%d\n", substr_label_cnt);                           \
    PRINT_CODE("JUMPIFEQ continue%%%d TF@%%counter ", input_count);                 \
    generate_constant(type3, symb3);                                                \
    PRINT_CODE("\nJUMPIFEQ continue%%%d TF@%%index GF@%%strlenMax\n", input_count); \
    PRINT_CODE("GETCHAR GF@%%substr ");                                             \
    generate_constant(type1, symb1);                                                \
    PRINT_CODE(" TF@%%index\n");                                                    \
    PRINT_CODE("CONCAT LF@%%%s LF@%%%s GF@%%substr\n", var, var);                   \
    PRINT_CODE("ADD TF@%%index TF@%%index int@1\n");                                \
    PRINT_CODE("ADD TF@%%counter TF@%%counter int@1\n");                            \
    PRINT_CODE("JUMP substr%%%d\n", substr_label_cnt);                              \
    PRINT_CODE("LABEL continue%%%d\n", input_count);                                \
    input_count++;                                                                  \
    substr_label_cnt++;

#define GEN_READ(value, err, type)                                              \
    PRINT_CODE("READ LF@%%%s %s\n", value, type);                               \
    PRINT_CODE("MOVE LF@%%%s int@0\n", err);                                    \
    PRINT_CODE("JUMPIFNEQ continue%%%d LF@%%%s nil@nil\n", input_count, value); \
    PRINT_CODE("MOVE LF@%%%s int@1\n", err);                                    \
    PRINT_CODE("LABEL continue%%%d\n", input_count);                            \
    input_count++;                                                              \
    runtime_error = true;

void generate()
{
    stdout_print("\n======================= GENERATING CODE =======================\n");
    //PRINT_HEADER();
    PRINT_CODE(".IFJcode20\n\n");

    PRINT_CODE("# Variables for built-in functions\n");
    PRINT_CODE("DEFVAR GF@%%strlenMax\n");
    PRINT_CODE("DEFVAR GF@%%downlimit\n");
    PRINT_CODE("DEFVAR GF@%%toplimit\n");
    PRINT_CODE("DEFVAR GF@%%inInterval\n\n");
    PRINT_CODE("DEFVAR GF@%%substr\n");

    PRINT_CODE("DEFVAR GF@!compvar\n");
    PRINT_CODE("DEFVAR GF@!condvar1\n");
    PRINT_CODE("DEFVAR GF@!condvar2\n");

    PRINT_CODE("JUMP $$main\n");
    PRINT_NL();

    generate_label(ast->Rptr);

    if (runtime_error)
    {
        PRINT_CODE("LABEL runtimeError\n");
        PRINT_CODE("WRITE string@Division\\032by\\032zero!\\010\n");
        PRINT_CODE("EXIT int@9\n");
    }
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
                function = ast->Rptr->value;
                if_s = 0;
                for_s = 0;
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

            if (ast->Rptr->Lptr)
            {
                if (ast->Rptr->Lptr->Lptr)
                {
                    function = ast->Rptr->value;
                    if_s = 0;
                    for_s = 0;
                    generate_return_types(ast->Rptr->Lptr->Lptr);
                }
            }
            if (ast->Rptr->Rptr)
            {
                generate_function(ast->Rptr->Rptr);
            }
            PRINT_CODE("POPFRAME\n");
            PRINT_CODE("RETURN\n");
            PRINT_NL();
        }
    }
    if (!ast->Lptr)
    {
        return;
    }
    generate_label(ast->Lptr);
}

void define_for_variables(Tree *ast)
{
    if (ast->type == N_IDENT_DEF)
    {
        PRINT_CODE("DEFVAR ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_NL();
        return;
    }

    if (ast->Lptr != NULL)
    {
        define_for_variables(ast->Lptr);
    }
    if (ast->Rptr != NULL)
    {
        define_for_variables(ast->Rptr);
    }
}

void generate_function(Tree *ast)
{
    int ifc, forc;
    switch (ast->Rptr->type)
    {

    case N_IDENT_DEF:
        if (infor == true)
        {
            ast->Rptr->type = N_IDENT_INIT;
        }
        else
        {
            generate_var_def(ast->Rptr);
            break;
        }

    case N_IDENT_INIT:
        if (ast->Rptr->Lptr->type != SEQ)
        {
            if (ast->Rptr->Rptr->type == N_INT2FLOAT)
            {
                if (isdigit(ast->Rptr->Rptr->value[0]))
                {
                    GEN_INT2FLOAT(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_LIT_INT);
                }
                else
                {
                    GEN_INT2FLOAT(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_IDENTIFIER);
                }
            }
            else if (ast->Rptr->Rptr->type == N_FLOAT2INT)
            {
                if (isdigit(ast->Rptr->Rptr->value[0]))
                {
                    GEN_FLOAT2INT(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_LIT_FLOAT);
                }
                else
                {
                    GEN_FLOAT2INT(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_IDENTIFIER);
                }
            }
            else if (ast->Rptr->Rptr->type == N_LEN)
            {
                if (isdigit(ast->Rptr->Rptr->value[0]))
                {
                    GEN_STRLEN(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_LIT_STRING);
                }
                else
                {
                    GEN_STRLEN(ast->Rptr->Lptr->value, ast->Rptr->Rptr->value, N_IDENTIFIER);
                }
            }
            else if (ast->Rptr->Rptr->type == N_FUNC)
            {
                gen_initvar_call(ast->Rptr->Rptr, ast->Rptr->Lptr->value);
            }
            else if (ast->Rptr->Rptr->type != N_PLUS && ast->Rptr->Rptr->type != N_MINUS &&
                     ast->Rptr->Rptr->type != N_DIV && ast->Rptr->Rptr->type != N_MULL)
            {
                PRINT_CODE("MOVE LF@%%%s ", ast->Rptr->Lptr->value);
                gen_expr(ast->Rptr->Rptr);
            }
            else
            {
                gen_expr(ast->Rptr->Rptr);
                PRINT_CODE("POPS LF@%%%s\n", ast->Rptr->Lptr->value);
                PRINT_CODE("CLEARS\n");
            }
        }
        else
        {
            generate_multivar_init(ast->Rptr->Lptr, ast->Rptr->Rptr);
        }

        break;

    case N_PRINT:
        if (ast->Rptr->Lptr)
        {
            generate_print(ast->Rptr->Lptr);
        }
        break;

    case N_FUNC:
        PRINT_CODE("CREATEFRAME\n");
        if (ast->Rptr->Lptr)
        {
            generate_call(ast->Rptr->Lptr);
        }
        PRINT_CODE("CALL $%s\n", ast->Rptr->value);
        break;

    case N_FOR:
        /* DEFVAR KAZDU PREMENNU */
        forc = for_s;
        for_s++;
        define_for_variables(ast->Rptr->Rptr);
        if (ast->Rptr->Lptr->Rptr)
        {
            PRINT_CODE("DEFVAR ");
            //for ; podm; ... {}
            generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type, ast->Rptr->Lptr->Rptr->Lptr->value);
            PRINT_NL();

            if (ast->Rptr->Lptr->Rptr->Rptr->type != N_PLUS && ast->Rptr->Lptr->Rptr->Rptr->type != N_MINUS &&
                ast->Rptr->Lptr->Rptr->Rptr->type != N_DIV && ast->Rptr->Lptr->Rptr->Rptr->type != N_MULL &&
                ast->Rptr->Lptr->Rptr->Rptr->type != N_FUNC)
            {
                // a = 6 MOVE LF@%a int@6
                PRINT_CODE("MOVE ");
                generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type, ast->Rptr->Lptr->Rptr->Lptr->value);
                PRINT_CODE(" ");
                gen_expr(ast->Rptr->Lptr->Rptr->Rptr);
                PRINT_NL();
            }
            else
            {
                // a= 6+5-7 POPS LF@%a
                gen_expr(ast->Rptr->Lptr->Rptr->Rptr);
                PRINT_CODE("POPS ");
                generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type, ast->Rptr->Lptr->Rptr->Lptr->value);
                PRINT_NL();
            }
        }
        PRINT_CODE("LABEL %d%strue\n", forc, function);
        generate_condition(ast->Rptr->Lptr->Lptr->Rptr, forc, 1);
        infor = true;
        generate_function(ast->Rptr->Rptr);
        infor = false;
        /* a = a-1*/
        if (ast->Rptr->Lptr->Lptr->Lptr->Rptr)
        {
            gen_expr(ast->Rptr->Lptr->Lptr->Lptr->Rptr->Rptr);
            PRINT_CODE("POPS ");
            generate_constant(ast->Rptr->Lptr->Lptr->Lptr->Rptr->Lptr->type, ast->Rptr->Lptr->Lptr->Lptr->Rptr->Lptr->value);
            PRINT_NL();
        }
        PRINT_CODE("JUMP %d%strue\n", forc, function);
        PRINT_CODE("LABEL %d%sfalse\n", forc, function);

        break;

    case N_IF:
        /* Vyries condition, ktora je sirsi expression */
        ifc = if_s;
        if_s++;

        generate_condition(ast->Rptr->Lptr->Rptr, ifc, 0);
        if (ast->Rptr->Rptr->Rptr)
        {
            generate_function(ast->Rptr->Rptr->Rptr);
        }
        PRINT_CODE("JUMP %s%dtrue\n", function, ifc);
        PRINT_CODE("LABEL %s%dfalse", function, ifc);
        PRINT_NL();
        if (ast->Rptr->Rptr->Lptr)
        {
            generate_function(ast->Rptr->Rptr->Lptr);
        }
        PRINT_CODE("LABEL %s%dtrue", function, ifc);
        PRINT_NL();
        break;

    case N_RETURN:
        if (ast->Rptr->Lptr)
        {
            generate_return(ast->Rptr->Lptr);
        }
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

// type: 0 - IF, 1 - FOR
void generate_condition(Tree *ast, int count, int type)
{

    if (ast->Lptr->type != N_PLUS && ast->Lptr->type != N_MINUS &&
        ast->Lptr->type != N_DIV && ast->Lptr->type != N_MULL &&
        ast->Lptr->type != N_FUNC)
    {
        PRINT_CODE("MOVE GF@!condvar1 ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_NL();
    }
    else
    {
        gen_expr(ast->Lptr);
        PRINT_CODE("POPS GF@!condvar1\n");
    }

    if (ast->Rptr->type != N_PLUS && ast->Rptr->type != N_MINUS &&
        ast->Rptr->type != N_DIV && ast->Rptr->type != N_MULL &&
        ast->Rptr->type != N_FUNC)
    {
        PRINT_CODE("MOVE GF@!condvar2 ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
    }
    else
    {
        gen_expr(ast->Rptr);
        PRINT_CODE("POPS GF@!condvar2\n");
    }

    switch (ast->type)
    {
    case N_GREATER:
        PRINT_CODE("GT GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            //b:=0 b>2
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }
        break;
    case N_LESS:
        PRINT_CODE("LT GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            // b:= 3  b < 1
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }
        break;

    case N_EQUAL:
        PRINT_CODE("EQ GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }
        break;
    case N_NOT_EQUAL:
        PRINT_CODE("EQ GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }
        break;
    case N_EQ_GREATER:
        PRINT_CODE("GT GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFEQ %s%dfalsee GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFEQ %d%sfalsee GF@!compvar bool@true\n", count, function);
        }
        PRINT_CODE("EQ GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }

        if (type == 0)
        {
            PRINT_CODE("LABEL %s%dfalsee\n", function, count);
        }
        else
        {
            PRINT_CODE("LABEL %d%sfalsee\n", count, function);
        }

        break;

    case N_EQ_LESS:
        PRINT_CODE("LT GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFEQ %s%dfalsee GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFEQ %d%sfalsee GF@!compvar bool@true\n", count, function);
        }

        PRINT_CODE("EQ GF@!compvar GF@!condvar1 GF@!condvar2\n");
        if (type == 0)
        {
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar bool@true\n", function, count);
        }
        else
        {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar bool@true\n", count, function);
        }

        if (type == 0)
        {
            PRINT_CODE("LABEL %s%dfalsee\n", function, count);
        }
        else
        {
            PRINT_CODE("LABEL %d%sfalsee\n", count, function);
        }
        break;
    default:
        break;
    }
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
        if (ast->Rptr->Lptr)
        {
            generate_call(ast->Rptr->Lptr);
        }
        PRINT_CODE("CALL $%s\n", ast->Rptr->value);
        PRINT_CODE("MOVE LF@%%%s TF@%%retval1\n", ast->Lptr->value);
        break;

    case N_PLUS:
    case N_MINUS:
    case N_MULL:
    case N_DIV:
        gen_expr(ast->Rptr);
        PRINT_CODE("POPS LF@%%%s\n", ast->Lptr->value);
        PRINT_CODE("CLEARS\n");
        break;

    case N_INT2FLOAT:
        if (isdigit(ast->Rptr->value[0]))
        {
            GEN_INT2FLOAT(ast->Lptr->value, ast->Rptr->value, N_LIT_INT);
        }
        else
        {
            GEN_INT2FLOAT(ast->Lptr->value, ast->Rptr->value, N_IDENTIFIER);
        }
        //GEN_INT2FLOAT(ast->Lptr->value, ast->Rptr->value);
        break;

    case N_FLOAT2INT:
        if (isdigit(ast->Rptr->value[0]))
        {
            GEN_FLOAT2INT(ast->Lptr->value, ast->Rptr->value, N_LIT_FLOAT);
        }
        else
        {
            GEN_FLOAT2INT(ast->Lptr->value, ast->Rptr->value, N_IDENTIFIER);
        }
        break;

    case N_LEN:
        if (isdigit(ast->Rptr->value[0]))
        {
            GEN_STRLEN(ast->Lptr->value, ast->Rptr->value, N_LIT_STRING);
        }
        else
        {
            GEN_STRLEN(ast->Lptr->value, ast->Rptr->value, N_IDENTIFIER);
        }
        break;
    }
}

void generate_multivar_init(Tree *vars, Tree *expr)
{
    int type1, type2, type3;
    switch (expr->Rptr->type)
    {
    case N_PLUS:
    case N_MINUS:
    case N_MULL:
    case N_DIV:
        gen_expr(expr->Rptr);
        PRINT_CODE("POPS LF@%%%s\n", vars->Rptr->value);
        PRINT_CODE("CLEARS\n");
        break;

    case N_INPUTI:
        GEN_READ(vars->Rptr->value, vars->Lptr->Rptr->value, "int");
        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    case N_INPUTS:
        GEN_READ(vars->Rptr->value, vars->Lptr->Rptr->value, "string");
        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    case N_INPUTF:
        GEN_READ(vars->Rptr->value, vars->Lptr->Rptr->value, "float");
        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    case N_SUBSTR:
        if (expr->Rptr->Lptr->Rptr->value[0] == '"')
            type1 = N_LIT_STRING;
        else
            type1 = N_IDENTIFIER;
        if (isdigit(expr->Rptr->Lptr->Lptr->Rptr->value[0]))
            type2 = N_LIT_INT;
        else
            type2 = N_IDENTIFIER;
        if (isdigit(expr->Rptr->Lptr->Lptr->Lptr->Rptr->value[0]))
            type3 = N_LIT_INT;
        else
            type3 = N_IDENTIFIER;

        GEN_SUBSTR(vars->Rptr->value, vars->Lptr->Rptr->value, expr->Rptr->Lptr->Rptr->value,
                   expr->Rptr->Lptr->Lptr->Rptr->value, expr->Rptr->Lptr->Lptr->Lptr->Rptr->value,
                   type1, type2, type3);

        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    case N_ORD:
        if (expr->Rptr->Lptr->Rptr->value[0] == '"')
            type1 = N_LIT_STRING;
        else
            type1 = N_IDENTIFIER;
        if (isdigit(expr->Rptr->Lptr->Lptr->Rptr->value[0]))
            type2 = N_LIT_INT;
        else
            type2 = N_IDENTIFIER;

        GEN_STRI2INT(vars->Rptr->value, vars->Lptr->Rptr->value, expr->Rptr->Lptr->Rptr->value,
                     expr->Rptr->Lptr->Lptr->Rptr->value, type1, type2);

        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    case N_CHR:
        if (isdigit(expr->Rptr->value[0]))
        {
            GEN_INT2CHAR(vars->Rptr->value, vars->Lptr->Rptr->value, expr->Rptr->value, N_LIT_INT);
        }
        else
        {
            GEN_INT2CHAR(vars->Rptr->value, vars->Lptr->Rptr->value, expr->Rptr->value, N_IDENTIFIER);
        }

        if (!vars->Lptr->Lptr)
            return;
        generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
        break;

    default:
        PRINT_CODE("MOVE LF@%%%s ", vars->Rptr->value);
        gen_expr(expr->Rptr);
        break;
    }

    if (!vars->Lptr)
    {
        return;
    }
    generate_multivar_init(vars->Lptr, expr->Lptr);
}

void gen_expr(Tree *ast)
{
    switch (ast->type)
    {
    case N_LIT_INT:
    case N_LIT_STRING:
    case N_LIT_FLOAT:
    case N_IDENTIFIER:
        generate_constant(ast->type, ast->value);
        PRINT_NL();
        break;
    case N_PLUS:
    case N_MINUS:
    case N_MULL:
    case N_DIV:
        calculate_expr(ast);
        break;
    }
}

void gen_initvar_call(Tree *ast, char *identif)
{
    PRINT_CODE("CREATEFRAME\n");
    if (ast->Lptr)
    {
        generate_call(ast->Lptr);
    }
    PRINT_CODE("CALL $%s\n", ast->value);
    PRINT_CODE("MOVE LF@%%%s TF@%%retval1\n", identif);
}

int getIDtype2(char *function, char *ID, int forcnt, int ifcnt, int hide)
{
    FunTItem *tmp = ftSearch(ft, function);
    SymTItem *sItem = stSearch(tmp->sym, ID);
    sItem = searchdown(sItem, hide, forcnt, ifcnt);
    return sItem->type;
}

SymTItem *getvalue(Tree *ast)
{
    FunTItem *fItem = ftSearch(ft, function);
    SymTItem *sItem = stSearch(fItem->sym, ast->value);
    return sItem;
}

void calculate_expr(Tree *ast)
{
    int type;

    switch (ast->type)
    {
    case N_PLUS:
        calculate_expr(ast->Lptr);
        calculate_expr(ast->Rptr);
        PRINT_CODE("ADDS\n");
        break;
    case N_MINUS:
        calculate_expr(ast->Lptr);
        calculate_expr(ast->Rptr);
        PRINT_CODE("SUBS\n");
        break;
    case N_MULL:
        calculate_expr(ast->Lptr);
        calculate_expr(ast->Rptr);
        PRINT_CODE("MULS\n");
        break;
    case N_DIV:

        //if(ast->Lptr != NULL){
        calculate_expr(ast->Lptr);
        calculate_expr(ast->Rptr);

        if (ast->Lptr->type == N_IDENTIFIER)
        {
            if (getvalue(ast->Lptr)->type == 1)
            {
                type = N_LIT_INT;
            }
            else
            {
                type = N_LIT_FLOAT;
            }
        }
        else
        {
            type = ast->Lptr->type;
        }
        PRINT_CODE("JUMPIFEQ runtimeError ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        if (type == N_LIT_INT)
        {
            PRINT_CODE(" int@0\n");
        }
        else
        {
            PRINT_CODE(" float@0x0p+0\n");
        }

        if (type == N_LIT_FLOAT)
        {
            PRINT_CODE("DIVS\n");
        }
        else if (type == N_LIT_INT)
        {
            PRINT_CODE("IDIVS\n");
        }

        break;
    case N_LIT_INT:
    case N_LIT_FLOAT:
    case N_IDENTIFIER:
        PRINT_CODE("PUSHS ");
        generate_constant(ast->type, ast->value);
        PRINT_NL();
        return;
    }
}

void generate_constant(int type, char *value)
{
    switch (type)
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

void generate_call(Tree *ast)
{
    static int parval = 1;
    switch (ast->Rptr->type)
    {
    case N_LIT_INT:
    case N_LIT_STRING:
    case N_LIT_FLOAT:
        PRINT_CODE("DEFVAR TF@%%%d\n", parval);
        PRINT_CODE("MOVE TF@%%%d ", parval);
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
        break;

    case N_IDENTIFIER:
        PRINT_CODE("DEFVAR TF@%%%d\n", parval);
        PRINT_CODE("MOVE TF@%%%d LF@%%%s\n", parval, ast->Rptr->value);
        break;

    default:
        break;
    }

    if (!ast->Lptr)
    {
        return;
    }
    parval++;
    generate_call(ast->Lptr);
}

void generate_return(Tree *ast)
{
    static int retval = 1;
    switch (ast->Rptr->type)
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
        PRINT_CODE("DEFVAR LF@%%retval%d\n", retval);
        PRINT_CODE("MOVE LF@%%retval%d LF@%%%s\n", retval, ast->Rptr->value);
        break;

    case N_PLUS:
    case N_MINUS:
    case N_MULL:
    case N_DIV:
        PRINT_CODE("DEFVAR LF@%%retval%d\n", retval);
        gen_expr(ast->Rptr);
        PRINT_CODE("POPS LF@%%retval%d ", retval);
        PRINT_NL();

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

void generate_return_types(Tree *ast)
{
    static int param = 1;
    PRINT_CODE("DEFVAR LF@%%%s\n", ast->Rptr->value);
    PRINT_CODE("MOVE LF@%%%s LF@%%%d\n", ast->Rptr->value, param);
    if (!ast->Lptr)
    {
        return;
    }
    param++;
    generate_return_types(ast->Lptr);
}