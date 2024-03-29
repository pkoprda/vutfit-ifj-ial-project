/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Rozhranie pre cely prekladac
 * Autor: Daniel Paul - xpauld00,
 */

#include "libmine.h"

char *displayNode[] = {"SEQ", "PACKAGE_MAIN", "PARAMS_RETURNTYPES", "DEF_FUNC", "IDENTIFIER",
                       "PARAM_IDENT_INT", "PARAM_IDENT_STR", "PARAM_IDENT_FLOAT", "LIT_INT", "LIT_STRING", "LIT_FLOAT",
                       "RETURN_TYPE_INT", "RETURN_TYPE_STR", "RETURN_TYPE_FLOAT", "PRINT", "INPUTI", "INPUTS", "INPUTF", "DEF_VAR", "INIT_VAR", "FUNC",
                       "PLUS", "MINUS", "MULL", "DIV", "GREATER", "LESS", "EQ_GREATER", "EQ_LESS", "EQUAL", "NOT_EQUAL",
                       "IF", "ELSE", "RETURN", "LEN", "SUBSTR", "ORD", "CHR", "FOR", "INT2FLOAT", "FLOAT2INT"};

/* Precedenčná tabuľka */
attrTable table[] = {
    {TOKEN_IDENTIF, -1, N_IDENTIFIER},
    {IDENTIF_DEF, -1, N_IDENT_DEF},
    {IDENTIF_INIT, -1, N_IDENT_INIT},
    {TOKEN_INT, -1, N_LIT_INT},
    {TOKEN_FLOAT, -1, N_LIT_FLOAT},
    {TOKEN_STRING, -1, N_LIT_STRING},
    {TOKEN_PLUS, 12, N_PLUS},
    {TOKEN_MINUS, 12, N_MINUS},
    {TOKEN_MUL, 13, N_MULL},
    {TOKEN_DIV, 13, N_DIV},
    {TOKEN_LESS, 10, N_LESS},
    {TOKEN_LESS_EQUAL, 10, N_EQ_LESS},
    {TOKEN_GREATER, 10, N_GREATER},
    {TOKEN_GREATER_EQUAL, 10, N_EQ_GREATER},
    {TOKEN_EQUAL, 9, N_EQUAL},
    {TOKEN_NOT_EQUAL, 9, N_NOT_EQUAL},
    {TOKEN_EOL, -1, -1},
    {TOKEN_COMMA, -1, -1},
    {TOKEN_SEMICOLON, -1, -1},
    {TOKEN_ROUND_LBRACKET, -1, -1},
    {TOKEN_ROUND_RBRACKET, -1, -1},
    {TOKEN_CURLY_LBRACKET, -1, -1},
    {TOKEN_CURLY_RBRACKET, -1, -1},
    {KEYWORD_PACKAGE, -1, -1},
    {KEYWORD_IF, -1, N_IF},
    {KEYWORD_ELSE, -1, N_ELSE},
    {KEYWORD_FOR, -1, N_FOR},
    {KEYWORD_FUNC, -1, N_DEF_FUNC},
    {KEYWORD_INT, -1, RETURN_TYPE_INT},
    {KEYWORD_FLOAT64, -1, RETURN_TYPE_FLOAT},
    {KEYWORD_STRING, -1, RETURN_TYPE_STR},
    {KEYWORD_RETURN, -1, N_RETURN},
    {FUNC_MAIN, -1, -1},
    {FUNC_INPUTS, -1, N_INPUTS},
    {FUNC_INPUTI, -1, N_INPUTI},
    {FUNC_INPUTF, -1, N_INPUTF},
    {FUNC_PRINT, -1, N_PRINT},
    {FUNC_INT2FLOAT, -1, N_INT2FLOAT},
    {FUNC_FLOAT2INT, -1, N_FLOAT2INT},
    {FUNC_LEN, -1, N_LEN},
    {FUNC_SUBSTR, -1, N_SUBSTR},
    {FUNC_ORD, -1, N_ORD},
    {FUNC_CHR, -1, N_CHR},
    {43, -1, N_FUNC},
};

TokenPtr tok;
Tree *ast;
bool func_main_defined = false;

/* Začiatočná funkcia syntaktickej analýzy, premenná "ast" obsahuje vytvorený abstraktný syntaktický strom. */
int parser()
{
    ast = parse();
    if (func_main_defined != true)
    {
        error_exit(SEM_ERROR_UNDEF, "Semantic error! Function main not defined.");
    }
    return 0;
}

/* Začína tu metóda rekurzívneho zostupu */
Tree *parse()
{
    Tree *root = NULL;
    checkForExcessEOL();
    do
    {
        root = createNode(SEQ, root, prolog());
    } while (!stackEmpty(&stack));
    return root;
}

/* Pomocná funkcia, ktorá kontroluje EOL tokeny, ktoré sú navyše */
void checkForExcessEOL()
{
    if (!stackEmpty(&stack))
    {
        while (getToken(&stack)->type == TOKEN_EOL)
        {
            if (stackEmpty(&stack))
            {
                return;
            }
        }
        ungetToken(&stack);
    }
}

/* Funkcia, reprezentujúca neterminál "prolog" */
Tree *prolog()
{
    Tree *root = NULL;
    checkForExcessEOL();
    tok = getToken(&stack);
    if (tok == NULL)
    {
        error_exit(SYNTAX_ERROR, "Syntax error! Package missing.");
    }
    switch (tok->type)
    {
    case KEYWORD_PACKAGE:
        expectToken(FUNC_MAIN);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, program(), createLeaf(PACKAGE_MAIN, NULL));
        break;
    default:
        error_exit(SYNTAX_ERROR, "Syntax error! Package missing.");
        break;
    }

    return root;
}

/* Funkcia, reprezentujúca neterminál "program" */
Tree *program()
{
    checkForExcessEOL();
    Tree *root = NULL;
    TokenPtr help = NULL;
    Tree *func = NULL;
    Tree *parameters = NULL;
    Tree *returntypes = NULL;
    Tree *params_returntypes = NULL;
    char *func_name = NULL;
    tok = getToken(&stack);
    if (tok == NULL)
        return NULL;

    /* Pravidlo pre vytvorenie funkcie */
    if (tok->type == KEYWORD_FUNC)
    {
        tok = getToken(&stack);
        help = tok;

        if (tok->type == TOKEN_IDENTIF || tok->type == FUNC_MAIN)
        {
            if (tok->type == FUNC_MAIN)
            {
                func_main_defined = true;
            }
            func_name = my_strdup(tok->value);
        }
        else
        {
            error_exit(SYNTAX_ERROR, "Syntax error!");
        }

        expectToken(TOKEN_ROUND_LBRACKET);
        parameters = params();

        if (help->type == FUNC_MAIN && parameters != NULL)
        {
            error_exit(SEM_ERROR_PARAMS, "Function main has to have no parameters");
        }

        expectToken(TOKEN_ROUND_RBRACKET);
        returntypes = rt();
        if (help->type == FUNC_MAIN && returntypes != NULL)
        {
            error_exit(SEM_ERROR_PARAMS, "Function main has to have no return types");
        }

        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        if (parameters != NULL || returntypes != NULL)
            params_returntypes = createNode(N_PARAMS_RETURNTYPES, parameters, returntypes);

        func = createNode(N_DEF_FUNC, params_returntypes, stmt());
        func->value = func_name;
        expectToken(TOKEN_CURLY_RBRACKET);
        optionalEOL();
        root = createNode(SEQ, program(), func);
    }
    return root;
}

/* Táto funkcia rieši volitelný token EOL */
void optionalEOL()
{
    if (!stackEmpty(&stack))
    {
        tok = getToken(&stack);
        if (tok->type != TOKEN_EOL)
        {
            ungetToken(&stack);
        }
    }
}

/* Táto funkcia rieši gramatiku návratových typov funkcií - neterminál returntype  */
Tree *rt()
{
    Tree *returntype = NULL;
    Tree *returntypeNode = NULL;
    tok = getToken(&stack);
    if (tok->type == TOKEN_ROUND_LBRACKET)
    {
        returntype = getRT();
        if (returntype != NULL)
        {
            returntypeNode = createNode(SEQ, rt2(), returntype);
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        return returntypeNode;
    }
    else
    {
        ungetToken(&stack);
        return NULL;
    }
}

/* Ak sa nachádza návratových typov viac - neterminál returntype2 */
Tree *rt2()
{
    Tree *returntype = NULL;
    tok = getToken(&stack);
    if (tok->type == TOKEN_COMMA)
    {
        returntype = getRT();
        return createNode(SEQ, rt2(), returntype);
    }
    else
    {
        ungetToken(&stack);
        return NULL;
    }
}

/* Funkcia na zistenie typu návratovej hodnoty - neterminál type */
Tree *getRT()
{
    tok = getToken(&stack);
    switch (tok->type)
    {
    case KEYWORD_INT:
        return createLeaf(RETURN_TYPE_INT, NULL);
        break;

    case KEYWORD_FLOAT64:
        return createLeaf(RETURN_TYPE_FLOAT, NULL);
        break;
    case KEYWORD_STRING:
        return createLeaf(RETURN_TYPE_STR, NULL);
        break;
    case TOKEN_ROUND_RBRACKET:
        ungetToken(&stack);
        return NULL;
    default:
        error_exit(SYNTAX_ERROR, "Syntax error!");
    }
}

/* Spracovanie parametrov funkcie - neterminál params */
Tree *params()
{
    Tree *param = NULL;
    tok = getToken(&stack);
    if (tok->type == TOKEN_IDENTIF)
    {
        char *val = tok->value;
        tok = getToken(&stack);
        switch (tok->type)
        {
        case KEYWORD_INT:
            param = createLeaf(N_PARAM_IDENT_INT, val);
            break;

        case KEYWORD_FLOAT64:
            param = createLeaf(N_PARAM_IDENT_FLOAT, val);
            break;
        case KEYWORD_STRING:
            param = createLeaf(N_PARAM_IDENT_STR, val);
            break;
        default:
            error_exit(SYNTAX_ERROR, "Syntax error!");
        }
    }
    else
    {
        ungetToken(&stack);
        return NULL;
    }
    tok = getToken(&stack);
    if (tok->type == TOKEN_COMMA)
    {
        return createNode(SEQ, params(), param);
    }
    else
    {
        ungetToken(&stack);
        return createNode(SEQ, NULL, param);
    }
}

/* Spracovanie statementov - neterminál stmt */
Tree *stmt()
{
    Tree *root = NULL;
    Tree *help = NULL;

    /*"IF" SUBTREES */

    Tree *condition = NULL;
    Tree *if_cont = NULL;
    Tree *els_cont = NULL;

    /*"FOR" SUBTREES*/
    Tree *id_def = NULL;
    Tree *for_expr = NULL;
    Tree *id_init = NULL;
    Tree *leaf = NULL;

    if (stackEmpty(&stack))
    {
        return root;
    }
    tok = getToken(&stack);
    switch (tok->type)
    {
    /* Riešenie funkcie print */
    case FUNC_PRINT:
        expectToken(TOKEN_ROUND_LBRACKET);
        help = createNode(N_PRINT, terms(), NULL);
        expectToken(TOKEN_ROUND_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    /* Riešenie priradenia: var := expr, var = expr, var1,var2 = expr1,expr2 */
    case TOKEN_IDENTIF:
        help = stmt2(tok->value);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    /* Riešenie returnu */
    case KEYWORD_RETURN:
        tok = getToken(&stack);
        if (tok->type != TOKEN_EOL)
        {
            ungetToken(&stack);
            root = createNode(SEQ, NULL, createNode(N_RETURN, stmt4_5(), NULL));
        }
        else
        {
            root = createNode(SEQ, NULL, createNode(N_RETURN, NULL, NULL));
            ungetToken(&stack);
        }
        expectToken(TOKEN_EOL);
        break;
    /* Riešenie foru */
    case KEYWORD_FOR:

        /* Definícia premennej (ak existuje) */
        tok = getToken(&stack);
        if (tok->type == TOKEN_IDENTIF)
        {
            expectToken(IDENTIF_DEF);
            leaf = createLeaf(N_IDENTIFIER, tok->value);
            id_def = createNode(N_IDENT_DEF, leaf, expr(0));
        }
        else
        {
            ungetToken(&stack);
        }
        expectToken(TOKEN_SEMICOLON);

        /* Riešenie porovnávacej podmienky */
        for_expr = createNode(SEQ, NULL, expr(0));
        if (for_expr->Rptr == NULL)
        {
            error_exit(SYNTAX_ERROR, "For has not expr.")
        }
        expectToken(TOKEN_SEMICOLON);

        /* Riešenie príkazu priradenia (ak existuje) */
        tok = getToken(&stack);
        if (tok->type == TOKEN_IDENTIF)
        {
            expectToken(IDENTIF_INIT);
            leaf = createLeaf(N_IDENTIFIER, tok->value);
            id_init = createNode(N_IDENT_INIT, leaf, expr(0));
        }
        else
        {
            ungetToken(&stack);
        }

        for_expr->Lptr = createNode(SEQ, NULL, id_init);

        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        help = createNode(N_FOR, createNode(SEQ, for_expr, id_def), stmt());
        expectToken(TOKEN_CURLY_RBRACKET);
        optionalEOL();
        root = createNode(SEQ, stmt(), help);
        break;
    /* Riešenie ifu */
    case KEYWORD_IF:

        /* Vytvorenie podmienky */
        condition = createNode(SEQ, NULL, expr(0));
        if (condition->Rptr == NULL)
        {
            error_exit(SYNTAX_ERROR, "If has no condition.")
        }
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        if_cont = stmt();
        expectToken(TOKEN_CURLY_RBRACKET);
        expectToken(KEYWORD_ELSE);
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        els_cont = stmt();
        expectToken(TOKEN_CURLY_RBRACKET);
        optionalEOL();
        help = createNode(N_IF, condition, createNode(N_ELSE, els_cont, if_cont));
        root = createNode(SEQ, stmt(), help);
        break;
    /* Vstavané funkcie musia byť priradené premennej */
    case FUNC_INPUTS:
    case FUNC_INPUTF:
    case FUNC_INPUTI:
    case FUNC_INT2FLOAT:
    case FUNC_FLOAT2INT:
    case FUNC_LEN:
    case FUNC_SUBSTR:
    case FUNC_ORD:
    case FUNC_CHR:
        error_exit(SEM_ERROR_PARAMS, "Inbuilt functions have to be assigned to variable");
    case TOKEN_EOL:
        return stmt();
    default:
        ungetToken(&stack);
    }
    return root;
}

/* Riešenie priradenia  - neterminál stmt2 */
Tree *stmt2(char *name)
{
    Tree *root = NULL;
    TokenPtr identificator = tok;
    Tree *help = NULL;
    tok = getToken(&stack);
    switch (tok->type)
    {
    /*:=*/
    case IDENTIF_DEF:
        root = createNode(N_IDENT_DEF, createLeaf(N_IDENTIFIER, identificator->value), expr(0));
        if (root->Rptr == NULL)
        {
            error_exit(SYNTAX_ERROR, "Right side of EXPR is empty.")
        }
        break;
    /* = */
    case IDENTIF_INIT:
        root = createNode(N_IDENT_INIT, createLeaf(N_IDENTIFIER, identificator->value), expr(0));
        if (root->Rptr == NULL)
        {
            error_exit(SYNTAX_ERROR, "Right side of EXPR is empty.")
        }
        break;
    /* func() */
    case TOKEN_ROUND_LBRACKET:
        help = arg_coma();
        expectToken(TOKEN_ROUND_RBRACKET);
        root = createNode(N_FUNC, help, NULL);
        root->value = my_strdup(name);
        break;
    /* var1,var2 = expr1, expr2 */
    case TOKEN_COMMA:
        help = stmt3();
        tok = getToken(&stack);
        if (tok->type == IDENTIF_INIT || tok->type == IDENTIF_DEF)
        {
            root = createNode(N_IDENT_INIT, createNode(SEQ, help, createLeaf(N_IDENTIFIER, name)), stmt4_5());
        }
        else
        {
            error_exit(SYNTAX_ERROR, "Syntax error!");
        }
        if (root->Rptr == NULL)
        {
            error_exit(SYNTAX_ERROR, "Right side of EXPR is empty.")
        }
        break;
    }
    return root;
}

/* Rieši priradnie viacero premenným - neterminál stmt3 */
Tree *stmt3()
{
    Tree *root = NULL;
    char *val = NULL;
    tok = getToken(&stack);
    if (tok->type == TOKEN_IDENTIF)
    {
        val = tok->value;
        tok = getToken(&stack);
        if (tok->type == TOKEN_COMMA)
        {
            root = createNode(SEQ, stmt3(), createLeaf(N_IDENTIFIER, val));
        }
        else
        {
            ungetToken(&stack);
            root = createNode(SEQ, root, createLeaf(N_IDENTIFIER, val));
        }
    }
    else
    {
        error_exit(SYNTAX_ERROR, "Syntax error!");
    }
    return root;
}

/* rieši časť za priradením viacerým premenným (= expr1,expr2) - neterminál stmt4,5 */
Tree *stmt4_5()
{
    Tree *root = NULL;
    root = createNode(SEQ, root, expr(0));
    tok = getToken(&stack);
    if (tok->type == TOKEN_COMMA)
    {
        root->Lptr = stmt4_5();
    }
    else
    {
        ungetToken(&stack);
    }
    return root;
}

/* Rieši čiarky v argumentoch funckie, ktoré sú predávané ako parametre - neterminál arg_comma */
Tree *arg_coma()
{
    Tree *help = NULL;
    tok = getToken(&stack);
    while (tok->type == TOKEN_IDENTIF || tok->type == TOKEN_INT || tok->type == TOKEN_FLOAT || tok->type == TOKEN_STRING)
    {
        switch (tok->type)
        {
        case TOKEN_IDENTIF:
            help = createNode(SEQ, help, createLeaf(N_IDENTIFIER, tok->value));
            break;
        case TOKEN_INT:
            help = createNode(SEQ, help, createLeaf(N_LIT_INT, tok->value));
            break;
        case TOKEN_STRING:
            help = createNode(SEQ, help, createLeaf(N_LIT_STRING, tok->value));
            break;
        case TOKEN_FLOAT:
            help = createNode(SEQ, help, createLeaf(N_LIT_FLOAT, tok->value));
            break;
        default:
            break;
        }
        tok = getToken(&stack);
        if (tok->type == TOKEN_COMMA)
            tok = getToken(&stack);
    }
    ungetToken(&stack);
    return help;
}

/* Rieši termy v printe - neterminál terms */
Tree *terms()
{
    tok = getToken(&stack);
    switch (tok->type)
    {
    case TOKEN_INT:
        return createNode(SEQ, terms2(), createLeaf(N_LIT_INT, tok->value));
        break;
    case TOKEN_FLOAT:
        return createNode(SEQ, terms2(), createLeaf(N_LIT_FLOAT, tok->value));
        break;
    case TOKEN_STRING:
        return createNode(SEQ, terms2(), createLeaf(N_LIT_STRING, tok->value));
        break;
    case TOKEN_IDENTIF:
        return createNode(SEQ, terms2(), createLeaf(N_IDENTIFIER, tok->value));
        break;

    default:
        ungetToken(&stack);
        return NULL;
    }
}

/* Rieši čiarky pri termoch v printe - neterminál terms2 */
Tree *terms2()
{
    tok = getToken(&stack);
    if (tok->type == TOKEN_COMMA)
    {
        return terms();
    }
    ungetToken(&stack);
    return NULL;
}

/* Pomocou aplikácie precedenčnej tabulky vytvorí podstrom s výrazom */
Tree *expr(int precedence)
{
    TokenPtr t = NULL;
    TokenPtr tmp = NULL;
    Tree *root = NULL;
    Tree *help = NULL;
    char *name = NULL;
    bool ungetT = false;

    // Ošetrenie pre negatívne čislo: var:= -4
    if (tok->type == IDENTIF_DEF || tok->type == IDENTIF_INIT)
    {
        tmp = getToken(&stack);
        if (tmp->type == TOKEN_ROUND_LBRACKET)
        {
            tmp = getToken(&stack);
            if (tmp->type == TOKEN_MINUS)
            {
                tmp = getToken(&stack);
                if (tmp->type == TOKEN_INT || tmp->type == TOKEN_FLOAT)
                {
                    tmp = getToken(&stack);
                    if (tmp->type == TOKEN_ROUND_RBRACKET)
                    {
                        error_exit(SYNTAX_ERROR, "var := (-number) is invalid");
                    }
                    ungetToken(&stack);
                }
                ungetToken(&stack);
            }
            ungetToken(&stack);
        }
        else if (tmp->type == TOKEN_MINUS)
        {
            tmp = getToken(&stack);
            if (tmp->type == TOKEN_INT || tmp->type == TOKEN_FLOAT)
            {
                tmp = getToken(&stack);
                if (tmp->type == TOKEN_EOL)
                {
                    error_exit(SYNTAX_ERROR, "var := -number is invalid")
                }
                else
                {
                    ungetToken(&stack);
                }
            }
            else
            {
                ungetToken(&stack);
            }
        }
        ungetToken(&stack);
    }


    tok = getToken(&stack);
    switch (tok->type)
    {
    case TOKEN_IDENTIF:
        name = tok->value;
        tok = getToken(&stack);
        if (tok->type == TOKEN_ROUND_LBRACKET)
        {
            help = arg_coma();
            expectToken(TOKEN_ROUND_RBRACKET);
            root = createNode(N_FUNC, help, NULL);
            root->value = my_strdup(name);
        }
        else
        {
            root = createLeaf(N_IDENTIFIER, name);
            ungetToken(&stack);
        }
        break;
    case TOKEN_ROUND_LBRACKET:
        root = expr(0);
        expectToken(TOKEN_ROUND_RBRACKET);
        tok = getToken(&stack);
        ungetT = true;
        break;
    case FUNC_PRINT:
        error_exit(6, "Print cannot be in expression");
    case TOKEN_MINUS:
    case TOKEN_PLUS:
    case TOKEN_MUL:
    case TOKEN_DIV:
    case TOKEN_LESS:
    case TOKEN_LESS_EQUAL:
    case TOKEN_GREATER:
    case TOKEN_GREATER_EQUAL:
    case TOKEN_EQUAL:
    case TOKEN_NOT_EQUAL:
        root = expr(precedence);
        break;
    case TOKEN_INT:
    case TOKEN_FLOAT:
    case TOKEN_STRING:
        root = createLeaf(table[tok->type].node, tok->value);
        tok = getToken(&stack);
        ungetT = true;
        break;
    case FUNC_INPUTS:
    case FUNC_INPUTI:
    case FUNC_INPUTF:
        root = createLeaf(table[tok->type].node, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_LEN:
        root = createLeaf(table[tok->type].node, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);
        if (t->type == TOKEN_STRING || t->type == TOKEN_IDENTIF)
        {
            root->value = t->value;
        }
        else if (t->type == TOKEN_INT || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "LEN takes only STRING.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_CHR:
        root = createLeaf(table[tok->type].node, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);
        if (t->type == TOKEN_INT || t->type == TOKEN_IDENTIF)
        {
            root->value = t->value;
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "CHR takes only INT.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_ORD:
        root = createNode(table[tok->type].node, NULL, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);

        if (t->type == TOKEN_STRING || t->type == TOKEN_IDENTIF)
        {
            root->Lptr = createNode(SEQ, NULL, createLeaf(table[t->type].node, t->value));
        }
        else if (t->type == TOKEN_INT || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "ORD takes only string as a first argument.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }

        expectToken(TOKEN_COMMA);
        t = getToken(&stack);

        if (t->type == TOKEN_INT || t->type == TOKEN_IDENTIF)
        {
            root->Lptr->Lptr = createNode(SEQ, NULL, createLeaf(table[t->type].node, t->value));
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "ORD takes only INT as a second argument.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_SUBSTR:
        root = createNode(table[tok->type].node, NULL, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);
        if (t->type == TOKEN_STRING || t->type == TOKEN_IDENTIF)
        {
            root->Lptr = createNode(SEQ, NULL, createLeaf(table[t->type].node, t->value));
        }
        else if (t->type == TOKEN_INT || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "SUBSTR takes only STRING as a first argument.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_COMMA);
        t = getToken(&stack);
        if (t->type == TOKEN_INT || t->type == TOKEN_IDENTIF)
        {
            root->Lptr->Lptr = createNode(SEQ, NULL, createLeaf(table[t->type].node, t->value));
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "SUBSTR takes only INT as a second argument.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!")
        }

        expectToken(TOKEN_COMMA);
        t = getToken(&stack);
        if (t->type == TOKEN_INT || t->type == TOKEN_IDENTIF)
        {
            root->Lptr->Lptr->Lptr = createNode(SEQ, NULL, createLeaf(table[t->type].node, t->value));
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "SUBSTR takes only INT as a third argument.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!")
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_INT2FLOAT:
        root = createLeaf(table[tok->type].node, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);
        if (t->type == TOKEN_INT || t->type == TOKEN_IDENTIF)
        {
            root->value = t->value;
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_FLOAT)
        {
            error_exit(SEM_ERROR_PARAMS, "CHR takes only INT.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;
    case FUNC_FLOAT2INT:
        root = createLeaf(table[tok->type].node, NULL);
        expectToken(TOKEN_ROUND_LBRACKET);
        t = getToken(&stack);
        if (t->type == TOKEN_FLOAT || t->type == TOKEN_IDENTIF)
        {
            root->value = t->value;
        }
        else if (t->type == TOKEN_STRING || t->type == TOKEN_INT)
        {
            error_exit(SEM_ERROR_PARAMS, "CHR takes only INT.");
        }
        else
        {
            error_exit(SYNTAX_ERROR, "SYNTAX ERROR!");
        }
        expectToken(TOKEN_ROUND_RBRACKET);
        break;

    default:
        ungetToken(&stack);
    }

    /* Kontrola dvoch po sebe idúcich operátorov */
    if (ungetT)
    {
        switch (tok->type)
        {
        case TOKEN_PLUS:
        case TOKEN_MUL:
        case TOKEN_DIV:
        case TOKEN_LESS:
        case TOKEN_LESS_EQUAL:
        case TOKEN_GREATER:
        case TOKEN_GREATER_EQUAL:
        case TOKEN_EQUAL:
        case TOKEN_NOT_EQUAL:
        case TOKEN_MINUS:
            tmp = getToken(&stack);
            switch (tmp->type)
            {
            case TOKEN_PLUS:
            case TOKEN_MUL:
            case TOKEN_DIV:
            case TOKEN_LESS:
            case TOKEN_LESS_EQUAL:
            case TOKEN_GREATER:
            case TOKEN_GREATER_EQUAL:
            case TOKEN_EQUAL:
            case TOKEN_NOT_EQUAL:
            case TOKEN_MINUS:
                error_exit(SYNTAX_ERROR, "Two operatorts cannot go after each other.");
            default:
                ungetToken(&stack);
                break;
            }
        default:
            break;
        }
    }

    /* 
        Pokiaľ je má aktuálny token väčšiu precedenciu ako prechádzajúci, 
        tak funkcia rekurzívne do seba vnorí s precedenciou o 1 väčšiou
        a tým pádom sa zísakava prednosť operátorov
        Napríklad + má menšiu precedenciu ako *, tak tento cyklus preskočí.
        * má väčšiu precedenciu ako +, tak sa vnorí do tohto cyklu a 
        vytvorí sa z neho uzol v abstraktnom syntaktickom strome, pretože má presnosť pred +
     */
    while (tok && table[tok->type].precedence >= precedence)
    {
        int op = tok->type;
        help = expr(table[op].precedence + 1);
        root = createNode(table[op].node, root, help);
        ungetT = false;
    }
    if (ungetT)
    {
        ungetToken(&stack);
    }

    return root;
}
