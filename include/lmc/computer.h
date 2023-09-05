/**
 * @file      computer.h
 * @version   0.1.0
 * @brief     Core module of the LMC.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 *
 * @addtogroup Computer
 * @{
 */

#ifndef LMC_COMPUTER_H_
#define LMC_COMPUTER_H_

#include "lmc/specs.h"

#include <err.h>
#include <errno.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name The LMC structure
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcMemory
 * @since 0.1.0
 * @brief LMC memory.
 */
typedef struct LmcMemory {
    struct {
        LmcRam wr;          /**< Word Register. */
        LmcRam sr;          /**< Selection Register. */
    } cache;                /**< Memory cache. */
    LmcRam ram[LMC_MAXRAM]; /**< Random Access Memory. */
} LmcMemory;

/**
 * @struct LmcControlUnit
 * @since 0.1.0
 * @brief The LMC control unit.
 */
typedef struct LmcControlUnit {
    struct {
        LmcRam op; /**< OPerations register. */
        LmcRam ad; /**< ADdress register. */
    } ir;          /**< Instructions Register. */
    LmcRam pc;     /**< Program Counter. */
} LmcControlUnit;

/**
 * @struct LmcLogicUnit
 * @since 0.1.0
 * @brief The LMC logic unit.
 */
typedef struct LmcLogicUnit {
    LmcRam acc;    /**< ACCumulator. */
    LmcRam opcode; /**< OPerations Code register. */
} LmcLogicUnit;

/**
 * @struct Bus
 * @since 0.1.0
 * @brief The LMC bus.
 */
typedef struct LmcBus {
    FILE* input;        /**< The computer input device. */
    FILE* output;       /**< The computer output device. */
    const char* prompt; /**< The command line prompt. */
    LmcRam buffer;      /**< A one-byte buffer between IO and memory. */
} LmcBus;

/**
 * @struct LmcDebugger
 * @since 0.1.0
 * @brief The LMC debugger.
 */
typedef struct LmcDebugger {
    LmcRam brk;    /**< BReaK address register. */
    LmcRam prt;    /**< Traced address register (PRinT). */
    LmcRam opcode; /**< Debugger OPeration Code register. */
} LmcDebugger;

/**
 * @struct LmcComputer
 * @since 0.1.0
 * @brief The full LMC structure.
 */
typedef struct LmcComputer {
    LmcMemory mem;     /**< MEMory. */
    LmcControlUnit cu; /**< Control Unit. */
    LmcLogicUnit alu;  /**< Arithmetic-Logic Unit. */
    LmcBus bus;        /**< Bus. */
    LmcDebugger dbg;   /**< DeBuGger. */
    bool on;           /**< flag indicating of the computer is on, or
                        * (if @c false) in shutdown process/off. */
} LmcComputer;

// clang-format off

/******************************************************************************
 * @}
 * @name LMC functions
 *
 * @{
 * @param filepath The compiled program to run. @c NULL indicate to
 * swtch to in interactive mode where the user must enter the program
 * manually.
 * @param bootstrap A compiled bootstrap file path.
 * @return The word register value at shutdown.
 ******************************************************************************/
// clang-format on

/**
 * @typedef LmcExec
 * @since 0.1.0
 * @brief Prototype of the LMC execution functions.
 */
typedef LmcRam (*LmcExec)(const char* bootstrap, const char* filepath);

/**
 * @since 0.1.0
 * @brief Execute a program without the debugger.
 */
LmcRam lmc_shell(const char* restrict bootstrap, const char* restrict filepath);

/**
 * @since 0.1.0
 * @brief Execute a program with the debugger.
 */
LmcRam lmc_dbgShell(const char* restrict bootstrap, const char* restrict filepath);

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // LMC_COMPUTER_H_
/** @} */
