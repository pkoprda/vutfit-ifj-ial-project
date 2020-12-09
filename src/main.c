/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Hlavny subor prekladaca
 * Autori: Peter Koprda - xkoprd00, Daniel Paul - xpauld00,
 *         Viliam Holik - xholik14, Pavol Babjak - xbabja03 
 */

#include "libmine.h"

int main()
{
    lexer();
    parser();
    semantics();
    generate();
    return 0;
}