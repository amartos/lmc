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
    INV = 0x01, /**< Inverse la valeur/signification. */
    NOT = 0x02, /**< Donne le NON booléen de la valeur. */
    HLT = 0x04, /**< Éteint l'ordinateur jusqu'au prochain programme. */
    WRT = 0x08, /**< Écrit une valeur en mémoire. */
    JMP = 0x10, /**< Change l'emplacement mémoire courant. */
    ADD = 0x20, /**< Addition. */
    VAR = 0x40, /**< La valeur est une adresse de la mémoire. */
    PTR = 0x80, /**< La valeur est un pointeur. */

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
} LmcOpCodes;

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // LMC_SPECS_H_
/** @} */
