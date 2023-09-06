/**
 * @file      specs.h
 * @version   0.1.0
 * @brief     LMC specifications.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 *
 * @addtogroup Specs LMC computer specifications
 * @{
 */

#ifndef LMC_SPECS_H_
#define LMC_SPECS_H_

#include <stdlib.h>

// clang-format off

/******************************************************************************
 * @name Memory types
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef LmcRam
 * @since 0.1.0
 * @brief Memory data type.
 */
// yes, char is already unsigned, but this is to insist on the fact.
typedef unsigned char LmcRam;

/**
 * @enum LmcMemoryCaracs
 * @since 0.1.0
 * @brief Numerical constants of the LMC.
 */
typedef enum LmcMemoryCaracs {
    LMC_MAXRAM    = 0x100,              /**< Max RAM size (bytes). */
    LMC_MAXROM    = 0x20,               /**< Max ROM size (bytes). */
    LMC_MAXDIGITS = sizeof(LmcRam) * 2, /**< Max digits for the memory values. */
    LMC_MEMCOL    = 0x0f,               /**< Max number of addresses per line for the dumps. */
    LMC_SIGN      = LMC_MAXRAM >> 1,    /**< Sign bit mask. */
    LMC_MAXVAL    = LMC_MAXRAM,         /**< Max value for each memory slot. */
} LmcMemoryCaracs;

// clang-format off

/******************************************************************************
 * @}
 * @name Messages formatting
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def LMC_HEXFMT
 * @since 0.1.0
 * @brief Hexadecimal numbers format string.
 * @param * #LMC_MAXDIGITS
 * @param x The value to print in hexadecimal.
 */
#define LMC_HEXFMT "%0*x"

// clang-format off

/******************************************************************************
 * @}
 * @name LMC language.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum LmcOpCodes
 * @since 0.1.0
 * @brief The LMC operation codes.
 *
 * The PC acronym used in the structure documentation corresponds to
 * the Program Counter.
 */
typedef enum __attribute__((__packed__)) LmcOpCodes {
    // Primitives
    INV = 1 << 0, /**< Invert the value/signification. */
    NOT = 1 << 1, /**< Boolean NOT. */
    HLT = 1 << 2, /**< Stop the current program. */
    WRT = 1 << 3, /**< Write. */
    JMP = 1 << 4, /**< Jump. */
    ADD = 1 << 5, /**< Addition. */
    VAR = 1 << 6, /**< The given value is a variable. */
    PTR = 1 << 7, /**< The given value is a pointer. */

    // Combinations
    INDIR = VAR | PTR, /**< Pointer dereferencing. */

    // Instructions
    // The !WRT are indeed useless, but used anyway to indicate the
    // corresponding combination of primitives.
    SUB   = ADD | INV,  /**< Substraction. */
    NAND  = ADD | NOT,  /**< Boolean NOT(AND). */
    LOAD  = !WRT,       /**< Read a value. */
    STORE = WRT,        /**< Store a value. */
    IN    = WRT | INV,  /**< Input from the bus input. */
    OUT   = !WRT | INV, /**< Output to the bus output. */
    JUMP  = JMP,        /**< Set the PC to the given address ("jump to").*/
    BRN   = JMP | INV,  /**< JUMP but only if the accumulator is less than @c 0. */
    BRZ   = JMP | NOT,  /**< JUMP but only if the accumulator is equal to @c 0. */
    START = PTR,        /**< The value is a start address. */

    // Debugger instructions.
    DEBUG = HLT   | INV, /**< Step in the debugger depending on the argument. */
    DUMP  = DEBUG | NOT, /**< Dump the memory between two given addresses. */
    BREAK = DEBUG | WRT, /**< Store a breakpoint. */
    FREE  = BREAK | NOT, /**< Free the breakpoints. */
    CONT  = DEBUG | JMP, /**< Skip the debug step until the PC is at the given address. */
    NEXT  = CONT  | NOT, /**< Execute the next program instruction and step in the debugger. */
    // PRINT uses ADD instead of OUT because OUT is "0|INV", which
    // would mean that PRINT would be equal to DEBUG, and would make
    // it impossible to distinguish the two.
    PRINT = DEBUG | ADD, /**< Print the given address value each time PC goes through it. */
    CLEAR = PRINT | NOT, /**< Stop printing memory values. */
} LmcOpCodes;

/**
 * @def LMC_PROGLANG
 * @since 0.1.0
 * @brief Opcode <> keywords conversion macro.
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
