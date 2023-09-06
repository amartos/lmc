/**
 * @file      common.h
 * @version   0.1.0
 * @brief     Common units tests structures.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 *
 * @addtogroup UnitsTests
 * @{
 */

#ifndef LMC_COMMON_H_
#define LMC_COMMON_H_

/**
 * Custom units tests library.
 * @see https://github.com/amartos/Sccroll
 */
#include <sccroll.h>

/**
 * @def MANUALIN
 * @since 0.1.0
 * @brief Program manually input through stdin.
 *
 * 20 instructions in total:
 * address 0x30, size 0x12
 * out     0x42
 * out     0x23
 * nand    0x00
 * add     0x01
 * nand    0x00
 * nand    0x01
 * store @ 0x30
 * out   @ 0x30
 * stop    0x00
 */
#define MANUALIN                                \
    "30\n12\n"                                  \
    "01\n42\n"                                  \
    "01\n23\n"                                  \
    "22\n00\n"                                  \
    "20\n01\n"                                  \
    "22\n00\n"                                  \
    "22\n00\n"                                  \
    "48\n30\n"                                  \
    "41\n30\n"                                  \
    "04\n00\n"

/**
 * @def MANUALOUT
 * @since 0.1.0
 * @brief Expected output for execution of #MANUALIN on stdout.
 */
#define MANUALOUT                               \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"                                    \
    "? >? >"

/**
 * @name Programs paths.
 * @{
 */

/**
 * @def PROGS
 * @since 0.1.0
 * @brief Programs directory.
 */
#define PROGS "tests/assets/programs/"

/**
 * @def UNDEFINED
 * @since 0.1.0
 * @brief Non-existant program file.
 */
#define UNDEFINED "foobar"

/**
 * @def _BOOTSTRAP
 * @since 0.1.0
 * @brief Compiled bootstrap.
 * @param ... Prefixes for the file name.
 */
#define _BOOTSTRAP(...) PROGS __VA_ARGS__ "bootstrap"

/**
 * @def BOOTSTRAP
 * @since 0.1.0
 * @brief Base compiled bootstrap.
 */
#define BOOTSTRAP _BOOTSTRAP()

/**
 * @def ALTBOOTSTRAP
 * @since 0.1.0
 * @brief Alternative compiled bootstrap.
 */
#define ALTBOOTSTRAP _BOOTSTRAP("alt")

/**
 * @def BIGBOOTSTRAP
 * @since 0.1.0
 * @brief Bootstrap program, but bigger than ROM.
 */
#define BIGBOOTSTRAP _BOOTSTRAP("big")

/**
 * @def PRODUCT
 * @since 0.1.0
 * @brief Compiled program calculating the product of two integers.
 */
#define PRODUCT PROGS "product"

/**
 * @def QUOTIENT
 * @since 0.1.0
 * @brief Compiled program calculating the quotient of an Euclidean
 * division.
 */
#define QUOTIENT PROGS "quotient"

/**
 * @def CMDLINE
 * @since 0.1.0
 * @brief Interactive mode value.
 */
#define CMDLINE NULL

/**
 * @def MALFORMED
 * @since 0.1.0
 * @brief Malformed program.
 */
#define MALFORMED PROGS "error"

/**
 * @def DUMMY
 * @since 0.1.0
 * @brief Program doing nothing.
 *
 * This program implements some syntaxes that should be detected by
 * the compiler. The goal with this is to check that those are indeed
 * detected and properly handled.
 */
#define DUMMY PROGS "dummy"

/**
 * @def DUMMYCODE
 * @since 0.1.0
 * @brief Expected compiled code of #DUMMY.
 *
 * The null byte near the end is there on purpose, to ensure that the
 * whole string is considered, not up to the first null.
 */
#define DUMMYCODE "\xad\x0e\x20\x23\x40\x53\xd0\xaf\x09\x02\x41\xff\x40\x00\x12\x56"

/**
 * @def DUMMYCODELEN
 * @since 0.1.0
 * @brief Bytes length of #DUMMYCODE.
 *
 * The +3 correction is due to the x00 byte near the end of
 * #DUMMYCODE. The premature ending of the string introduces a bias
 * for the tests.
 */
#define DUMMYCODELEN (strlen(DUMMYCODE)+3)
/** @} */

#endif // LMC_COMMON_H_
