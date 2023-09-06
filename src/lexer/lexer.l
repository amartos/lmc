/**
 * @file        lexer.l
 * @version     0.1.0
 * @brief       Flex version of the LMC lexer module.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2023 Alexandre Martos <contact@amartos.fr>
 * @license     GPLv3
 *
 * @addtogroup LexerInternals
 * @{
 */

%{

#include "lmc/specs.h"
#include "lmc/lexer.h"

/*
 * The bison-generated header file. Only exists at compile-time.
 */
#include "lexer.tab.h"

%}

%option noinput nounput noyywrap
%option case-insensitive

/*
 * For blocks of comments
 */
%x BLOCK

%%

EOF            { yylineno = 0; return EOF; }
\n             { ++yylineno; return EOL; }
[ \t]*         {}                /* ignore spaces */
("#"|";"|"//")[^\n]*? {}         /* ignore short comments */
"/*"           { BEGIN(BLOCK); } /* ignore C-style comments blocks */
<BLOCK>\n      { ++yylineno; }
<BLOCK>.       {}
<BLOCK>"*/"    { BEGIN(INITIAL); }
0x[[:xdigit:]]+ { yylval.value = (LmcRam)strtol(yytext+2, NULL, 16); return VALUE; } /* 0x hex value */
x[[:xdigit:]]+ { yylval.value = (LmcRam)strtol(yytext+1, NULL, 16); return VALUE; } /* x hex value */
[[:digit:]]+   { yylval.value = (LmcRam)strtol(yytext, NULL, 10); return VALUE; }   /* decimal value */
[[:alpha:]]+   { yylval.string = strdup(yytext); return KEYWORD; }                  /* keyword */
\*?@           { yylval.string = strdup(yytext); return POINTER; }                  /* modifiers */
.              { return *yytext; }                                                  /*  unkown chars */

%%
