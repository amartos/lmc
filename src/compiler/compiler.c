/**
 * @file        compiler.c
 * @version     0.1.0
 * @brief       Module de compilation de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup CompilerInternals
 * @{
 */

#include "lmc/compiler.h"

/**
 * @name Gestion du fichier compilé.
 * @{
 */

/**
 * @since 0.1.0
 * @brief Ouvre les fichiers sources et destination.
 * @param source Le chemin du fichier source.
 * @param dest Le chemin du fichier de destination; s'il est identique
 * à @p source ou qu'il est @c NULL, le chemin du fichier source sera
 * utilisé, et une extension ".lmc" sera ajoutée.
 * @return Le descripteur du fichier de destination (celui du fichier
 * source est stocké dans yyin).
 */
static FILE* lmc_compilerOpenFiles(const char* restrict source, const char* dest)
    __attribute__((nonnull (1)));

/**
 * @since 0.1.0
 * @brief Écrit un couple code d'opération/valeur argument dans le
 * fichier de destination.
 * @param code Le code d'opération.
 * @param value La valeur d'argument.
 */
static void lmc_compilerWrite(LmcRam code, LmcRam value);

/**
 * @var src
 * @since 0.1.0
 * @brief Chemin du fichier source.
 */
static const char* src = NULL;

/**
 * @var realdest
 * @since 0.1.0
 * @brief Chemin du fichier de destination s'il est différent de celui
 * donné.
 */
static const char* realdest = NULL;

/**
 * @var output
 * @since 0.1.0
 * @brief Flux du fichier compilé.
 */
static FILE* output = NULL;

static LmcRam startpos = 0;

static LmcRam size = 0;
/** @} */

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

int lmc_compile(const char* source, const char* dest)
{
    int status = 0;

    // On prépare les descripteur de fichiers des sources et
    // destination et on réserve l'espace pour les codes de départ et
    // taille du programme dans le fichier de destination. Cela évite
    // à l'utilisateur d'avoir à calculer la taille, et s'il ne donne
    // pas de directive "start", on met le départ à la valeur par
    // défaut.
    output = lmc_compilerOpenFiles(source, dest);
    lmc_compilerWrite(0, 0);
    // On initialise startpos à LMC_MAXROM+1 car elle équivaut à la
    // position 0 du programme dans la mémoire (juste après
    // l'argument de l'instruction JUMP du bootstrap). Si aucune
    // position de départ n'est donnée, ce sera celle par défaut.
    startpos = LMC_MAXROM + 1;
    // compilerWrite incrémente la taille, or on la veut à 0.
    size = 0;

    // On analyse le fichier source, et on inscrit les données
    // compilées dans le fichier de destination. On ferme ensuite la
    // source une fois l'analyse terminée.
    status = yyparse(lmc_compilerCallback, source);
    fclose(yyin);

    // On modifie les espaces réservés pour les informations sur le
    // programme avec les-dites informations. On ferme ensuite la
    // destination, puisqu'il n'y a plus rien à y écrire. On indique
    // aussi le chemin du fichier de destination s'il est différent de
    // celui précisé (ou que dest est NULL, en cas).
    rewind(output);
    lmc_compilerWrite(startpos, size);
    fclose(output);
    if (!status && realdest != dest) printf("LMC: compiled to '%s'\n", realdest);

    return status;
}

static FILE* lmc_compilerOpenFiles(const char* restrict source, const char* dest)
{
    FILE* output;

    // On modifie la source de stdin au fichier donné.
    src = source;
    if (!(yyin = fopen(source, "r")))
        err(EXIT_FAILURE, "%s", source);

    realdest = dest && *dest ? dest : LMC_BIN;
    if (!(output = fopen(realdest, "w")))
        err(EXIT_FAILURE, "%s", realdest);
    return output;
}

void lmc_compilerCallback(LmcRam code, LmcRam value)
{
    switch(code)
    {
    case START:       startpos += value; break;
    case START | VAR: startpos  = value; break;
    default: lmc_compilerWrite(code, value); break;
    }
}

static void lmc_compilerWrite(LmcRam code, LmcRam value)
{
    LmcRam hexcodes[2] = { code, value };
    if (fwrite(&hexcodes, sizeof(LmcRam), 2, output) != 2)
        err(EXIT_FAILURE, "could not write codes (" LMC_HEXFMT "," LMC_HEXFMT ") in %s",
            LMC_MAXDIGITS, code,
            LMC_MAXDIGITS, value,
            realdest
        );
    size += 2;
}
