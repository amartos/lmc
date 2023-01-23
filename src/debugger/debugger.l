/**
 * @file        debugger.l
 * @version     0.1.0
 * @brief       Module flex du debugger de l'Ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation
 * @code{.sh}
 * flex -o debugger.yy.c src/debugger/debugger.l
 * @endcode
 *
 * @addtogroup DebuggerInternals Structure internes au debugger.
 * @{
 */

 /**
  * @brief Options pour lex.
  * @details
  * noinput nounput désactivent les routines correspondantes, inutiles puisque
  * l'on fournit directement le texte à analyser.
  * noyywrap désactive yywrap, surtout pour les EOF.
  *
  * case-insensitive rend l'analyse de syntaxe casse-indépendante.
  *
  * prefix change le préfixe de yy à dbg, utile pour différencier les
  * analyseurs.
  */

 /**
  * Désactive les fonctions inutilisées.
  */
%option nounput noinput noyywrap

 /**
  * Rend l'analyse insensible à la casse. Le champ lexical et la
  * syntaxe du debugger sont assez simples pour le permettre.
  */
%option case-insensitive

%{

#include "lmc/computer.h"
#include "lmc/hashtable.h"

/**
 * Généré par bison.
 */
#include "debugger.tab.h"

%}

%%

 /**
  * Les patterns recherchés par le debugger. C'est bien simple, il n'y
  * en a que deux : des mots clés, ou des nombres hexadécimaux. Le
  * reste est géré dans le module bison correspondant.
  */

[ \t]*      {} /**< on ignore les whitespaces */
EOL         { return END; }
[a-z]+      { yylval.string = strdup(yytext); return KEYWORD; }             /**< mot-clé */
[0-9a-f]+   { yylval.value = (int)strtol(yytext, NULL, 16); return VALUE; } /**< code hexadécimal */
.           { fprintf(stderr, "unexpected: '%s'\n", yytext); } /**< tout le reste */

%%

    /* clang-format on */

// static int string_to_dbgcode(const char* restrict string)
// {
//     const struct {
//         const char* restrict string;
//         const DebugCode code;
//     } dbg_cmds[DBGMAX] = {
//         { "next",    DBGNEXT},
//         { "data",    DBGDATA},
//         {"trace",   DBGTRACE},
//         { "hide", DBGUNTRACE},
//         {"write",   DBGWRITE},
//         {"reset",   DBGRESET},
//         {  "run",     DBGRUN},
//         {"pause",   DBGPAUSE},
//         {"abort",    DBGABRT},
//         {"break",   DBGBREAK},
//         { "free", DBGUNBREAK},
//         { "help",    DBGHELP},
//         { "jump",    DBGJUMP},
//         { "goto",    DBGGOTO},
//         {      0,          0}
//     };
// }
