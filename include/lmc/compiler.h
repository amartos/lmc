/**
 * @file        compiler.h
 * @version     0.1.0
 * @brief       Interface du compilateur.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup Compiler
 * @{
 */

#ifndef LMC_COMPILER_H_
#define LMC_COMPILER_H_

#include "lmc/computer.h"
#include "lmc/lexer.h"

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @enum LmcCompileHeader
 * @since 0.1.0
 * @brief Index des éléments d'en-tête d'un fichier compilé.
 */
typedef enum LmcCompileHeader {
    LMC_STARTPOS = 0, /**< Adresse de départ du programme. */
    LMC_SIZE,         /**< Taille du programme. */
    LMC_MAXHEADER,    /**< Taille de l'en-tête. */
} LmcCompileHeader;

/**
 * @def LMC_EXT
 * @since 0.1.0
 * @brief Extension pour les fichiers compilés.
 */
#define LMC_EXT ".lmc"

/**
 * @def LMC_BIN
 * @since 0.1.0
 * @brief Nom du fichier compilé par défaut.
 */
#define LMC_BIN "lmc.out"

/**
 * @since 0.1.0
 * @brief Compile un fichier source.
 * @param source Le chemin du fichier source à compiler.
 * @param dest Le chemin du fichier de destination. S'il est @c NULL
 * ou identique à @p source, le chemin du fichier @p source est
 * utilisé, mais avec l'extension #LMC_EXT ajoutée.
 * @return une valeur non-nulle en cas d'échec, ou 0 en cas de
 * réussite.
 */
int lmc_compile(const char* source, const char* dest) __attribute__((nonnull (1)));

/**
 * @since 0.1.0
 * @brief Gère un couple code d'opération/valeur argument.
 * @param header La table de l'en-tête du fichier compilé, de
 * taille #LMC_MAXHEADER.
 * @param code Le code d'opération.
 * @param value La valeur d'argument.
 */
void lmc_compilerCallback(LmcRam* header, LmcRam code, LmcRam value);

#endif // LMC_COMPILER_H_
/** @} */
