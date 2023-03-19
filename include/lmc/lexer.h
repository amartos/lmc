/**
 * @file        lexer.h
 * @version     0.1.0
 * @brief       Interface du traducteur.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup Lexer
 * @{
 */

#ifndef LMC_LEXER_H_
#define LMC_LEXER_H_

#include "lmc/computer.h"

#include <err.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @typedef LmcLexerCallback
 * @brief Type de fonction de rappel à utiliser lors de la traduction.
 */
typedef void (LmcLexerCallback)(LmcRam code, LmcRam arg);

// Variables et fonctions du module flex nécessaires au bon
// fonctionnement lors de l'analyse de texte.
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
int yylex(void);
int yyerror(LmcLexerCallback callback, const char* restrict desc, const char* restrict msg)
    __attribute__((nonnull));
int yyparse(LmcLexerCallback callback, const char* restrict desc);

/**
 * @since 0.1.0
 * @brief Traduit la chaîne donnée en un code d'opération.
 * @param word Une chaîne contenant un mot.
 * @return le code correspondant au mot, ou 0 si le mot n'a aucun code
 * correspondant.
 */
LmcOpCodes lmc_stringToOpCode(char* word) __attribute__((nonnull));

#endif // LMC_LEXER_H_
/** @} */
