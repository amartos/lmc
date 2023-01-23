/**
 * @file        lexer.c
 * @version     0.1.0
 * @brief       Fichier source du module lexer.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 */

#include "lmc/lexer.h"

// clang-format off

/******************************************************************************
 * API de *lex et yacc.
 ******************************************************************************/
// clang-format on

// cette fonction du module flex doit être implémentée par
// l'utilisateur.
int yyerror(LmcRam* startpos, LmcRam* size, FILE* output, const char* restrict source, const char* restrict msg)
{
    (void) startpos;
    (void) size;
    (void) output;
    fprintf(stderr, "%s: %s at line %i: '%s'\n", source, msg, yylineno, yytext);
    return EXIT_FAILURE;
}
