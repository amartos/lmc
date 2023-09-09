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

static LmcRamArray bytes = {0};
static size_t times = 0;

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
    for (bytes.current = 0; bytes.current < (times ? times : bytes.max); ) {
        size_t before_index = bytes.current;
        lmc_append(&bytes, 42, 23);
        assert(bytes.current == before_index + 2);
        if (!sccroll_mockGetTrigger()) {
            assert(bytes.values[before_index] == 42);
            assert(bytes.values[before_index+1] == 23);
        }
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

void set_bytes_array(size_t size, size_t t) {
    if (size) {
        bytes.max = size;
        bytes.values = calloc(bytes.max, sizeof(LmcRam));
        if (!bytes.values)
            err(EXIT_FAILURE, "failed alloc for array.values of size %lu", bytes.max);
    }
    times = t;
}

void sccroll_after(void)
{
    free(bytes.values);
    bytes.max = 0;
    times = 0;
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
    set_bytes_array(10, 0);
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
{
    set_bytes_array(10, 0);
    test_append();
}

SCCROLL_TEST(
    append_too_much,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE, },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            // (2a,17) base 16 == (42,23) base 10
            "lexer: memory array size insufficient at (2a,17): Cannot allocate memory",
        }
    }
)
{
    set_bytes_array(10, 15); // 10 size, 15 to insert
    test_append();
}

SCCROLL_TEST(append_error)
{
    set_bytes_array(10, 0);
    sccroll_mockPredefined(test_append);
}
