/**
 * @file      lexer.c
 * @version   0.1.0
 * @brief     Lexer unit tests.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 */

#include "tests/common.h"
#include "lmc/specs.h"
#include "lmc/lexer.h"
#include "lmc/compiler.h"

#include <search.h>

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

LmcRamArray bytes = {0};

void test_opcode(void)
{
    int i;
    const char* keyword;
    for (i = 0, keyword = lmc_keyword(i); i < LMC_MAXRAM; ++i, keyword = lmc_keyword(i))
        if (!sccroll_mockGetTrigger())
            assert((!*keyword) || (lmc_opcode((char*)keyword) == (LmcOpCodes)i));
    if (!sccroll_mockGetTrigger()) assert(lmc_opcode("") == 0);
}

void test_append(void)
{
    LmcRam values[10] = { 0 };
    LmcRamArray array = { .values = values, .max = 10, };
    lmc_append(&array, 42, 23);
    if (!sccroll_mockGetTrigger())
    {
        assert(array.values[0] == 42);
        assert(array.values[1] == 23);
        assert(array.max == 10);
        assert(array.current == 2);
    }
}

// Callback to check the translation.
__attribute__((nonnull (1)))
void lmc_lexerCheckerCallback(LmcRamArray* array, LmcRam code, LmcRam value)
{
    LmcRam hex[2] = { code, value };
    assert(!memcmp(hex, &array->values[array->current], sizeof(LmcRam)*2));
    array->current += 2;
}

// Callback raising a fatal error if called.
__attribute__((noreturn))
void lmc_lexerFatalCallback(LmcRamArray* array, LmcRam a, LmcRam b)
{
    (void) array;
    (void) a;
    (void) b;
    errno = EINTR;
    err(EXIT_FAILURE, "callback called when it should not");
}

void sccroll_before(void) {
    if (bytes.values) {
        free(bytes.values);
        bytes.max     = 0;
        bytes.current = 0;
    }
}

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(translation)
{ test_opcode(); }

SCCROLL_TEST(translation_error_handling)
{ sccroll_mockPredefined(test_opcode); }

SCCROLL_TEST(
    notaword,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE, },
    .std = {
        [STDERR_FILENO] = { .content.blob = "lexer: unknown item 'foobar': No such process", }
    }
)
{ lmc_opcode("foobar"); }

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
    // If the callback is used, a different error is raised.
    assert(yyerror(&lexer, "message") == EXIT_FAILURE);
}

SCCROLL_TEST(analysis)
{
    bytes.values = calloc(DUMMYCODELEN, sizeof(LmcRam));
    if (!bytes.values) err(EXIT_FAILURE, "could not allocate for dummy code dup");
    memcpy(bytes.values, DUMMYCODE, sizeof(LmcRam)*DUMMYCODELEN);
    bytes.max = DUMMYCODELEN;

    // DUMMYCODE contains the header, but not at the first two bytes.
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
{ test_append(); }

SCCROLL_TEST(append_error)
{ sccroll_mockPredefined(test_append); }
