/**
 * @file        lexer.y
 * @version     0.1.0
 * @brief       Module bison du traducteur de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation
 * @code{.sh}
 * bison -d -o lexer.tab.c src/lexer/lexer.y
 * @endcode
 *
 * @addtogroup LexerInternals
 * @{
 */

%{

// clang-format off

/******************************************************************************
 * Fichier en-tête généré par bison.
 ******************************************************************************/
// clang-format on

#include "lmc/computer.h"
#include "lmc/lexer.h"

#include <err.h>
#include <errno.h>
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
    LmcOpCodes operation;
    LmcRam value;
};

// Définition des divers items possibles et de leur type. Les
// mots-clés, représentés par KEYWORD, sont forcément des l-values, et
// donc positionnés à gauche ("left"); leurs arguments, représentés
// par VALUE, sont des r-values, donc forcément à droite
// ("right"). START est la commande indiquant l'adresse de départ
// relative (à la dernière adresse de mémoire morte) du programme,
// et STARTABS est sa contrepartie absolue (la valeur donnée est
// l'adresse réelle). Ce sont deux items spéciaux, comme des
// mots-clés, mais leur traitement sera légèrement différent.

%left   <string>        KEYWORD
%token  <string>        POINTER
%token  <value>         VALUE
%token                  EOL
%type   <operation>     keyword
%type   <value>         arg

// Paramètres supplémentaires pour la fonction d'analyse yyparser; ils
// permettent de stocker des informations à utiliser plus tard
// (startpos, size), ou de passer des paramètres aux fonctions
// internes.
%parse-param { LmcLexerCallback callback } { LmcRam* header } { const char* restrict desc }

// Le point de départ de l'analyse.
%start line

%%

// clang-format off

/******************************************************************************
 * États de l'automate et liaisons.
 ******************************************************************************/
// clang-format on

// Permet d'analyser plusieurs lignes d'un fichier (sinon l'automate
// s'arrête après la première).
line: | line expr;

// Une ligne contenant une commande.
expr:
        keyword arg { callback(header, $1, $2); }
|               EOL {}
;

// conversion des mots-clés; elle est traitée à part du fait de la
// possibilité de l'adressage, qui doit être combiné à la valeur du
// code d'opération.
keyword:
            KEYWORD { $$ = lmc_opcode($1), free($1); }
|   KEYWORD POINTER {
                $$ = lmc_opcode($1) | lmc_opcode($2);
                free($1), free($2);
    }
|             VALUE { $$ = $1; }
;

arg:
              VALUE { $$ = $1; }
|               EOL { $$ = 0; }
;

%%

// clang-format off

/******************************************************************************
 * API de flex.
 ******************************************************************************/
// clang-format on

// cette fonction du module flex doit être implémentée par
// l'utilisateur.
int yyerror(LmcLexerCallback callback, LmcRam* header, const char* restrict desc, const char* restrict msg) {
    (void) callback;
    (void) header;
    fprintf(stderr, "%s: %s at line %i: '%s'\n", desc, msg, yylineno, yytext);
    return EXIT_FAILURE;
}
