/**
 * @file        lexer.y
 * @version     0.1.0
 * @brief       The Bison version of the LMC lexer module.
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

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

%}

%union {
    char* string;         // any string
    LmcOpCodes operation; // bytecode instructions
    LmcRam value;         // arguments
};

%left   <string>        KEYWORD
%token  <string>        POINTER
%token  <value>         VALUE
%token                  EOL
%type   <operation>     keyword
%type   <value>         arg

%parse-param { LmcLexer* lexer }

%start line

%%

// Parse multiple lines, not only the first.
line: | line expr;

expr:
        keyword arg { lexer->callback(&lexer->values, $1, $2); }
|               EOL {}
;

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
 * Flex interface implementations.
 ******************************************************************************/
// clang-format on

int yyerror(LmcLexer* lexer, const char* restrict msg)
{
    fprintf(stderr, "%s: %s at line %i: '%s'\n", lexer->desc, msg, yylineno, yytext);
    return EXIT_FAILURE;
}
