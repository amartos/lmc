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
 * Gestion des données analysées.
 ******************************************************************************/
// clang-format on

/**
 * @name Traduction des mnémoniques en codes d'opération.
 * @{
 */

/**
 * @since 0.1.0
 * @brief Génère la table de hachage des mots clés.
 */
static void lmc_hcreate(void) __attribute__((constructor));

// on ajoute l'attribut destructor à une fonction de la librairie
// standard, pour l'inscrire automatiquement à l'exit.
void hdestroy(void) __attribute__((destructor));

/**
 * @var lmc_compiler_mnemonics
 * @since 0.1.0
 * @brief Table des correspondances mot-clé <> code d'opération.
 */
static const struct {
    char* key;
    LmcOpCodes data;
} lmc_compiler_mnemonics[LMC_MAXRAM] = {
    {    "@",   VAR},
    {   "*@", INDIR},
    {  "add",   ADD},
    {  "sub",   SUB},
    { "nand",  NAND},
    { "load",  LOAD},
    {"store", STORE},
    {   "in",    IN},
    {  "out",   OUT},
    { "jump",  JUMP},
    {  "brn",   BRN},
    {  "brz",   BRZ},
    { "stop",   HLT},
    {"start", START},
};

/** @} */

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

static void lmc_hcreate(void)
{
    int status;
    ENTRY entries[LMC_MAXRAM];

    // on utilise hcreate ici car le programme n'a pas besoin de plus
    // d'une table de hachage.
    if (!(status = hcreate(LMC_MAXRAM)) && errno)
        err(EXIT_FAILURE, "could not create hash table");
    else if (!status && !errno) return; // Une table de hachage existe déjà.

    // on entre les données dans la table.
    // On utilise une variable intermédiaire ici car ENTRY nécessite
    // un pointeur, or les codes d'opérations sont stockées dans une
    // énumération.
    for (int i = 0; i < LMC_MAXRAM; ++i)
        if (lmc_compiler_mnemonics[i].key) {
            entries[i].key = (char*) lmc_compiler_mnemonics[i].key;
            entries[i].data = (void*) &(lmc_compiler_mnemonics[i].data);
            if (!hsearch(entries[i], ENTER))
                err(EXIT_FAILURE, "could not add '%s' item in hash table", lmc_compiler_mnemonics[i].key);
        }
}

LmcOpCodes lmc_stringToOpCode(char* string)
{
    // on cherche le mot-clé donné dans la table de hachage. Cependant,
    // si le mot-clé n'y est pas, ou que la chaîne est vide, on
    // renvoie 0.
    ENTRY entry = { .key = string, };
    ENTRY* retval = NULL;
    LmcOpCodes value = 0;
    if (*string && !(retval = hsearch(entry, FIND)))
        err(EXIT_FAILURE, "unknown item '%s'", string);
    else if (retval)
        value = *((LmcOpCodes*) (retval->data));
    free(string); // la chaîne a été strdup()
    return value;
}

/** @} */
