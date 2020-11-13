#include "libmine.h"

/* Inicializacia zasobniku */
void initStack(Stack *s)
{
    /* Inicializacia zasobniku zlyhala */
    if (s == NULL)
    {
        // Doplnit spravny exit code
        exit(0);
    }
    /* Alokovanie pamäte zasobniku */
    else
    {
        s->value = (void **)calloc(128, 128 * sizeof(void *));
        s->top = 0;
        s->size = 128;
    }
}

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
    else
    {
        debug_print("STACK IS EMPTY!");
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

void stackFree(Stack *s)
{
    free(s->value);
    s->value = NULL;
    s->top = 0;
    s->size = 0;
}

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

void stackPrint(Stack *s, char *displayToken[])
{
    stdout_print("\n*************************\n");
    for (int i = 0; i < s->top; i++)
    {
        TokenPtr tmp = (TokenPtr)s->value[i];
        stdout_print("-----------------\n");
        stdout_print("|\t%s\t|\t%s\n", displayToken[tmp->type], tmp->value);
        stdout_print("-----------------\n");
        UNUSED(tmp);
        UNUSED(displayToken);
    }
    stdout_print("*************************\n\n");
}