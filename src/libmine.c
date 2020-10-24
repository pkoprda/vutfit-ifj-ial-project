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
        error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
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
        error_exit(INTERNAL_ERROR, "Chyba alokacie pamäti!");
    }
    token->type = type;
    token->value = value;
    return token;
}

void initString(string *s)
{
    s->str = (char *)malloc(sizeof(char) * SIZE_STRING_INC);
    if (!s->str)
    {
        error_exit(INTERNAL_ERROR, "Chyba alokacie pameti!");
    }
    s->str[0] = '\0';
    s->length = 0;
    s->allocSize = SIZE_STRING_INC;
}

string *addChar(string *s, char c)
{
    if (s->length + 1 >= s->allocSize)
    {
        s->str = (char *)realloc(s->str, s->length + SIZE_STRING_INC);
        if (!s->str)
        {
            error_exit(INTERNAL_ERROR, "Chyba alokacie pameti!");
        }
        s->allocSize = s->length + SIZE_STRING_INC;
    }
    s->str[s->length] = c;
    s->length++;
    s->str[s->length] = '\0';
    return s;
}
