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
#include "lmc/specs.h"
#include "lmc/lexer.h"
#include "lmc/compiler.h"

#include <search.h>

// clang-format off

/******************************************************************************
 * Préparation
 ******************************************************************************/
// clang-format on

// Variable contenant du code traduit pour comparaison.
LmcMemoryArray bytes = {0};

// Callback vérifiant la traduction de code.
__attribute__((nonnull (1)))
void lmc_lexerCheckerCallback(LmcMemoryArray* array, LmcRam code, LmcRam value)
{
    LmcRam hex[2] = { code, value };
    assert(!memcmp(hex, &array->values[array->current], sizeof(LmcRam)*2));
    array->current += 2;
}

// Callback qui interromp le programme s'il est appelé.
__attribute__((noreturn))
void lmc_lexerFatalCallback(LmcMemoryArray* array, LmcRam a, LmcRam b)
{
    (void) array;
    (void) a;
    (void) b;
    errno = EINTR;
    err(EXIT_FAILURE, "callback called when it should not");
}

// Simulacres et leur gestion pour les tests d'erreurs.
static LmcMockTrigger trigger = { 0 };

void sccroll_before(void) {
    trigger.errnum = 0, trigger.delay = 0;
    if (bytes.values) {
        free(bytes.values);
        bytes.max     = 0;
        bytes.current = 0;
    }
}

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
    LmcLexer lexer = { .callback = lmc_lexerFatalCallback, .desc = "description", };
    // Si le callback est utilisé, une erreur différente est levée.
    assert(yyerror(&lexer, "message") == EXIT_FAILURE);
}

SCCROLL_TEST(analysis)
{
    bytes.values = calloc(DUMMYCODELEN, sizeof(LmcRam));
    if (!bytes.values) err(EXIT_FAILURE, "could not allocate for dummy code dup");
    memcpy(bytes.values, DUMMYCODE, sizeof(LmcRam)*DUMMYCODELEN);
    bytes.max = DUMMYCODELEN;

    // DUMMYCODE contient l'en-tête, mais ce ne sont pas les deux
    // premiers octets rencontrés.
    bytes.values[0] = (char)START;
    bytes.values[1] = 0x8a;
    LmcLexer lexer = {
        .callback = lmc_lexerCheckerCallback,
        .values = bytes,
        .desc = "checker",
    };
    if (!(yyin = fopen(DUMMY LMC_EXT, "r")))
        err(EXIT_FAILURE, DUMMY LMC_EXT);
    assert(!yyparse(&lexer));
}

SCCROLL_TEST(append)
{
    LmcRam values[10] = { 0 };
    LmcMemoryArray array = { .values = values, .max = 10, };
    lmc_append(&array, 42, 23);
    assert(array.values[0] == 42);
    assert(array.values[1] == 23);
    assert(array.max == 10);
    assert(array.current == 2);
}

SCCROLL_TEST(
    append_error,
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "lexer: memory array size insufficient at (2a,17): Cannot allocate memory",
        }
    }
)
{
    LmcRam values[10] = { 0 };
    LmcMemoryArray array = { .values = values, .max = 10, .current = 9, };
    lmc_append(&array, 42, 23);
    assertMsg(false, "Forbidden point reached");
}
