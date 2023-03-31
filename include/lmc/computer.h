/**
 * @file      computer.h
 * @version   0.1.0
 * @brief     Module principal de l'ordinateur en papier.
 * @year      2022
 * @author    Alexandre Martos
 * @email     alexandre.martos (at) protonmail.ch
 * @copyright GNU General Public License v3
 * @compilation
 * flex lmc.lang.lex -o lexlang.c
 * gcc -Wall -std=gnu99 computer.c -o computer.o
 * @addtogroup Computer
 * @{
 */

#ifndef LMC_COMPUTER_H_
#define LMC_COMPUTER_H_

#include <err.h>     // pour les fonctions de gestion d'erreur
#include <errno.h>   // pour les codes errno
#include <stdbool.h> // pour le type bool
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name La mémoire
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

/**
 * @def LMC_HEXFMT
 * @since 0.1.0
 * @brief Le format utilisé pour les nombres hexadécimaux affichés.
 * @param * LMC_MAXDIGITS
 */
#define LMC_HEXFMT "%0*x"

/**
 * @struct LmcMemoryArray
 * @since 0.1.0
 * @brief Table de valeurs LmcRam.
 */
typedef struct LmcMemoryArray {
    LmcRam* values; /**< Table des valeurs. */
    size_t max;     /**< Taille maximale de LmcMemoryArray::values. */
    size_t current; /**< Premier index libre de LmcMemoryArray::values. */
} LmcMemoryArray;

/**
 * @struct LmcMemory
 * @since 0.1.0
 * @brief Structure de la mémoire de la machine.
 */
typedef struct LmcMemory {
    struct {
        LmcRam wr;          /**< Le registre mot. */
        LmcRam sr;          /**< Le registre de sélection. */
    } cache;                /**< Le cache de la mémoire. */
    LmcRam ram[LMC_MAXRAM]; /**< Le stockage mémoire. */
} LmcMemory;

// clang-format off

/******************************************************************************
 * @}
 * @name L'unité de contrôle
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcControlUnit
 * @since 0.1.0
 * @brief La structure de l'unité de contrôle de l'ordinateur.
 */
typedef struct LmcControlUnit {
    struct {
        LmcRam op; /**< Le registre de code opératoire. */
        LmcRam ad; /**< Le registre d'adresse. */
    } ir;          /**< Le registre d'instruction. */
    LmcRam pc;     /**< Le registre 'Program Counter'. */
} LmcControlUnit;

// clang-format off

/******************************************************************************
 * @}
 * @name L'unité arithmétique et logique
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcLogicUnit
 * @since 0.1.0
 * @brief La structure de l'unité arithmétique et logique.
 */
typedef struct LmcLogicUnit {
    LmcRam acc;    /**< L'accumulateur. */
    LmcRam opcode; /**< Le registre pour les codes d'opération. */
} LmcLogicUnit;

// clang-format off

/******************************************************************************
 * @}
 * @name Le bus et les Entrées/Sorties
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct Bus
 * @since 0.1.0
 * @brief La structure contenant les entrées/sorties.
 */
typedef struct LmcBus {
    FILE* input;        /**< L'entrée. */
    FILE* output;       /**< La sortie. */
    const char* prompt; /**< L'invite de commande. */
    LmcRam buffer;      /**< Un buffer entre les E/S et la mémoire. */
    bool newline;       /**< Drapeau indiquant s'il faut afficher un
                         * caractère de fin de ligne avant l'output à
                         * venir. */
} LmcBus;

// clang-format off

/******************************************************************************
 * @}
 * @name L'ordinateur en (e-)papier
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcComputer
 * @since 0.1.0
 * @brief Structure complète de l'ordinateur.
 */
typedef struct LmcComputer {
    LmcMemory mem;     /**< La mémoire de l'ordinateur. */
    LmcControlUnit cu; /**< L'unité de contrôle. */
    LmcLogicUnit alu;  /**< L'unité arthmétique et logique. */
    LmcBus bus;        /**< Le bus de communication avec le monde extérieur. */
    bool on;           /**< Drapeau indiquant si l'ordinateur est allumé. */
} LmcComputer;

// clang-format off

/******************************************************************************
 * @}
 * @name Fonctions principales de l'ordinateur
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute un programme.
 * @param filepath Le chemin d'un fichier compilé à exécuter, ou NULL
 * pour entrer en mode de programmation interactive.
 * @return La valeur du registre mot à l'extinction.
 */
LmcRam lmc_shell(const char* restrict filepath);

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

#endif // LMC_COMPUTER_H_
/** @} */
