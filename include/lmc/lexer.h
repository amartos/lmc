/**
 * @file        lexer.h
 * @version     0.1.0
 * @brief       Interface du traducteur.
 * @year        2023
 * @author      Alexandre Martos
 * @email       contact@amartos.fr
 * @copyright   GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup Lexer
 * @{
 */

#ifndef LMC_LEXER_H_
#define LMC_LEXER_H_

#include "lmc/specs.h"

#include <err.h>
#include <search.h>
#include <stdio.h>
#include <stdlib.h>

/**
 * @struct LmcRamArray
 * @since 0.1.0
 * @brief Table de valeurs LmcRam.
 */
typedef struct LmcRamArray {
    LmcRam* values; /**< Table des valeurs. */
    size_t max;     /**< Taille maximale de LmcRamArray::values. */
    size_t current; /**< Premier index libre de LmcRamArray::values. */
} LmcRamArray;

/**
 * @typedef LmcLexerCallback
 * @since 0.1.0
 * @brief Type de fonction de rappel à utiliser lors de la traduction.
 *
 * Ces fonctions sont utilisées pour gérer l'ajout du couple (@p code,
 * @p value) dans la table @p array).
 *
 * @note La fonction lmc_append() correspond à ce type.
 *
 * @param array La table de traduction.
 * @param code Le premier élément ajouté.
 * @param value Le second élément ajouté.
 */
typedef void (*LmcLexerCallback)(LmcRamArray* array, LmcRam code, LmcRam arg);

/**
 * @struct LmcLexer
 * @since 0.1.0
 * @brief Structure contenant les informations nécessaires à la
 * traduction.
 */
typedef struct LmcLexer {
    const char* desc;          /**< Descriptif de la traduction. */
    LmcLexerCallback callback; /**< Fonction gérant la table de traduction. */
    LmcRamArray values;        /**< Table de traduction. */
} LmcLexer;

// Variables et fonctions du module flex nécessaires au bon
// fonctionnement lors de l'analyse de texte.
extern FILE* yyin;
extern char* yytext;
extern int yylineno;
int yylex(void);
int yyerror(LmcLexer* lexer, const char* restrict msg) __attribute__((nonnull));
int yyparse(LmcLexer* lexer);

/**
 * @since 0.1.0
 * @brief Traduit la chaîne donnée en un code d'opération.
 * @param keyword Un mot clé.
 * @return le code correspondant au mot, ou 0 si le mot n'a aucun code
 * correspondant.
 */
LmcOpCodes lmc_opcode(char* keyword) __attribute__((nonnull));

/**
 * @since 0.1.0
 * @brief Convertit un code d'opération en mot-clé.
 * @param opcode Un code d'opération.
 * @return Le mot-clé correspondant au code d'opération, ou une chaîne
 * vide si le mot-clé ne correspond à aucun code.
 */
const char* lmc_keyword(LmcOpCodes opcode) __attribute__((returns_nonnull));

/**
 * @since 0.1.0
 * @brief Ajoute le couple (@p code, @p value) à la table de
 * traduction et incrémente l'index LmcMemoryArray::current.
 * @param array La table de traduction.
 * @param code Le premier élément ajouté.
 * @param value Le second élément ajouté.
 */
void lmc_append(LmcRamArray* array, LmcRam code, LmcRam value)
    __attribute__((nonnull (1)));

#endif // LMC_LEXER_H_
/** @} */
