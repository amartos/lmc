/**
 * @file        hashtable.h
 * @version     0.1.0
 * @brief       Interface du module de gestion de la table de hachage.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation Cf. le fichier lmc.h
 *
 * @addtogroup Hashtable Gestion de la table de hachage.
 * @{
 */

#ifndef LMC_LEXER_H_
#define LMC_LEXER_H_

#include "lmc/computer.h"

#include <stdio.h>
#include <stdlib.h>

// Variables et fonctions du module flex nécessaires au bon
// fonctionnement.
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
int yyerror(LmcRam* startpos, LmcRam* size, FILE* output, const char* restrict source, const char* restrict msg);

#endif // LMC_LEXER_H_
