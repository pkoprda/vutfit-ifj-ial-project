#include "libmine.h"

char *displayNode[] = {"SEQ", "PACKAGE_MAIN", "PARAMS_RETURNTYPES", "DEF_FUNC", "IDENTIFIER",
                       "PARAM_IDENT_INT", "PARAM_IDENT_STR", "PARAM_IDENT_FLOAT", "LIT_INT", "LIT_FLOAT", "LIT_STRING",
                       "RETURN_TYPE_INT", "RETURN_TYPE_STR", "RETURN_TYPE_FLOAT"};

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

Tree *prolog()
{
    Tree *root = NULL;
    tok = getToken(&stack);
    switch (tok->type)
    {
    case TOKEN_EOL:
        while (getToken(&stack)->type == TOKEN_EOL)
        {
            //debug_print("\n%s\n", (TokenPtr)stack->value[stack->top]);
            if (stackEmpty(&stack))
            {
                error_exit(SYNTAX_ERROR, "Syntax error");
            }
        }
        ungetToken(&stack);
        tok = getToken(&stack);
    case KEYWORD_PACKAGE:
        expectToken(FUNC_MAIN);
        expectToken(TOKEN_EOL);
        root = createNode(SEQ, program(), createLeaf(PACKAGE_MAIN, NULL));
        break;
    default:
        error_exit(SYNTAX_ERROR, "Syntax error");
        break;

        /*case KEYWORD_FUNC:
        expectToken(TOKEN_IDENTIF);
        expectToken(TOKEN_ROUND_LBRACKET);
        //params
        expectToken(TOKEN_ROUND_RBRACKET);
        //return types
        expectToken(TOKEN_ROUND_LBRACKET);
        expectToken(TOKEN_ROUND_RBRACKET);

        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        //stmt
        expectToken(TOKEN_CURLY_RBRACKET);
        //program
        break;

    case KEYWORD_IF:
        //CHECK
        //expr
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        //stmt
        expectToken(TOKEN_CURLY_RBRACKET);
        expectToken(KEYWORD_ELSE);
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        //stmt
        expectToken(TOKEN_CURLY_RBRACKET);
        //stmt
        break;
    case KEYWORD_FOR:
        //id_def
        expectToken(TOKEN_SEMICOLON);
        expectToken(TOKEN_EOL);

        break;*/
    }

    return root;
}

Tree *program()
{
    Tree *root = NULL;
    Tree *func = NULL;
    Tree *parameters = NULL;
    Tree *returntypes = NULL;
    Tree *params_returntypes = NULL;
    tok = getToken(&stack);
    if (tok == NULL)
        return NULL;
    /*TODO: Insert into tree, insert into symtable*/
    if (tok->type == KEYWORD_FUNC)
    {
        expectToken(TOKEN_IDENTIF);
        expectToken(TOKEN_ROUND_LBRACKET);
        parameters = params();
        expectToken(TOKEN_ROUND_RBRACKET);
        returntypes = rt();
        expectToken(TOKEN_CURLY_LBRACKET);
        expectToken(TOKEN_EOL);
        params_returntypes = createNode(N_PARAMS_RETURNTYPES, parameters, returntypes);
        func = createNode(N_DEF_FUNC, params_returntypes, stmt());
        expectToken(TOKEN_CURLY_RBRACKET);
        optionalEOL();
        root = createNode(SEQ, program(), func);
    }

    return root;
}

void optionalEOL()
{
    tok = getToken(&stack);
    if (tok->type != TOKEN_EOL)
    {
        ungetToken(&stack);
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
    //tok = getToken(&stack);
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
