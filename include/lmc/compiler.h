/**
 * @file        compiler.h
 * @version     0.1.0
 * @brief       Compiler interface.
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   2023 Alexandre Martos <contact@amartos.fr>
 * @license     GPLv3
 *
 * @addtogroup Compiler
 * @{
 */

#ifndef LMC_COMPILER_H_
#define LMC_COMPILER_H_

#include "lmc/specs.h"
#include "lmc/lexer.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @enum LmcCompileHeader
 * @since 0.1.0
 * @brief Programs header structure indexes.
 */
typedef enum LmcCompileHeader {
    LMC_STARTPOS = 0, /**< Start address. */
    LMC_SIZE,         /**< Program size. */
    LMC_MAXHEADER,    /**< Header max size. */
} LmcCompileHeader;

/**
 * @def LMC_EXT
 * @since 0.1.0
 * @brief Default extension of programs source files.
 */
#define LMC_EXT ".lmc"

/**
 * @def LMC_BIN
 * @since 0.1.0
 * @brief Default compiled file name.
 */
#define LMC_BIN "lmc.out"

/**
 * @since 0.1.0
 * @brief Compile a source file.
 * @param source The source file path.
 * @param dest The compiled file path. Defaults to #LMC_BIN if @c NULL
 * or identical to @p source.
 * @return non-null in case of errors, otherwise @c 0.
 */
int lmc_compile(const char* source, const char* dest) __attribute__((nonnull (1)));

#endif // LMC_COMPILER_H_
/** @} */
