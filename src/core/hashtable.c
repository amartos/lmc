/**
 * @file        hashtable.c
 * @version     0.1.0
 * @brief       Fichier source du module hashtable.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup HashtableInternals Structures internes de gestion de la table de hachage
 * @{
 */

#include "lmc/hashtable.h"

// clang-format off

/******************************************************************************
 * @name Création et destruction de la table
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Créé la table de hachage pour LMC_MAXHASH éléments.
 */
static void lmc_hashtable(void) __attribute__((constructor));

/**
 * @since 0.1.0
 * @brief Redéfinition du prototype de la fonction de destruction de
 * la table incluse dans @c search.h pour y adjoindre l'attribut
 * "destructor".
 */
void hdestroy(void) __attribute__((destructor));

// clang-format off

/******************************************************************************
 * @}
 * à}
 * Implémentation
 ******************************************************************************/
// clang-format on

static void lmc_hashtable(void)
{
    // le programme est petit, il ne devrait pas avoir besoin de plus
    // d'une table de hachage.
    if (!hcreate(LMC_MAXHASH))
        err(EXIT_FAILURE, "could not create hash table");
}

void lmc_hashInsertList(LmcKeyword* keywords)
{
    while(keywords->key) lmc_hashInsert(*keywords++);
}

void lmc_hashInsert(LmcKeyword keyword)
{
    // on entre les données dans la table. On passe par une ENTRY
    // intermédiaire puisque la structure nécessite un pointeur et
    // que les données LmcKeyword contiennent une variable.
    ENTRY entry = {
        .key = keyword.key,
        .data = &keyword.data
    };
    if (!hsearch(entry, ENTER))
        err(EXIT_FAILURE, "could not add (%s,%u) in hash table", keyword.key, keyword.data);
}

LmcRam lmc_hashget(char* keyword, bool mandatory)
{
    const char* const errmsg = "unkown token '%s'";

    // on cherche le mot-clé donné dans la table de hachage.
    ENTRY* retval = NULL;
    ENTRY entry = { .key = keyword, };
    if (!(retval = hsearch(entry, FIND))) {
        if (mandatory) err(EXIT_FAILURE, errmsg, keyword);
        // Si mandatory est true, la suite n'est jamais exécutée car
        // err va quitter.
        warn(errmsg, keyword);
        return 0;
    }
    return *((unsigned char*) (retval->data));
}
