/**
 * @file      compiler.c
 * @version   0.1.0
 * @brief     Tests unitaires du compilateur.
 * @year      2022
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright GNU General Public License v3
 * @compilation
 * gcc -xc -Wall -std=gnu99 -lsccroll -lgcov \
 *     tests/units/computer.c src/core/computer.c \
 *     -o build/bin/tests/units/computer
 */

#include "tests/common.h"
#include "lmc/computer.h"
#include "lmc/lexer.h"
#include "lmc/compiler.h"

#include <search.h>

// clang-format off

/******************************************************************************
 * Préparation
 ******************************************************************************/
// clang-format on

// Variable contenant du code traduit pour comparaison.
char* bytes = NULL;

// Callback vérifiant la traduction de code.
void lmc_lexerCheckerCallback(LmcRam code, LmcRam value)
{
    char hex[3] = { code, value };
    assert(!memcmp(hex, bytes, sizeof(char)*2));
    bytes += 2;
}

// Callback qui interromp le programme s'il est appelé.
__attribute__((noreturn))
void lmc_lexerFatalCallback(LmcRam a, LmcRam b)
{
    (void) a;
    (void) b;
    errno = EINTR;
    err(EXIT_FAILURE, "callback called when it should not");
}

// Simulacres et leur gestion pour les tests d'erreurs.
static LmcMockTrigger trigger = { 0 };

void sccroll_before(void) { trigger.errnum = 0, trigger.delay = 0; }

// On ne créé pas de simulacre de hcreate, ni de hsearch qui teste
// lors de l'entrée d'un item, car ces actions sont faites avant
// l'appel du main.
SCCROLL_MOCK(ENTRY*, hsearch, ENTRY item, ACTION action)
{ return lmc_mockErr(hsearch, ERRHSEARCH, NULL, item, action); }

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(translation)
{
    int i;
    const char* keyword;
    for (i = 0, keyword = lmc_keyword(i); i < LMC_MAXRAM; ++i, keyword = lmc_keyword(i))
        assert((!*keyword) || (lmc_opcode((char*)keyword) == (LmcOpCodes)i));
    assert(lmc_opcode("") == 0);
}

SCCROLL_TEST(
    notaword,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE, },
    .std = {
        [STDERR_FILENO] = { .content.blob = "lexer: unknown item 'foobar': Success", }
    }
)
{ trigger.errnum = ERRHSEARCH, lmc_opcode("foobar"); }

SCCROLL_TEST(
    hsearch_find_errors_handling,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE, },
    .std = {
        [STDERR_FILENO] = { .content.blob = "lexer: unknown item 'in': Success", }
    }
)
{ trigger.errnum = ERRHSEARCH, lmc_opcode("in"); }

SCCROLL_TEST(
    yyerror_output,
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "description: message at line 42: 'foobarbiz'",
        }
    }
)
{
    yylineno = 42;
    yytext = "foobarbiz";
    // Si le callback est utilisé, une erreur différente est levée.
    assert(yyerror(lmc_lexerFatalCallback, "description", "message") == EXIT_FAILURE);
}

SCCROLL_TEST(analysis)
{
    // on utilise une autre variable car bytes va changer (et on ne
    // pourra plus le libérer).
    char* code = strdup(DUMMYCODE);
    bytes = code;
    // DUMMYCODE contient l'en-tête, mais ce ne sont pas les deux
    // premiers octets rencontrés.
    bytes[0] = (char)START;
    bytes[1] = 0x8a;
    if (!(yyin = fopen(DUMMY LMC_EXT, "r")))
        err(EXIT_FAILURE, DUMMY LMC_EXT);
    assert(!yyparse(lmc_lexerCheckerCallback, "checker"));
    free(code);
}
