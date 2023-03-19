/**
 * @file        lexer.c
 * @version     0.1.0
 * @brief       Module de traduction de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup LexerInternals Structure internes du traducteur.
 * @{
 */

#include "lmc/lexer.h"

// clang-format off

/******************************************************************************
 * @name Traduction des mnémoniques en codes d'opération.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Génère la table de hachage des mots clés.
 */
static void lmc_hcreate(void) __attribute__((constructor));

// on ajoute l'attribut destructor à une fonction de la librairie
// standard, pour l'inscrire automatiquement à l'exit.
void hdestroy(void) __attribute__((destructor));

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
    case VAR:   return "@";
    case INDIR: return "*@";
    case ADD:   return "add";
    case SUB:   return "sub";
    case NAND:  return "nand";
    case LOAD:  return "load";
    case STORE: return "store";
    case IN:    return "in";
    case OUT:   return "out";
    case JUMP:  return "jump";
    case BRN:   return "brn";
    case BRZ:   return "brz";
    case HLT:   return "stop";
    case START: return "start";
    default:    return "";
    }
}

static void lmc_hcreate(void)
{
    int status      = 0;
    ENTRY entry     = {0};

    // on utilise hcreate ici car le programme n'a pas besoin de plus
    // d'une table de hachage.
    if (!(status = hcreate(LMC_MAXRAM)) && errno)
        err(EXIT_FAILURE, "could not create hash table");
    else if (!status && !errno) return; // Une table de hachage existe déjà.

    // on entre les données dans la table.
    for (size_t i = 0; i < LMC_MAXRAM; ++i)
        if (*(entry.key = (char*)lmc_keyword(i))) {
            // on stocke une valeur, non un pointeur. Cette tactique
            // est employée dans l'exemple donné dans le manuel de hsearch.
            entry.data = (void*)i;
            if (!hsearch(entry, ENTER))
                err(EXIT_FAILURE, "could not add '%s' item in hash table", entry.key);
        }
}

LmcOpCodes lmc_opcode(char* keyword)
{
    // on cherche le mot-clé donné dans la table de hachage. Cependant,
    // si le mot-clé n'y est pas, ou que la chaîne est vide, on
    // renvoie 0.
    ENTRY entry = { .key = keyword, };
    ENTRY* retval = NULL;
    LmcOpCodes value = 0;
    if (*keyword && !(retval = hsearch(entry, FIND)))
        err(EXIT_FAILURE, "unknown item '%s'", keyword);
    else if (retval)
        // size_t pour la taille du type, compatible avec void*
        value = (size_t)(retval->data);
    return value;
}
