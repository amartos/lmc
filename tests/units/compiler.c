/**
 * @file      compiler.c
 * @version   0.1.0
 * @brief     LMC compiler unit tests.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 */

#include "tests/common.h"
#include "lmc/computer.h"
#include "lmc/compiler.h"

#include <search.h>

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

// Information about the files to compile.
enum {
    product = 0,
    quotient,
    none,
    dummycode,
};

static struct {
    const char* compiled;
    const char* source;
    char* temp;
    char* content;
    size_t size;
} program_files[] = {
    [product]  = { .compiled = PRODUCT,  .source = PRODUCT LMC_EXT, },
    [quotient] = { .compiled = QUOTIENT, .source = QUOTIENT LMC_EXT, },
    {0},
    [dummycode] = { .temp = LMC_BIN, .source = DUMMY LMC_EXT },
    {0},
};

// Helpers macro for tests definitions
#define FILES(index)                                                    \
    {                                                                   \
        .path = program_files[index].temp,                              \
            .content = {                                                \
                .blob = program_files[index].content,                   \
                .size = program_files[index].size,                      \
            },                                                          \
    }

#define SRCSLIST(macro)                         \
    macro(product),                             \
    macro(quotient)

// Compiled files path template.
static const char* restrict template = "/tmp/%s.lmc.XXXXXX";

// Prepare the tests data. Called as a constructor as sccroll_init()
// is called after the tests definition and registration, causing
// issues for the library handlers.
__attribute__((constructor))
void test_prep(void)
{
    char compname[SCCMAX] = { 0 };
    char filepath[SCCMAX] = { 0 };
    char buffer[SCCMAX] = { 0 };
    FILE* stream = NULL;
    for (int i = 0; program_files[i].compiled; ++i) {
        sprintf(compname, "%s", program_files[i].compiled);
        sprintf(filepath, template, basename(compname));
        if (!(stream = fopen(program_files[i].compiled, "r"))
            || (program_files[i].size = fread(buffer , sizeof(char), SCCMAX, stream)) < 1
            || fclose(stream)
            || mkstemp(filepath) < 0)
            err(EXIT_FAILURE, "%s", program_files[i].compiled);
        program_files[i].content = blobdup(buffer, program_files[i].size);
        program_files[i].temp = strdup(filepath);
        memset(filepath, 0, strlen(filepath));
        memset(compname, 0, sizeof(compname));
    }

    program_files[dummycode].content = DUMMYCODE;
    program_files[dummycode].size = sizeof(char)*DUMMYCODELEN;
}

void lmc_compile_errtests(void) { lmc_compile(PRODUCT LMC_EXT, NULL); }

void sccroll_clean(void)
{
    for (int i = 0; program_files[i].compiled; ++i) {
        remove(program_files[i].temp);
        free(program_files[i].content);
        free(program_files[i].temp);
    }
}

// clang-format off

/******************************************************************************
 * Tests
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(compilation, .files = { SRCSLIST(FILES), })
{
    for (int i = 0; program_files[i].compiled; ++i)
        assert(!lmc_compile(program_files[i].source, program_files[i].temp));
}

SCCROLL_TEST(
    no_output_file,
    .files = { FILES(dummycode), FILES(dummycode), },
    .std = {
        [STDOUT_FILENO] = { .content.blob =
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'",
        }
    }
)
{
    assert(!lmc_compile(DUMMY LMC_EXT, NULL));
    assert(!lmc_compile(DUMMY LMC_EXT, ""));
}

SCCROLL_TEST(
    malformed_program,
    .std = {
        [STDERR_FILENO] = { .content.blob = MALFORMED LMC_EXT ": syntax error at line 1: ','" },
    }
)
{ assert(lmc_compile(MALFORMED LMC_EXT, NULL)); }

SCCROLL_TEST(
    errors_handling,
    .std = {
        [STDOUT_FILENO] = { .content.blob =
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
            "LMC: compiled to '" LMC_BIN "'\n"
        },
    },
)
{ sccroll_mockPredefined(lmc_compile_errtests); }
