/**
 * @file        debugger.y
 * @version     0.1.0
 * @brief       Module bison du debugger de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation
 * @code{.sh}
 * bison -d -o debugger.tab.c src/debugger/debugger.y
 * @endcode
 *
 * @addtogroup DebuggerInternals
 * @{
 */

%{

// clang-format off

/******************************************************************************
 * Fichier en-tête généré par bison.
 ******************************************************************************/
// clang-format on

#include "lmc/computer.h"
#include "lmc/hashtable.h"
#include "lmc/lexer.h"

// Fichier en-tête servant à partager une interface avec le reste du
// programme.
#include "lmc/debugger.h"

#include <stdio.h>
#include <stdlib.h>

%}

// clang-format off

/******************************************************************************
 * Définition des types de données analysée.
 ******************************************************************************/
// clang-format on

// Les type de données rencontrées lors de l'analyse.
%union {
    char* string;
    LmcDbgCodes instruction;
    LmcRam value;
};

// définition des divers items possibles et de leur type. Les
// mots-clés, représentés par KEYWORD, sont forcément des l-values, et
// donc positionnés à gauche ("left"); leurs arguments, représentés
// par VALUE, sont des r-values, donc forcément à droite
// ("right"). END est un token représentant la fin de ligne.

%left   <string>        KEYWORD
%right  <value>         VALUE
%type   <instruction>   keyword
%token                  END

// Paramètres supplémentaires pour yyparse, pour stocker les données
// obtenues.
%parse-param { LmcDbg* instruction } { LmcRam* value }

%%

// clang-format off

/******************************************************************************
 * États de l'automate et liaisons.
 ******************************************************************************/
// clang-format on

// Une ligne contenant une ou plus commandes.
expr:
    keyword END { *instruction = $1; }
|   keyword VALUE END { *instruction = $1; *value = $2; }
;

// Un ou plusieurs mots clés qui se suivent.
keyword:
KEYWORD { $$ = lmchashget($1, false); free($1); }
|   KEYWORD keyword { $$ = lmc_hashget($1, false) | $2; free($1); }
;

%%

// clang-format off

/******************************************************************************
 * Gestion des données analysées.
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Initialise le debugger.
 */
static void lmc_debuggerInit(void) __attribute__((constructor));

/**
 * @var lmc_dbg_mnemonics
 * @since 0.1.0
 * @brief Contient la correspondance des mots-clés <> code
 * d'instruction de debuggage.
 */
static LmcKeyword lmc_dbg_mnemonics[DBGMAX] ={
    { "next",    DBGNEXT},
    { "data",    DBGDATA},
    {"trace",   DBGTRACE},
    { "hide", DBGUNTRACE},
    {"write",   DBGWRITE},
    {"reset",   DBGRESET},
    {  "run",     DBGRUN},
    {"pause",   DBGPAUSE},
    {"abort",    DBGABRT},
    {"break",   DBGBREAK},
    { "free", DBGUNBREAK},
    { "help",    DBGHELP},
    { "jump",    DBGJUMP},
    { "goto",    DBGGOTO},
    {   NULL,          0}
};

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

static void lmc_debuggerInit(void) { lmc_hashInsertList(lmc_dbg_mnemonics); }

// On implémente lmc_debuggerInput comme alias de yyparse, puisque les
// fonctions remplissent exactement le même rôle.
extern __typeof__(yyparse) lmc_debuggerInput __attribute__((alias("yyparse")));
