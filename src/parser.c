#include "libmine.h"

char *displayNode[] = {"SEQ", "PACKAGE_MAIN", "PARAMS_RETURNTYPES", "DEF_FUNC", "IDENTIFIER",
                       "PARAM_IDENT_INT", "PARAM_IDENT_STR", "PARAM_IDENT_FLOAT", "LIT_INT", "LIT_FLOAT", "LIT_STRING",
                       "RETURN_TYPE_INT", "RETURN_TYPE_STR", "RETURN_TYPE_FLOAT", "PRINT", "INPUTI", "INPUTS", "INPUTF", "DEF_VAR", "INIT_VAR", "FUNC",
                       "PLUS", "MINUS", "MULL", "DIV", "GREATER", "LESS", "EQ_GREATER", "EQ_LESS", "EQUAL", "NOT_EQUAL",
                       "IF", "ELSE", "RETURN", "LEN", "SUBSTR", "ORD", "CHR", "FOR", "INT2FLOAT", "FLOAT2INT"};

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
    /*33?*/
    {43, -1, N_FUNC},
};

TokenPtr tok;
Tree *ast;

int parser()
{
    ast = parse();
    Print_tree(ast);
    return 0;
}

Tree *parse()
{
    Tree *root = NULL;

    do
    {
        root = createNode(SEQ, root, prolog());
    } while (!stackEmpty(&stack));
    return root;
}

void checkForExcessEOL()
{
    if (!stackEmpty(&stack))
    {
        while (getToken(&stack)->type == TOKEN_EOL)
        {
            //debug_print("\n%s\n", (TokenPtr)stack->value[stack->top]);
            if (stackEmpty(&stack))
            {
                error_exit(SYNTAX_ERROR, "Syntax error");
            }
        }
        ungetToken(&stack);
    }
}

Tree *prolog()
{
    Tree *root = NULL;
    checkForExcessEOL();
    tok = getToken(&stack);
    switch (tok->type)
    {
    case KEYWORD_PACKAGE:
        expectToken(FUNC_MAIN);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, program(), createLeaf(PACKAGE_MAIN, NULL));
        break;
    default:
        error_exit(SYNTAX_ERROR, "Syntax error");
        break;
    }
    return root;
}

Tree *program()
{
    checkForExcessEOL();
    Tree *root = NULL;
    Tree *func = NULL;
    Tree *parameters = NULL;
    Tree *returntypes = NULL;
    Tree *params_returntypes = NULL;
    char *func_name = NULL;
    tok = getToken(&stack);
    if (tok == NULL)
        return NULL;
    /*TODO: Insert into tree, insert into symtable*/
    if (tok->type == KEYWORD_FUNC)
    {
        tok = getToken(&stack);
        if (tok->type == TOKEN_IDENTIF || tok->type == FUNC_MAIN)
        {
            func_name = my_strdup(tok->value);
        }
        else
        {
            error_exit(SYNTAX_ERROR, "Syntax error");
        }
        expectToken(TOKEN_ROUND_LBRACKET);
        parameters = params();
        expectToken(TOKEN_ROUND_RBRACKET);
        returntypes = rt();
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

Tree *rt()
{
    Tree *returntype = NULL;
    Tree *returntypeNode = NULL;
    tok = getToken(&stack);
    if (tok->type == TOKEN_ROUND_LBRACKET)
    {
        returntype = getRT();
        returntypeNode = createNode(SEQ, rt2(), returntype);
        expectToken(TOKEN_ROUND_RBRACKET);
        return returntypeNode;
    }
    else
    {
        ungetToken(&stack);
        return NULL;
    }
}

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

    default:
        error_exit(SYNTAX_ERROR, "Syntax error");
    }
}

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
            error_exit(SYNTAX_ERROR, "Syntax error");
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

    printf("I get here\n");
    if (stackEmpty(&stack))
    {
        printf("I get here too\n");
        return root;
    }
    tok = getToken(&stack);
    switch (tok->type)
    {
    case FUNC_PRINT:
        expectToken(TOKEN_ROUND_LBRACKET);
        help = createNode(N_PRINT, terms(), NULL);
        expectToken(TOKEN_ROUND_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    case TOKEN_IDENTIF:
        help = stmt2();
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;

    case KEYWORD_RETURN:
        root = createNode(SEQ, NULL, createNode(N_RETURN, NULL /*, expr()*/, expr(0)));
        expectToken(TOKEN_EOL);
        break;

    case KEYWORD_FOR:

        /*id_def*/
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

        /*for_expr*/
        for_expr = createNode(SEQ, NULL, expr(0));
        expectToken(TOKEN_SEMICOLON);

        /*id_init*/
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

        /*adding id_init as Lptr to for_expr*/
        for_expr->Lptr = createNode(SEQ, NULL, id_init);

        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        help = createNode(N_FOR, createNode(SEQ, for_expr, id_def), stmt());
        expectToken(TOKEN_CURLY_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    case KEYWORD_IF:

        condition = createNode(SEQ, NULL, expr(0));
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        if_cont = stmt();
        expectToken(TOKEN_CURLY_RBRACKET);
        expectToken(KEYWORD_ELSE);
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        els_cont = stmt();
        expectToken(TOKEN_CURLY_RBRACKET);
        expectToken(TOKEN_EOL);
        help = createNode(N_IF, condition, createNode(N_ELSE, els_cont, if_cont));
        root = createNode(SEQ, stmt(), help);
        break;
    case TOKEN_EOL:
        return stmt();
        /* FIX */
    /*case FUNC_INPUTS:
        expectToken(TOKEN_ROUND_LBRACKET);
        help = createLeaf(N_INPUTS, NULL);
        expectToken(TOKEN_ROUND_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    case FUNC_INPUTI:
        expectToken(TOKEN_ROUND_LBRACKET);
        help = createLeaf(N_INPUTI, NULL);
        expectToken(TOKEN_ROUND_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;
    case FUNC_INPUTF:
        expectToken(TOKEN_ROUND_LBRACKET);
        help = createLeaf(N_INPUTF, NULL);
        expectToken(TOKEN_ROUND_RBRACKET);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, stmt(), help);
        break;*/
    /* FIX */
    default:
        ungetToken(&stack);
    }
    return root;
}

Tree *stmt2()
{
    Tree *root = NULL;
    TokenPtr identificator = tok;
    tok = getToken(&stack);
    switch (tok->type)
    {
    /*:=*/
    case IDENTIF_DEF:
        root = createNode(N_IDENT_DEF, createLeaf(N_IDENTIFIER, identificator->value), expr(0));
        break;
    /* = */
    case IDENTIF_INIT:
        root = createNode(N_IDENT_INIT, createLeaf(N_IDENTIFIER, identificator->value), expr(0));
        break;
    }
    return root;
}

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
        error_exit(SYNTAX_ERROR, "Syntax error");
    }
}

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

Tree *expr(int precedence)
{
    Tree *root = NULL;
    Tree *help = NULL;
    char *name = NULL;
    bool ungetT = false;
    tok = getToken(&stack);
    switch (tok->type)
    {
    case TOKEN_IDENTIF:
        name = tok->value;
        tok = getToken(&stack);
        if (tok->type == TOKEN_ROUND_LBRACKET)
        {
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
    case TOKEN_MINUS:
        //tok = getToken(&stack);
        /* Pridanie "-" pred hodnotu */
        /*if (tok->type == TOKEN_INT || tok->type == TOKEN_FLOAT)
        {
            char *buff = (char *)malloc(strlen(tok->value) + 2);
            strcpy(buff, "-");
            strcat(buff, tok->value);
            free(tok->value);
            tok->value = buff;
        }
        ungetToken(&stack);*/
        root = expr(precedence);
        break;
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
    default:
        ungetToken(&stack);
    }

    while (tok && table[tok->type].precedence >= precedence)
    {
        int op = tok->type;
        //tok = getToken(&stack);
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

void prt_ast(Tree *t)
{
    if (t == NULL)
        debug_print("NULL\n");
    else
    {
        debug_print("%s ", displayNode[t->type]);
        if (t->type == N_IDENTIFIER || t->type == N_LIT_INT || t->type == N_LIT_FLOAT || t->type == N_LIT_STRING)
        {
            debug_print("%s\n", t->value);
        }
        else
        {
            debug_print("\n");
            prt_ast(t->Lptr);
            prt_ast(t->Rptr);
        }
    }
}

void Print_tree2(Tree *TempTree, char *sufix, char fromdir)
{
    /* vykresli sktrukturu binarniho stromu */
    if (TempTree != NULL)
    {
        char *suf2 = (char *)malloc(strlen(sufix) + 4);
        strcpy(suf2, sufix);

        if (fromdir == 'L')
        {
            suf2 = strcat(suf2, "  |");
            fprintf(stderr, "%s\n", suf2);
        }
        else
            suf2 = strcat(suf2, "   ");

        Print_tree2(TempTree->Rptr, suf2, 'R');
        if (TempTree->value)
            fprintf(stderr, "%s  +-[ (%d) %s \"%s\" ]\n", sufix, TempTree->type, displayNode[TempTree->type], TempTree->value);
        else
            fprintf(stderr, "%s  +-[ (%d) %s ]\n", sufix, TempTree->type, displayNode[TempTree->type]);

        strcpy(suf2, sufix);

        if (fromdir == 'R')
            suf2 = strcat(suf2, "  |");
        else
            suf2 = strcat(suf2, "   ");

        Print_tree2(TempTree->Lptr, suf2, 'L');

        if (fromdir == 'R')
            fprintf(stderr, "%s\n", suf2);

        free(suf2);
    }
}

void Print_tree(Tree *TempTree)
{
    fprintf(stderr, "===========================================\n");
    fprintf(stderr, "Struktura binarniho stromu:\n");
    fprintf(stderr, "\n");

    if (TempTree != NULL)
        Print_tree2(TempTree, "", 'X');
    else
        fprintf(stderr, "Strom je prazdny...\n");

    fprintf(stderr, "\n");
    fprintf(stderr, "===========================================\n");
}
