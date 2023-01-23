/**
 * @file        hashtable.h
 * @version     0.1.0
 * @brief       Interface du module de gestion de la table de hachage.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation Cf. le fichier lmc.h
 *
 * @addtogroup Hashtable Gestion de la table de hachage.
 * @{
 */

#ifndef LMC_HASHTABLE_H_
#define LMC_HASHTABLE_H_

#include "lmc/computer.h"

#include <err.h>
#include <errno.h>
#include <search.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @def LMC_MAXHASH
 * @since 0.1.0
 * @brief Nombre maximum d'éléments de la table de hachage.
 *
 * Correspond à la valeur @c nel passée à @c hcreate.
 */
#define LMC_MAXHASH BUFSIZ

/**
 * @struct LmcKeyword
 * @since 0.1.0
 * @brief Structure de correspondances mot-clé <> entier.
 */
typedef struct LmcKeyword {
    char* key;   /**< Mot clé. */
    LmcRam data; /**< Entier correspondant à LmcKeyword::key. */
} LmcKeyword;

/**
 * @since 0.1.0
 * @brief Insère un couple (mot-clé,entier) dans la table de hachage.
 * @param keyword La structure LmcKeyword contenant le couple.
 */
void lmc_hashInsert(LmcKeyword keyword);

/**
 * @since 0.1.0
 * @brief Insère une liste de couples (mot-clé,entier) dans la table
 * de hachage.
 * @param keywords Une table de maximum LMC_MAXHASH LmcKeyword
 * contenant les couples et dont la dernière LmcKeyword::key est
 * @c NULL.
 */
void lmc_hashInsertList(LmcKeyword* keywords) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Donne le code correspondant au mot.
 * @param keyword Un mot clé.
 * @param mandatory Un drapeau indiquant si la fonction doit quitter
 * (@c EXIT_FAILURE) ou non si aucun mot ne correspond.
 * @return le code correspondant au mot, ou 0 si aucun ne correspond.
 */
LmcRam lmc_hashget(char* keyword, bool mandatory);

#endif // LMC_HASHTABLE_H_
/** @} */
