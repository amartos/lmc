/**
 * @file        compiler.c
 * @version     0.1.0
 * @brief       LMC compiler module.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2023 Alexandre Martos <contact@amartos.fr>
 * @license     GPLv3
 *
 * @addtogroup CompilerInternals
 * @{
 */

#include "lmc/compiler.h"

/**
 * @since 0.1.0
 * @brief Add a (instruction, argument) bytecodes couple to the
 * translation table.
 * @param array The translation table.
 * @param code,value The (instruction, argument) couple.
 */
static void lmc_compilerCallback(LmcRamArray* array, LmcRam code, LmcRam value);

/**
 * @since 0.1.0
 * @brief Write the translated program bytecode into the destination
 * file.
 * @param lexer The translation information.
 * @param path The destination file path.
 */
static void lmc_compilerWrite(LmcLexer* lexer, const char* restrict path)
    __attribute__((nonnull));


// clang-format off

/******************************************************************************
 * @}
 * Implementation
 ******************************************************************************/
// clang-format on

int lmc_compile(const char* source, const char* dest)
{
    int status = 0;
    const char* output = dest && *dest ? dest : LMC_BIN;

    // Init the start position at LMC_MAXROM+1 as it is the first
    // writable memory slot after the last bootstrap JUMP instruction
    // argument slot (thus in RAM, but loosely considered part of
    // ROM). This is a default value that can be changed in the source
    // of the compiled program.
    LmcRam array[LMC_MAXRAM] = { [LMC_STARTPOS] = LMC_MAXROM + 1 };

    LmcLexer lexer = {
        // .current value reserves space for the header.
        .values   = { .values = array, .max = LMC_MAXRAM, .current = LMC_MAXHEADER, },
        .callback = lmc_compilerCallback,
        .desc     = source,
    };

    if (!(yyin = fopen(source, "r"))) err(EXIT_FAILURE, "%s", source);
    status = yyparse(&lexer);
    fclose(yyin);

    if (!status) {
        lmc_compilerWrite(&lexer, output);
        // Print the final destination for clarity.
        if (output != dest) printf("LMC: compiled to '%s'\n", output);
    }

    return status;
}

static void lmc_compilerCallback(LmcRamArray* array, LmcRam code, LmcRam value)
{
    switch(code)
    {
    case START:       array->values[LMC_STARTPOS] += value; break;
    case START | VAR: array->values[LMC_STARTPOS]  = value; break;
    default:
        lmc_append(array, code, value);
        array->values[LMC_SIZE] = array->current - LMC_MAXHEADER;
        break;
    }
}

static void lmc_compilerWrite(LmcLexer* lexer, const char* restrict path)
{
    FILE* output = NULL;
    size_t towrite = lexer->values.current;
    if (!(output = fopen(path, "w"))
        || fwrite(lexer->values.values, sizeof(LmcRam), towrite, output) != towrite)
        err(EXIT_FAILURE, "%s", path);
    fclose(output);
}
