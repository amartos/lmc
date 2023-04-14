/**
 * @file      specs.h
 * @version   0.1.0
 * @brief     Spécifications générales de l'ordinateur en papier.
 * @year      2023
 * @author    Alexandre Martos
 * @email     alexandre.martos (at) protonmail.ch
 * @copyright GNU General Public License v3
 * @compilation cf. lmc.h
 *
 * @addtogroup Specs Spécifications de l'ordinateur en papier
 * @{
 */

#ifndef LMC_SPECS_H_
#define LMC_SPECS_H_

#include <stdlib.h>

// clang-format off

/******************************************************************************
 * @name Types et valeurs pour la mémoire
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef LmcRam
 * @since 0.1.0
 * @brief Type de donnée pour les adresses de la mémoire.
 */
// char est déjà non signé, mais on appuie le fait, ici.
typedef unsigned char LmcRam;

/**
 * @enum LmcMemoryCaracs
 * @since 0.1.0
 * @brief Caractéristiques numériques de la mémoire de l'ordinateur.
 */
typedef enum LmcMemoryCaracs {
    LMC_MAXRAM    = 0x100,              /**< Quantité de mémoire maximale (octets). */
    LMC_MAXROM    = 0x20,               /**< Quantité de mémoire en lecture seule (octets). */
    LMC_MAXDIGITS = sizeof(LmcRam) * 2, /**< Nombre de chiffres des valeurs de la mémoire. */
    LMC_MEMCOL    = 0x0f,               /**< Nombre d'adresses par ligne de dump. */
    LMC_SIGN      = LMC_MAXRAM >> 1,    /**< Masque pour le bit de signe. */
} LmcMemoryCaracs;

// clang-format off

/******************************************************************************
 * @}
 * @name Structures et macros utilitaires
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def LMC_HEXFMT
 * @since 0.1.0
 * @brief Le format utilisé pour les nombres hexadécimaux affichés.
 * @param * LMC_MAXDIGITS
 */
#define LMC_HEXFMT "%0*x"

// clang-format off

/******************************************************************************
 * @}
 * @name Programmation de l'ordinateur.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum LmcOpCodes
 * @since 0.1.0
 * @brief Les codes opératoires de l'ordinateur en papier.
 */
typedef enum __attribute__((__packed__)) LmcOpCodes {
    // Primitives
    INV = 1 << 0, /**< Inverse la valeur/signification. */
    NOT = 1 << 1, /**< Donne le NON booléen de la valeur. */
    HLT = 1 << 2, /**< Éteint l'ordinateur jusqu'au prochain programme. */
    WRT = 1 << 3, /**< Écrit une valeur en mémoire. */
    JMP = 1 << 4, /**< Change l'emplacement mémoire courant. */
    ADD = 1 << 5, /**< Addition. */
    VAR = 1 << 6, /**< La valeur est une adresse de la mémoire. */
    PTR = 1 << 7, /**< La valeur est un pointeur. */

    // Combinaisons
    INDIR = VAR | PTR, /**< Déréférence un pointeur. */

    // Instructions
    // les !WRT sont inutiles ici, mais sont laissés pour montrer la
    // correspondant de la valeur.
    SUB   = ADD | INV,  /**< Opération de soustraction. */
    NAND  = ADD | NOT,  /**< Opération booléenne NON ET. */
    LOAD  = !WRT,       /**< Charge une valeur dans l'accumulateur. */
    STORE = WRT,        /**< Stocke une valeur en mémoire. */
    IN    = WRT | INV,  /**< Enregistre une valeur venant de l'utilisateur. */
    OUT   = !WRT | INV, /**< Affiche une valeur sur le tampon de sortie. */
    JUMP  = JMP,        /**< Saute vers une adresse (alias de #JMP).*/
    BRN   = JMP | INV,  /**< Saute si l'accumulateur est négatif. */
    BRZ   = JMP | NOT,  /**< Saute si l'accumulateur est nul.*/
    START = PTR,        /**< Indique une adresse de démarrage. */

    // Instructions spécifiques au debugger.
    DEBUG = HLT   | INV, /**< Allumer/éteindre le debugger (selon l'argument). */
    DUMP  = DEBUG | NOT, /**< Afficher toute la mémoire entre deux adresses. */
    BREAK = DEBUG | WRT, /**< Enregistrer un point d'arrêt. */
    FREE  = BREAK | NOT, /**< Libérer un point d'arrêt. */
    CONT  = DEBUG | JMP, /**< Continuer jusqu'au prochain point d'arrêt. */
    NEXT  = CONT  | NOT, /**< Exécuter la prochaine instruction et s'arrêter. */
    // PRINT utilise ADD et non OUT car la valeur OUT vaut "0|INV", ce
    // qui signifierait que PRINT équivaudrait à DEBUG... ce qui
    // poserait un problème.
    PRINT = DEBUG | ADD, /**< Afficher le contenu d'une adresse à chaque étape. */
    CLEAR = PRINT | NOT, /**< Arrêter d'afficher le contenu d'une adresse. */
} LmcOpCodes;

/**
 * @def LMC_PROGLANG
 * @since 0.1.0
 * @brief Macro de conversion opcode <-> chaîne de caractères.
 */
#define LMC_PROGLANG(macro)                     \
    macro(VAR,"@")                              \
    macro(INDIR,"*@")                           \
    macro(ADD,"add")                            \
    macro(SUB,"sub")                            \
    macro(NAND,"nand")                          \
    macro(LOAD,"load")                          \
    macro(STORE,"store")                        \
    macro(IN,"in")                              \
    macro(OUT,"out")                            \
    macro(JUMP,"jump")                          \
    macro(BRN,"brn")                            \
    macro(BRZ,"brz")                            \
    macro(HLT,"stop")                           \
    macro(START,"start")                        \
    macro(DEBUG, "debug")                       \
    macro(BREAK, "break")                       \
    macro(FREE, "free")                         \
    macro(CONT, "continue")                     \
    macro(NEXT, "next")                         \
    macro(PRINT, "print")                       \
    macro(DUMP, "dump")

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // LMC_SPECS_H_
/** @} */
