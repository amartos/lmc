/**
 * @file        compiler.y
 * @version     0.1.0
 * @brief       Module bison du compilateur de l'ordinateur en papier.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation
 * @code{.sh}
 * bison -d -o compiler.tab.c src/compiler/compiler.y
 * @endcode
 *
 * @addtogroup CompilerInternals
 * @{
 */

%{

// clang-format off

/******************************************************************************
 * Fichier en-tête généré par bison.
 ******************************************************************************/
// clang-format on

#include "lmc/computer.h"
#include "lmc/hashtable.h"
#include "lmc/lexer.h"

// Fichier en-tête servant pour l'interface avec le reste du
// programme.
#include "lmc/compiler.h"

#include <err.h>
#include <errno.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @since 0.1.0
 * @brief Écrite un couple code d'opération/valeur argument dans le
 * fichier de destination.
 * @param output Le descripteur du fichier de destination.
 * @param code Le code d'opération LmcOpCodes (casté en LmcRam).
 * @param value La valeur d'argument.
 */
void lmc_compilerWrite(FILE* output, LmcRam code, LmcRam value);

%}

// clang-format off

/******************************************************************************
 * Définition des types de données analysée.
 ******************************************************************************/
// clang-format on

// Les type de données rencontrées lors de l'analyse.
%union {
    char* string;
    LmcOpCodes operation;
    LmcRam value;
};

// Définition des divers items possibles et de leur type. Les
// mots-clés, représentés par KEYWORD, sont forcément des l-values, et
// donc positionnés à gauche ("left"); leurs arguments, représentés
// par VALUE, sont des r-values, donc forcément à droite
// ("right"). START est la commande indiquant l'adresse de départ
// relative (à la dernière adresse de mémoire morte) du programme,
// et STARTABS est sa contrepartie absolue (la valeur donnée est
// l'adresse réelle). Ce sont deux items spéciaux, comme des
// mots-clés, mais leur traitement sera légèrement différent.

%left   <string>        KEYWORD
%token  <string>        POINTER
%token  <value>         VALUE
%type   <operation>     keyword
%left   <string>        START    "start"
%left   <string>        STARTABS "startabs"

// Paramètres supplémentaires pour la fonction d'analyse yyparser; ils
// permettent de stocker des informations à utiliser plus tard
// (startpos, size), ou de passer des paramètres aux fonctions
// internes.
%parse-param { LmcRam* startpos } { LmcRam* size } { FILE* output } { const char* restrict source }

// Le point de départ de l'analyse.
%start line

%%

// clang-format off

/******************************************************************************
 * États de l'automate et liaisons.
 ******************************************************************************/
// clang-format on

// Permet d'analyser plusieurs lignes d'un fichier (sinon l'automate
// s'arrête après la première).
line: | line expr;

// Une ligne contenant une commande.
expr:
       START VALUE { *startpos = LMC_MAXROM + 1 + $2; } // +1 pour l'argument du jump du bootstrap
|   STARTABS VALUE { *startpos = $2; }
|    keyword VALUE {
        // LmcOpCodes est cast en LmcRam. Cela ne devrait pas poser de
        // problèmes puisque les valeurs des codes d'opérations sont
        // compatibles.
        lmc_compilerWrite(output, $1, $2);
        *size += 2;
    }
;

// conversion des mots-clés; elle est traitée à part du fait de la
// possibilité de l'adressage, qui doit être combiné à la valeur du
// code d'opération.
keyword:
            KEYWORD { $$ = lmc_hashget($1, true), free($1); }
|   KEYWORD POINTER {
        $$ = lmc_hashget($1, true) | lmc_hashget($2, true);
        free($1), free($2);
   }
|             VALUE { $$ = $1; }
;

%%

// clang-format off

/******************************************************************************
 * Gestion de la compilation.
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Effectue le nettoyage du module.
 */
static void lmc_compilerCleanup(void);

/**
 * @var lmc_compiler_mnemonics
 * @since 0.1.0
 * @brief Table des correspondances mot-clé <> code d'opération.
 */
static LmcKeyword lmc_compiler_mnemonics[LMC_MAXRAM] = {
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
    {   NULL,     0},
};

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
 * @var realdest
 * @since 0.1.0
 * @brief Chemin du fichier de destination s'il est différent de celui
 * donné.
 */
static const char* realdest = NULL;
/** @} */

// clang-format off

/******************************************************************************
 * Implémentation
 ******************************************************************************/
// clang-format on

int lmc_compile(const char* source, const char* dest)
{
    int status = 0;
    FILE* output = NULL;

    // On initialise startpos à LMC_MAXROM+1 car elle équivaut à la
    // position 0 du programme dans la mémoire (juste après
    // l'argument de l'instruction JUMP du bootstrap). Si aucune
    // position de départ n'est donnée, ce sera celle par défaut.
    LmcRam startpos = LMC_MAXROM + 1, size = 0;

    // On programme le nettoyage du module.
    atexit(lmc_compilerCleanup);

    // On insère les mots-clés dans la table de hachage. On ne le fait
    // pas avec une fonction à attribut "constructor" car le
    // compilateur n'est pas nécéssairement utilisé à tous les coups,
    // et il ne peut être utilisé qu'une fois.
    lmc_hashInsertList(lmc_compiler_mnemonics);

    // On prépare les descripteur de fichiers des sources et
    // destination et on réserve l'espace pour les codes de départ et
    // taille du programme dans le fichier de destination. Cela évite
    // à l'utilisateur d'avoir à calculer la taille, et s'il ne donne
    // pas de directive "start", on met le départ à la valeur par
    // défaut.
    output = lmc_compilerOpenFiles(source, dest);
    lmc_compilerWrite(output, startpos, size);

    // On analyse le fichier source, et on inscrit les données
    // compilées dans le fichier de destination. On ferme ensuite la
    // source une fois l'analyse terminée.
    status = yyparse(&startpos, &size, output, source);
    fclose(yyin);

    // On modifie les espaces réservés pour les informations sur le
    // programme avec les-dites informations. On ferme ensuite la
    // destination, puisqu'il n'y a plus rien à y écrire. On indique
    // aussi le chemin du fichier de destination s'il est différent de
    // celui précisé (ou que dest est NULL, en cas).
    rewind(output);
    lmc_compilerWrite(output, startpos, size);
    fclose(output);
    if (!status && realdest != dest) printf("LMC: compiled to '%s'\n", realdest);

    return status;
}

static FILE* lmc_compilerOpenFiles(const char* restrict source, const char* dest)
{
    FILE* output;

    // On modifie la source de stdin au fichier donné.
    if (!(yyin = fopen(source, "r")))
        err(EXIT_FAILURE, "%s", source);

    realdest = dest && *dest ? dest : LMC_BIN;
    if (!(output = fopen(realdest, "w")))
        err(EXIT_FAILURE, "%s", realdest);
    return output;
}

void lmc_compilerWrite(FILE* output, LmcRam code, LmcRam value)
{
    LmcRam hexcodes[2] = { code, value };
    if (fwrite(&hexcodes, sizeof(LmcRam), 2, output) != 2)
        err(EXIT_FAILURE, "could not write codes (" LMC_HEXFMT "," LMC_HEXFMT ") in %s",
            LMC_MAXDIGITS, code,
            LMC_MAXDIGITS, value,
            realdest
        );
}
