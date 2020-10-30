#include "libmine.h"

TokenPtr tmptoken;
Tree *ast;

int parser()
{
    ast = parse();
    return 0;
}

Tree *parse()
{
    Tree *root = NULL;

    do
    {
        tmptoken = getToken(&stack);
        root = createNode(0, tmptoken->value, NULL, NULL);
        printf("yay %s", root->value);
    } while (!stackEmpty(&stack));
    return root;
}
