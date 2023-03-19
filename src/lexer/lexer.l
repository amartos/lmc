/**
 * @file        lexer.l
 * @version     0.1.0
 * @brief       Module flex du traducteur de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation
 * @code{.sh}
 * flex -o lexer.yy.c src/lexer/lexer.l
 * @endcode
 *
 * @addtogroup LexerInternals
 * @{
 */

%{

#include "lmc/computer.h"
#include "lmc/lexer.h"

/**
 * Le header du fichier généré par bison. Il n'existe pas en dehors de
 * la compilation.
 */
#include "lexer.tab.h"

%}

/**
 * Désactive certaines fonctions de l'interface *lex/yacc qui ne sont
 * pas utilisées ici.
 */
%option noinput nounput noyywrap

/**
 * On rend le langage indépendant de la casse. Il n'est pas complexe,
 * cela ne devrait donc pas poser de problèmes. Pour des langages plus
 * poussés, cette option serait déconseillée.
 */
%option case-insensitive

/**
 * Définition d'une directive pour les blocks de commentaires.
 */
%x BLOCK

%%

 /**
  * @name Syntaxe du langage
  *
  * Un mot-clé supplémentaire est utilisé par rapport aux mnémoniques
  * demandées ("start"). Il permet, optionnellement, d'indiquer
  * l'adresse de départ pour le programme.
  * @{
  */

EOF            { yylineno = 0; return EOF; }
\n             { ++yylineno; }
[ \t]*         {} /**< on ignore les espaces */
("#"|";"|"//")[^\n]*? {} /** on ignore les sauts de lignes et commentaires (mais on compte les lignes). */
"/*"           { BEGIN(BLOCK); }    /**< blocks de commentaires style c */
<BLOCK>\n      { ++yylineno; }      /**< les sauts de ligne ne sont pas ignorés par la règle suivante. */
<BLOCK>.       {}                   /**< on ignore tout le reste */
<BLOCK>"*/"    { BEGIN(INITIAL); }  /**< fin des blocks */
x[[:xdigit:]]+ { yylval.value = (LmcRam)strtol(yytext+1, NULL, 16); return VALUE; } /**< code hexadécimal */
[[:digit:]]+   { yylval.value = (LmcRam)strtol(yytext, NULL, 10); return VALUE; }   /**< nombre décimal */
[[:alpha:]]+   { yylval.string = strdup(yytext); return KEYWORD; }                  /**< mot-clé */
\*?@           { yylval.string = strdup(yytext); return POINTER; }                  /**< adressage */
.              { return *yytext; } /**< caractère non reconnu */

  /** @} */

%%
