/**
 * @file        lexer.h
 * @version     0.1.0
 * @brief       The LMC lexer interface.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license     GPLv3
 *
 * @addtogroup Lexer
 * @{
 */

#ifndef LMC_LEXER_H_
#define LMC_LEXER_H_

#include "lmc/specs.h"

#include <err.h>
#include <errno.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct LmcRamArray
 * @since 0.1.0
 * @brief Store the current translation of given instructions.
 */
typedef struct LmcRamArray {
    LmcRam* values; /**< Instructions byte values array. */
    size_t max;     /**< Max size of LmcRamArray::values. */
    size_t current; /**< Last free index of LmcRamArray::values. */
} LmcRamArray;

/**
 * @typedef LmcLexerCallback
 * @since 0.1.0
 * @brief Lexer callback functions prototype.
 *
 * These function are used to handle the storage of each instruction
 * couple (@p code, @p value) in the translation array.
 *
 * @note The lmc_append() is an example of such function.
 *
 * @param array The translation table.
 * @param code,value The byte code for the instruction and its
 * argument.
 */
typedef void (*LmcLexerCallback)(LmcRamArray* array, LmcRam code, LmcRam arg);

/**
 * @struct LmcLexer
 * @since 0.1.0
 * @brief Structure storing minimal necessary information for the
 * translation.
 */
typedef struct LmcLexer {
    const char* desc;          /**< A description of the translation. */
    LmcLexerCallback callback; /**< Callback function storing
                                * translated values in the translation
                                * array. */
    LmcRamArray values;        /**< The translation array. */
} LmcLexer;

/**
 * @name Externals
 * The following variables and functions are necessary for the lexer
 * operations.
 * @{
 */
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
int yylex(void);
int yyerror(LmcLexer* lexer, const char* restrict msg) __attribute__((nonnull));
int yyparse(LmcLexer* lexer);
/** }@ */


/**
 * @since 0.1.0
 * @brief Translate the given keyword to a LMC byte instruction code.
 * @param keyword The keyword.
 * @return The corresponding byet instruction, otherwise @c 0.
 */
LmcOpCodes lmc_opcode(char* keyword) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Reverse translate a byte code operation.
 * @param opcode The operation bytecode.
 * @return The corresponding keyword, otherwise an empty string.
 */
const char* lmc_keyword(LmcOpCodes opcode) __attribute__((returns_nonnull));

/**
 * @since 0.1.0
 * @brief Add  the instruction couple (@p code, @p value) in the
 * translation table and increments LmcMemoryArray::current.
 * @param array The translation table.
 * @param code,value The instruction and argument couple bytes
 * code and value.
 */
void lmc_append(LmcRamArray* array, LmcRam code, LmcRam value)
    __attribute__((nonnull (1)));

#endif // LMC_LEXER_H_
/** @} */
