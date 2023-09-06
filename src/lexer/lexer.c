/**
 * @file        lexer.c
 * @version     0.1.0
 * @brief       Lexer for the LMC compiler.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2023 Alexandre Martos <contact@amartos.fr>
 * @license     GPLv3
 *
 * @addtogroup LexerInternals Internal structures of the Lexer.
 * @{
 */

#include "lmc/lexer.h"

// clang-format off

/******************************************************************************
 * @name Mnemonics translation.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Generate the keywords hash table.
 */
static void lmc_hcreate(void) __attribute__((constructor));

// this function is already defined in the standard library, and not
// redefined here. The only change of this prototype is the addition
// of the destructor attribute, which will register it at exit without
// using the atexit function.
void hdestroy(void) __attribute__((destructor));

/**
 * @def LMC_LEXERSWITCH
 * @since 0.1.0
 * @brief Cenerate a switch case for the reverse translation of
 * operation codes.
 * @param opcode An operation bytecode.
 * @param string The corresponding keyword.
 */
#define LMC_LEXERSWITCH(opcode,string) case opcode: return string;

/**
 * @since 0.1.0
 * @brief Convert a string to lowercase.
 * @param string The string to convert.
 * @return A malloc'ed copy of @p string converted to lowercase.
 */
char* lmc_strtolower(const char* restrict string) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 * Implémentation
 ******************************************************************************/
// clang-format on

const char* lmc_keyword(LmcOpCodes opcode)
{
    switch (opcode)
    {
    LMC_PROGLANG(LMC_LEXERSWITCH);
    default: return "";
    }
}

static void lmc_hcreate(void)
{
    int status      = 0;
    ENTRY entry     = {0};

    // The whole program does not need more than one hash table.
    if (!(status = hcreate(LMC_MAXRAM)) && errno)
        err(EXIT_FAILURE, "could not create hash table");
    else if (!status && !errno) return;

    for (size_t i = 0; i < LMC_MAXRAM; ++i)
        if (*(entry.key = (char*)lmc_keyword(i))) {
            // A value is stored, not a pointer. This tactic is given
            // as an example in the hsearch manual.
            entry.data = (void*)i;
            if (!hsearch(entry, ENTER))
                err(EXIT_FAILURE, "could not add '%s' item in hash table", entry.key);
        }
}

LmcOpCodes lmc_opcode(char* keyword)
{
    ENTRY entry = { .key = lmc_strtolower(keyword), };
    ENTRY* retval = NULL;
    LmcOpCodes value = 0;
    if (*keyword && !(retval = hsearch(entry, FIND)))
        err(EXIT_FAILURE, "unknown item '%s'", keyword);
    else if (retval)
        // use size_t for void* size compatibility. The implicit cast
        // handles the truncation. No sign issue is expected as the
        // operations bytecodes are all unsigned.
        value = (size_t)(retval->data);
    free(entry.key);
    return value;
}

char* lmc_strtolower(const char* restrict string)
{
    char* copy = strdup(string);
    char* current = copy;
    while(*current) {
        *current = tolower(*current);
        ++current;
    }
    return copy;
}

void lmc_append(LmcRamArray* array, LmcRam code, LmcRam value)
{
    if (array->current >= array->max-1) {
        errno = ENOMEM;
        err(EXIT_FAILURE,
            "memory array size insufficient at (" LMC_HEXFMT "," LMC_HEXFMT ")",
            LMC_MAXDIGITS, code,
            LMC_MAXDIGITS, value);
    }
    array->values[array->current++] = code;
    array->values[array->current++] = value;
}
