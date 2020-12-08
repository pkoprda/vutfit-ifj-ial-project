#include "code_gen.h"


#define GEN_INT2FLOAT(var, symb)                \
    if(isdigit(symb[0])) type = N_LIT_INT;      \
    else type = N_IDENTIFIER;                   \
    PRINT_CODE("INT2FLOAT LF@%%%s ", var);      \
    generate_constant(type, symb);              \
    PRINT_NL()

#define GEN_FLOAT2INT(var, symb)                \
    if(isdigit(symb[0])) type = N_LIT_FLOAT;    \
    else type = N_IDENTIFIER;                   \
    PRINT_CODE("FLOAT2INT LF@%%%s ", var);      \
    generate_constant(type, symb);              \
    PRINT_NL()

#define GEN_STRLEN(var, symb)                   \
    if(symb[0] == '"') type = N_LIT_STRING;     \
    else type = N_IDENTIFIER;                   \
    PRINT_CODE("STRLEN LF@%%%s ", var);         \
    generate_constant(type, symb);              \
    PRINT_NL();

#define GEN_INT2CHAR(var, symb)                 \
    if(isdigit(symb[0])) type = N_LIT_INT;      \
    else type = N_IDENTIFIER;                   \
    PRINT_CODE("INT2CHAR LF@%%%s ", var);       \
    generate_constant(type, symb);              \
    PRINT_NL();

#define GEN_STRI2INT(var, symb1, symb2)         \
    if(symb1[0] == '"') type1 = N_LIT_STRING;   \
    else type1 = N_IDENTIFIER;                  \
    if(isdigit(symb2[0])) type2 = N_LIT_INT     \
    else type2 = N_IDENTIFIER;                  \
    PRINT_CODE("STRI2INT LF@%%%s ", var);       \
    generate_constant(type1, symb1);            \
    PRINT_CODE(" ");                            \
    generate_constant(type2, symb2);            \
    PRINT_NL();

char *function;
int if_s,for_s;

void generate()
{
    stdout_print("\n======================= GENERATING CODE =======================\n");
    //PRINT_HEADER();
    PRINT_CODE(".IFJcode20\n\n");
    
    PRINT_CODE("DEFVAR GF@!true\n");
    PRINT_CODE("MOVE GF@!true bool@true\n");
    PRINT_CODE("DEFVAR GF@!compvar\n");
    
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
            
            if(ast->Rptr->Lptr)
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

void generate_function(Tree *ast)
{
    int ifc,forc;
    switch (ast->Rptr->type)
    {
        case N_IDENT_DEF:
            generate_var_def(ast->Rptr);
            break;

        case N_IDENT_INIT:
            if (ast->Rptr->Lptr->type != SEQ)
            {
                if(ast->Rptr->Rptr->type != N_PLUS && ast->Rptr->Rptr->type != N_MINUS &&
                   ast->Rptr->Rptr->type != N_DIV && ast->Rptr->Rptr->type != N_MULL && 
                   ast->Rptr->Rptr->type != N_FUNC)
                {
                    PRINT_CODE("MOVE LF@%%%s ", ast->Rptr->Lptr->value);
                    gen_expr(ast->Rptr->Rptr);
                }
                else if(ast->Rptr->Rptr->type == N_FUNC)
                {
                    gen_initvar_call(ast->Rptr->Rptr, ast->Rptr->Lptr->value);
                }
                else
                {
                    gen_expr(ast->Rptr->Rptr);
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

        case N_FUNC:
            PRINT_CODE("CREATEFRAME\n");
            if(ast->Rptr->Lptr)
            {
                generate_call(ast->Rptr->Lptr);
            }
            PRINT_CODE("CALL $%s\n", ast->Rptr->value);
            break; 

        case N_FOR:
            /* DEFVAR KAZDU PREMENNU */
            forc = for_s;
            for_s++;
            if(ast->Rptr->Lptr->Rptr){
                PRINT_CODE("DEFVAR ");
                //for ; podm; ... {}
                generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type,ast->Rptr->Lptr->Rptr->Lptr->value);
                PRINT_CODE(" ");
                
                if(ast->Rptr->Rptr->type != N_PLUS && ast->Rptr->Rptr->type != N_MINUS &&
                   ast->Rptr->Rptr->type != N_DIV && ast->Rptr->Rptr->type != N_MULL && 
                   ast->Rptr->Rptr->type != N_FUNC)
                {
                    // a = 6 MOVE LF@%a int@6
                    PRINT_CODE("MOVE ");
                    generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type,ast->Rptr->Lptr->Rptr->Lptr->value);
                    PRINT_CODE(" ");
                    gen_expr(ast->Rptr->Lptr->Rptr->Rptr);
                    PRINT_NL();

                } else {
                    // a= 6+5-7 POPS LF@%a
                    gen_expr(ast->Rptr->Lptr->Rptr->Rptr);
                    PRINT_CODE("POPS ");
                    generate_constant(ast->Rptr->Lptr->Rptr->Lptr->type,ast->Rptr->Lptr->Rptr->Lptr->value);
                    PRINT_NL();
                }
            }
            PRINT_CODE("LABEL %d%strue\n", forc, function);
            generate_condition(ast->Rptr->Lptr->Lptr->Rptr, forc, 1);
            generate_function(ast->Rptr->Rptr);
            /* a = a-1*/
            if(ast->Rptr->Lptr->Lptr->Lptr->Rptr){
                gen_expr(ast->Rptr->Lptr->Lptr->Lptr->Rptr->Rptr);
                PRINT_CODE("POPS ");
                generate_constant(ast->Rptr->Lptr->Lptr->Lptr->Rptr->Lptr->type,ast->Rptr->Lptr->Lptr->Lptr->Rptr->Lptr->value);
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
           if(ast->Rptr->Rptr->Rptr)
           {
                generate_function(ast->Rptr->Rptr->Rptr);
           }
            PRINT_CODE("JUMP %s%dtrue\n", function, ifc);
            PRINT_CODE("LABEL %s%dfalse", function, ifc );
            PRINT_NL();
            if(ast->Rptr->Rptr->Lptr)
            {
                generate_function(ast->Rptr->Rptr->Lptr);
            }
            PRINT_CODE("LABEL %s%dtrue", function, ifc );
            PRINT_NL();
            break;

        case N_RETURN:
            if(ast->Rptr->Lptr)
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

// TODO: conditions <= >=
// type: 0 - IF, 1 - FOR
void generate_condition(Tree *ast, int count,  int type){
    switch (ast->type)
    {
    case N_GREATER:
        PRINT_CODE("GT GF@!compvar ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_CODE(" ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
        //!compvar a true budu globalne variables
        // JUMPIFNEQ labelfalse !compvar !true
        if (type == 0){
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar GF@!true\n", function, count);
        } else {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar GF@!true\n", count, function);
        }
        break;
    case N_LESS:
        PRINT_CODE("LT GF@!compvar ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_CODE(" ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
        //!compvar a true budu globalne variables
        if (type == 0){
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar GF@!true\n", function, count);
        } else {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar GF@!true\n", count, function);
        }
        break;

    case N_EQUAL:
        PRINT_CODE("EQ GF@!compvar ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_CODE(" ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
        //!compvar a true budu globalne variables
        if (type == 0){
            PRINT_CODE("JUMPIFNEQ %s%dfalse GF@!compvar GF@!true\n", function, count);
        } else {
            PRINT_CODE("JUMPIFNEQ %d%sfalse GF@!compvar GF@!true\n", count, function);
        }
        break;
    case N_NOT_EQUAL:
        PRINT_CODE("EQ GF@!compvar ");
        generate_constant(ast->Lptr->type, ast->Lptr->value);
        PRINT_CODE(" ");
        generate_constant(ast->Rptr->type, ast->Rptr->value);
        PRINT_NL();
        //!compvar a true budu globalne variables
        if (type == 0){
            PRINT_CODE("JUMPIFEQ %s%dfalse GF@!compvar GF@!true\n", function, count);
        } else {
            PRINT_CODE("JUMPIFEQ %d%sfalse GF@!compvar GF@!true\n", count, function);
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
    int type;
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
            if(ast->Rptr->Lptr)
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
            GEN_INT2FLOAT(ast->Lptr->value, ast->Rptr->value);
            break;

        case N_FLOAT2INT:
            GEN_FLOAT2INT(ast->Lptr->value, ast->Rptr->value);
            break;

        case N_LEN:           
            GEN_STRLEN(ast->Lptr->value, ast->Rptr->value);
            break;
    }
}

// TODO: Runtime errors u funkcii inputi(), inputs(), inputf()
// TODO: substr(), ord(), chr()
void generate_multivar_init(Tree *vars, Tree *expr)
{
    switch(expr->Rptr->type)
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
            PRINT_CODE("READ LF@%%%s int\n", vars->Rptr->value);
            if(!vars->Lptr->Lptr) return;
            generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
            break;

        case N_INPUTS:
            PRINT_CODE("READ LF@%%%s string\n", vars->Rptr->value);
            if(!vars->Lptr->Lptr) return;
            generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
            break;

        case N_INPUTF:
            PRINT_CODE("READ LF@%%%s float\n", vars->Rptr->value);
            if(!vars->Lptr->Lptr) return;
            generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
            break;

        case N_SUBSTR:
            if(!vars->Lptr->Lptr) return;
            generate_multivar_init(vars->Lptr->Lptr, expr->Lptr);
            break;

        case N_ORD:
            break;
        
        case N_CHR:
            break;
        
        default:
            PRINT_CODE("MOVE LF@%%%s ", vars->Rptr->value);
            gen_expr(expr->Rptr);
            break;
    }        

    if(!vars->Lptr)
    {
        return;
    }
    generate_multivar_init(vars->Lptr, expr->Lptr);

}

void gen_expr(Tree *ast){
    switch (ast->type){
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
    if(ast->Lptr)
    {
        generate_call(ast->Lptr);
    }
    PRINT_CODE("CALL $%s\n", ast->value);
    PRINT_CODE("MOVE LF@%%%s TF@%%retval1\n", identif);
}

int getIDtype2(char *function, char *ID, int forcnt, int ifcnt, int hide){
    FunTItem *tmp = ftSearch(ft, function);
    SymTItem *sItem = stSearch(tmp->sym, ID);
    sItem = searchdown(sItem, hide, forcnt, ifcnt);
    return sItem->type;
}

char *getvalue(Tree *ast){
    FunTItem *fItem = ftSearch(ft, function); 
    SymTItem *sItem = stSearch(fItem->sym, ast->value);
    return sItem->value;
}

// 6+4*2-1+8-4+2+6-8
void calculate_expr(Tree *ast){
    switch (ast->type){
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
            /* CHECK PRE INT A FLOAT */
            PRINT_CODE("DIVS\n");
            //}
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

void generate_call(Tree *ast)
{
    static int parval = 1;
    switch(ast->Rptr->type)
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

    if(!ast->Lptr)
    {
        return;
    }
    parval++;
    generate_call(ast->Lptr);
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
    if(!ast->Lptr)
    {
        return;
    }
    param++;
    generate_return_types(ast->Lptr);
}