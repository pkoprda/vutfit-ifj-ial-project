/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Implementacia zasobniku
 * Autor: Daniel Paul - xpauld00
 */

#include "libmine.h"

/* Inicializacia zasobniku */
void initStack(Stack *s)
{
    /* Inicializacia zasobniku zlyhala */
    if (s == NULL)
    {
        error_exit(INTERNAL_ERROR, "Internal error! Could not initialize stack!");
    }

    /* Alokovanie pamäte zasobniku */
    else
    {
        s->value = (void **)calloc(128, 128 * sizeof(void *));
        s->top = 0;
        s->size = 128;
    }
}

/* Zasobnik je prazdny -> funkcia vrati 0 */
int stackEmpty(Stack *s)
{
    return s->top == 0;
}

/* Vrati vrchol zasobniku */
void *stackTop(Stack *s)
{
    if (s->top == 0)
    {
        return NULL;
    }
    else
    {
        return s->value[s->top - 1];
    }
}

/* Uvolni najvrchnejsi prvok zo zasobniku */
void stackPop(Stack *s)
{
    if (s->top != 0)
    {
        s->top--;
    }
}

/* Vlozi prvok na vrchol zasobniku */
void stackPush(Stack *s, void *p)
{
    if (s->top == s->size)
    {
        s->size *= 2;
        s->value = (void **)realloc(s->value, s->size * sizeof(void *));
    }

    s->value[s->top++] = p;
}

/* Uvolni pamat a vyprazdni zasobnik */
void stackFree(Stack *s)
{
    free(s->value);
    s->value = NULL;
    s->top = 0;
    s->size = 0;
}

/* Otoci zasobnik */
void stackFlip(Stack *s)
{
    void *tmpValue;
    int j = s->top - 1;
    for (int i = 0; i < (s->top / 2); ++i)
    {
        tmpValue = s->value[i];
        s->value[i] = s->value[j];
        s->value[j--] = tmpValue;
    }
}
