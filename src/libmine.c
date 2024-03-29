/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Implementacia kniznice pre spracovanie retazcov
 * Autori: Peter Koprda - xkoprd00, Daniel Paul - xpauld00
 */

#include "libmine.h"

char *my_strdup(const char *str)
{
    char *newStr = (char *)malloc(strlen(str) + 1);

    if (!newStr)
    {
        error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
    }
    else
        strcpy(newStr, str);

    return newStr;
}

string *initBuffer()
{
    string *buffer = (string *)calloc(LINE_BUFF_LEN, LINE_BUFF_LEN * sizeof(char));
    if (!buffer)
    {
        error_exit(INTERNAL_ERROR, "Allocation error!");
    }
    return buffer;
}

void clearBuffer(string *buffer)
{
    buffer->length = 0;
    buffer->str[0] = '\0';
}

void freeBuffer(string *buffer)
{
    free(buffer);
    buffer = NULL;
}

TokenPtr initToken(int type, char *value)
{
    TokenPtr token = malloc(sizeof(struct tToken));
    if (!token)
    {
        error_exit(INTERNAL_ERROR, "Allocation error!");
    }
    token->type = type;
    token->value = value;
    return token;
}

void ungetToken(Stack *s)
{
    s->top++;
}

TokenPtr getToken(Stack *s)
{
    if (stackEmpty(s))
    {
        return NULL;
    }
    TokenPtr tmp = (TokenPtr)stackTop(s);
    stackPop(s);
    return tmp;
}

/* Funkcia, ktorá skontroluje, či sa ďalší token riadi pravidlom gramatiky */
void expectToken(int type)
{
    if (stackEmpty(&stack))
    {
        error_exit(SYNTAX_ERROR, "Syntax error");
    }
    if (!(getToken(&stack)->type == type))
    {
        error_exit(SYNTAX_ERROR, "Syntax error");
    }
}

/* Vytvorenie uzlu v abstraktnom syntaktickom strome */
Tree *createNode(int type, Tree *Lptr, Tree *Rptr)
{
    Tree *tree = malloc(sizeof(struct Tree));
    if (tree == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }
    tree->type = type;
    tree->value = NULL;
    tree->Lptr = Lptr;
    tree->Rptr = Rptr;
    return tree;
}

/* Vytvorenie v abstraktnom syntaktickom strome */
Tree *createLeaf(int type, char *value)
{
    Tree *tree = malloc(sizeof(struct Tree));
    if (tree == NULL)
    {
        error_exit(INTERNAL_ERROR, "Failed to allocate memory");
    }
    tree->type = type;
    tree->value = value;
    tree->Lptr = NULL;
    tree->Rptr = NULL;
    return tree;
}

/**
 * Funkcia prevzana a upravená zo stránok IFJ
 */
void initString(string *s)
{
    s->str = (char *)malloc(sizeof(char) * SIZE_STRING_INC);
    if (!s->str)
    {
        error_exit(INTERNAL_ERROR, "Allocation error!");
    }
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = SIZE_STRING_INC;
}

/**
 * Funkcia prevzana a upravená zo stránok IFJ
 */
string *addChar(string *s, char c)
{
    if (s->length + 1 >= s->allocSize)
    {
        s->str = (char *)realloc(s->str, s->length + SIZE_STRING_INC);
        if (!s->str)
        {
            error_exit(INTERNAL_ERROR, "Allocation error!");
        }
        s->allocSize = s->length + SIZE_STRING_INC;
    }
    s->str[s->length] = c;
    s->length++;
    s->str[s->length] = '\0';
    return s;
}

void convertString(char *val)
{
    if (!val)
        return;

    val++;
    val[strlen(val) - 1] = '\0';

    char *str = my_strdup(val);
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (str[i] == '\\')
        {
            i++;

            if (str[i] == 't')
            {
                printf("\\009");
            }
            else if (str[i] == 'n')
            {
                printf("\\010");
            }
            else if (str[i] == '"')
            {
                printf("\\034");
            }
            else if (str[i] == '\\')
            {
                printf("\\092");
            }
            else if (str[i] == 'x')
            {
                i++;
                char hex[3] = {0};
                hex[0] = str[i++];
                hex[1] = str[i++];
                printf("\\%03ld", strtol(hex, NULL, 16));
            }
            continue;           
        }
        if (str[i] == ' ')
        {
            printf("\\032");
            continue;
        }
        else if (str[i] == '#')
        {
            printf("\\035");
            continue;
        }
        printf("%c", str[i]);
    }
}