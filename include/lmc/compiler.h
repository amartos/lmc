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

#endif // LMC_COMPILER_H_
/** @} */
