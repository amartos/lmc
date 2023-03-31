/**
 * @file      lmc.h
 * @version   0.1.0
 * @brief     Fichier en-tête principal de l'ordinateur en papier
 * ("Little man computer")
 * @year      2023
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright GNU General Public License v3
 * @compilation
 * @code{.sh}
 * bison -d -o compiler.tab.c src/compiler/compiler.y
 * flex -o compiler.yy.c src/compiler/compiler.l
 * gcc -xc -Wall -Wextra -std=gnu99 -Iinclude \
 *     src/core/computer.c \
 *     compiler.tab.c compiler.yy.c \
 *     src/lmc.c -o build/bin/lmc
 * @endcode
 *
 * @addtogroup LMC L'ordinateur en papier
 * @{
 */

#ifndef LMC_H_
#define LMC_H_

#include "lmc/specs.h"
#include "lmc/computer.h" // Le module principal de l'ordinateur en papier.
#include "lmc/compiler.h" // Le module de compilation des programmes.

#include <argp.h>   // pour la gestion des arguments
#include <stdio.h>
#include <stdlib.h>

#endif // LMC_H_
/** @} */
