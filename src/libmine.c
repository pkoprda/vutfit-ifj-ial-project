#include "libmine.h"

char *my_strdup(const char *str)
{
    char *newStr = (char *)malloc(strlen(str) + 1);

    if (!newStr)
    {
        // Bad malloc
        error_exit(INTERNAL_ERROR, "Allocation error!");
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

void ungetToken(Stack *s)
{
    s->top++;
}

TokenPtr getToken(Stack *s)
{
    if (stackEmpty(s))
    {
        debug_print("GET TOKEN RETURNED NULL");
        return NULL;
    }
    TokenPtr tmp = (TokenPtr)stackTop(s);
    stackPop(s);
    return tmp;
}

void expectToken(int type)
{
    if (!(getToken(&stack)->type == type))
    {
        error_exit(SYNTAX_ERROR, "Syntax error");
    }
}

Tree *createNode(int type, Tree *Lptr, Tree *Rptr)
{
    Tree *tree = malloc(sizeof(struct Tree));
    //if (tree == NULL)
    //error_exit(99, "Failed to allocate memory");
    tree->type = type;
    tree->value = NULL;
    tree->Lptr = Lptr;
    tree->Rptr = Rptr;
    return tree;
}

Tree *createLeaf(int type, char *value)
{
    Tree *tree = malloc(sizeof(struct Tree));
    //if (tree == NULL)
    //error_exit(99, "Failed to allocate memory");
    tree->type = type;
    tree->value = value;
    tree->Lptr = NULL;
    tree->Rptr = NULL;
    return tree;
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

int hashCode(tKey key, int size)
{
    int retval = 1;
    int keylen = strlen(key);
    for (int i = 0; i<keylen; i++){
        retval += key[i];
    }
    return (retval % size);
}

void ftInit(FunTable *fun)
{
	for (int i = 0 ; i<FTsize; i++){
		(*fun)[i] = NULL;
	}
}

void stInit(SymTable *sym)
{
	for (int i = 0 ; i<STsize; i++){
		(*sym)[i] = NULL;
	}
}

void newFun(FunTable *fun, tKey key, int retvar, int count, int types, SymTable *sym)
{
    FunTItem *tmp = ftSearch(fun, key);
    if (tmp != NULL){
        error_exit(3, "Function is defined yet");
    } else{
        FunTItem *new = malloc(sizeof(FunTItem));
        //if (new == NULL)
        //error_exit(99, "Failed to allocate memory");
        new->key = key;
        new->count = count;
        new->retvar = retvar;
        new->types = types;
        new->sym = sym;
        int hash = hashCode(key, FTsize);
        new->next = (*fun)[hash];
        (*fun)[hash] = new;
    }
}

FunTItem *ftSearch(FunTable *fun, tKey key)
{
    int hash = hashCode(key, FTsize);
    FunTItem *tmp = (*fun)[hash];
    while (tmp != NULL){
        if (!strcmp(tmp->key, key)){
            return tmp;
        }
        tmp = tmp->next;
    }
	return NULL;
}

void newSym(tKey key, int type, char *value, SymTable *sym)
{
    SymTItem *tmp = stSearch(sym, key);
    if (tmp != NULL){
        error_exit(3, "Variable is defined yet");
    } else{
        SymTItem *new = malloc(sizeof(SymTItem));
        //if (new == NULL)
        //error_exit(99, "Failed to allocate memory");
        new->key = key;
        new->type = type;
        new->value = value;
        int hash = hashCode(key, FTsize);
        new->next = (*sym)[hash];
        (*sym)[hash] = new;
    }
}

SymTItem *stSearch(SymTable *sym, tKey key)
{
    int hash = hashCode(key, STsize);
    SymTItem *tmp = (*sym)[hash];
    while (tmp != NULL){
        if (!strcmp(tmp->key, key)){
            return tmp;
        }
        tmp = tmp->next;
    }
	return NULL;
}