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

#include "lmc/specs.h"

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
 * @name Structure de l'ordinateur en (e-)papier
 * @{
 ******************************************************************************/
// clang-format on

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

/**
 * @struct LmcLogicUnit
 * @since 0.1.0
 * @brief La structure de l'unité arithmétique et logique.
 */
typedef struct LmcLogicUnit {
    LmcRam acc;    /**< L'accumulateur. */
    LmcRam opcode; /**< Le registre pour les codes d'opération. */
} LmcLogicUnit;

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
 ******************************************************************************/
// clang-format on

#endif // LMC_COMPUTER_H_
/** @} */
