#include "symtable.h"

#define PRINT_CODE(M, ...)               \
    fprintf(stdout, "" M "", ##__VA_ARGS__) 

#define PRINT_NL() \
    fprintf(stdout, "\n")


int getIDtype(char *function, char *ID, int forcnt, int ifcnt, int hide);
SymTItem *getvalue(Tree *ast);

void generate_label(Tree *ast);
void generate_function(Tree *ast);

void generate_var_def(Tree *ast);
void generate_multivar_init(Tree *vars, Tree *expr);

void generate_condition(Tree *ast, int count,  int type);

void generate_constant(int type, char* value);

void calculate_expr(Tree *ast);
void gen_expr(Tree *ast);
void generate_print(Tree *ast);

void gen_initvar_call(Tree *ast, char *identif);
void generate_call(Tree *ast);
void generate_return(Tree *ast);
void generate_return_types(Tree *ast);


void input_errvalue();
void input_okvalue();