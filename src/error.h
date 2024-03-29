/**
 * Projekt: Prekladac jazyka  IFJ20 do medzikodu IFJcode20
 * Popis: Navratove hodnoty prekladaca
 * Autor: Peter Koprda - xkoprd00
 */

#ifndef _ERROR_H
#define _ERROR_H

#define SCANNER_OK 0
#define LEX_ERROR 1
#define SYNTAX_ERROR 2
#define SEM_ERROR_UNDEF 3
#define SEM_ERROR_DATA_TYPE 4
#define SEM_ERROR_TYPE 5
#define SEM_ERROR_PARAMS 6
#define SEM_ERROR_OTHERS 7
#define DIVISION_ZERO_ERROR 9
#define INTERNAL_ERROR 99
#endif