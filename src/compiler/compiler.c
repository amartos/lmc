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
 * @brief Ajoute un couple code d'opération/valeur argument à la table
 * de traduction.
 * @param lexer Les informations de traduction.
 * @param array La table de traduction.
 */
static void lmc_compilerCallback(LmcMemoryArray* array, LmcRam code, LmcRam value);

/**
 * @since 0.1.0
 * @brief Écrit les codes de la table de traduction dans le fichier de
 * destination.
 * @param lexer Les informations de traduction.
 * @param path Le chemin du fichier de destination.
 */
static void lmc_compilerWrite(LmcLexer* lexer, const char* restrict path)
    __attribute__((nonnull));

/** @} */

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

int lmc_compile(const char* source, const char* dest)
{
    int status = 0;

    // Si la destination n'est pas précisée, on utilise la valeur par
    // défaut.
    const char* output = dest && *dest ? dest : LMC_BIN;

    // On initialise startpos à LMC_MAXROM+1 car elle équivaut à la
    // position 0 du programme dans la mémoire (juste après
    // l'argument de l'instruction JUMP du bootstrap). Si aucune
    // position de départ n'est donnée, ce sera celle par défaut.
    LmcRam array[LMC_MAXRAM] = { [LMC_STARTPOS] = LMC_MAXROM + 1 };

    // L'index courant est situé juste après l'en-tête du fichier car
    // on réserve ces emplacements.
    LmcLexer lexer = {
        .values   = { .values = array, .max = LMC_MAXRAM, .current = LMC_MAXHEADER, },
        .callback = lmc_compilerCallback,
        .desc     = source,
    };
    // On analyse le fichier source, et on inscrit les données
    // compilées dans le fichier de destination. On ferme ensuite la
    // source une fois l'analyse terminée.
    if (!(yyin = fopen(source, "r"))) err(EXIT_FAILURE, "%s", source);
    status = yyparse(&lexer);
    fclose(yyin);

    // On écrit dans le fichier de destination qu'on ferme ensuite. On
    // indique aussi le chemin du fichier de destination s'il est
    // différent de celui précisé (ou que dest est NULL, en cas).
    lmc_compilerWrite(&lexer, output);
    if (!status && output != dest) printf("LMC: compiled to '%s'\n", output);

    return status;
}

static void lmc_compilerCallback(LmcMemoryArray* array, LmcRam code, LmcRam value)
{
    switch(code)
    {
    case START:       array->values[LMC_STARTPOS] += value; break;
    case START | VAR: array->values[LMC_STARTPOS]  = value; break;
    default:
        lmc_append(array, code, value);
        array->values[LMC_SIZE] = array->current - LMC_MAXHEADER;
        break;
    }
}

static void lmc_compilerWrite(LmcLexer* lexer, const char* restrict path)
{
    FILE* output = NULL;
    size_t towrite = lexer->values.current;
    if (!(output = fopen(path, "w"))
        || fwrite(lexer->values.values, sizeof(LmcRam), towrite, output) != towrite)
        err(EXIT_FAILURE, "%s", path);
    fclose(output);
}
