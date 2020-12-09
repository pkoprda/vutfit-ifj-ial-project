/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Rozhranie pre generaciu kodu
 * Autori: Peter Koprda - xkoprd00, Daniel Paul - xpauld00
 */

#include "symtable.h"

#define PRINT_CODE(M, ...)               \
    fprintf(stdout, "" M "", ##__VA_ARGS__) 

#define PRINT_NL() \
    fprintf(stdout, "\n")


int getIDtype2(char *function, char *ID, int forcnt, int ifcnt, int hide);

/* ziskanie identifikatoru z hashtable */
SymTItem *getvalue(Tree *ast);

/* generovanie  navesti pre funkcie */
void generate_label(Tree *ast);

/* generovanie tela funkcie */
void generate_function(Tree *ast);

/* generovanie definicie pre jednu premennu */
void generate_var_def(Tree *ast);

/* generovanie inicializacie pre viacere premenne */
void generate_multivar_init(Tree *vars, Tree *expr);

/* generovanie podmienky */
void generate_condition(Tree *ast, int count,  int type);

/* generovanie konstanty - premenna alebo literal */
void generate_constant(int type, char* value);

/* Vypočíta výraz pomocou zásobníka a postorder prechodom podstromom AST s výrazom */
void calculate_expr(Tree *ast);

/* Vygeneruje výsledok výrazu */
void gen_expr(Tree *ast);

/* generovanie instrukcie WRITE */
void generate_print(Tree *ast);

/* generovanie instrukcie CONCAT */
void gen_string_concat(Tree *ast, char *id);

/* vytvorenie docasneho ramca a zavolanie funkcie */
void gen_initvar_call(Tree *ast, char *identif);

/* vytvorenie premennych na docasnom ramci, ktore su presunute na lokalny ramec funkcie pri zavolani funkcie */
void generate_call(Tree *ast);

/* presunutie navratovych hodnot z funkcie do premennych */
void generate_return(Tree *ast);

/* priradenie hodnot do lokalneho ramca, ktore su pouzite v danej funkcii */
void generate_return_types(Tree *ast);
