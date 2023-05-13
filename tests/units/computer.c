/**
 * @file      computer.c
 * @version   0.1.0
 * @brief     Tests unitaires du module computer.c
 * @year      2022
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright GNU General Public License v3
 * @compilation
 * gcc -xc -Wall -std=gnu99 -lsccroll -lgcov \
 *     tests/units/computer.c src/core/computer.c \
 *     -o build/bin/tests/units/computer
 */

#include "tests/common.h"
#include "lmc/computer.h"

#include <stdio.h>
#include <stdarg.h>

// clang-format off

/******************************************************************************
 * Préparation
 ******************************************************************************/
// clang-format on

static const char* bootstrap = NULL;
static const char* file = NULL;

void sccroll_before(void) { bootstrap = NULL, file = NULL; }

void test_lmc_shell(void)
{
    int status = lmc_shell(bootstrap, file);
    if (!sccroll_mockGetTrigger()) assert(status == 0);
}

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(
    manual_prog,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            MANUALIN
        },
        [STDOUT_FILENO] = { .content.blob =
            MANUALOUT
            "422301"
        },
     }
)
{
    bootstrap = BOOTSTRAP;
    file = CMDLINE;
    test_lmc_shell();
}

SCCROLL_TEST(
    manual_prog_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // On s'assure d'avoir assez de matière pour tous les
            // tests.
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
            MANUALIN MANUALIN MANUALIN MANUALIN
        },
        [STDOUT_FILENO] = { .content.blob =
            MANUALOUT "422301"
            "? >? >? >? >? >? >? >? >"
            "? >? >? >? >? >? >? >? >"
            "? >? >"
        },
     }
)
{
    bootstrap = BOOTSTRAP;
    file = CMDLINE;
    sccroll_mockPredefined(test_lmc_shell);
}

SCCROLL_TEST(
    file_prog,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // operation = résultat (base 16)
            "03\n08\n" // 3*8 = 18
            "07\n07\n" // 7*7 = 31
            "0f\n03\n" // f/3 = 5
            "09\n04\n" // 9/4 = 2
            "ff\n00\n" // ff/0 = error
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >18"
            "? >? >31"
            "? >? >05"
            "? >? >02"
        }
    }
)
{
    bootstrap = BOOTSTRAP;
    file = PRODUCT;
    test_lmc_shell();
    test_lmc_shell();
    file = QUOTIENT;
    test_lmc_shell();
    test_lmc_shell();
}

SCCROLL_TEST(
    div_by_zero,
    .std = {
        [STDIN_FILENO]  = { .content.blob = "ff\n00\n" }, // ff/0 = error
        [STDOUT_FILENO] = { .content.blob = "? >? >" },
    }
)
{ assert(lmc_shell(BOOTSTRAP, QUOTIENT) == 1); }

SCCROLL_TEST(
    file_prog_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // operation = résultat (base 16)
            "03\n08\n" // 3*8 = 18
            "07\n07\n" // 7*7 = 31
            "0f\n03\n" // f/3 = 5
            "09\n04\n" // 9/4 = 2
            "ff\n00\n" // ff/0 = error
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >18"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
        }
    }
)
{
    bootstrap = BOOTSTRAP;
    file = PRODUCT;
    sccroll_mockPredefined(test_lmc_shell);
}

SCCROLL_TEST(
    cmdline_eof,
    .std = {
        [STDIN_FILENO]  = { .content.blob = "" },
        [STDOUT_FILENO] = { .content.blob = "? >" },
    }
)
{ assert(!lmc_shell(BOOTSTRAP, CMDLINE)); }

SCCROLL_TEST(
    file_not_found,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE },
    .std = {
        [STDERR_FILENO] = { .content.blob = "computer: foobar: No such file or directory" },
    }
)
{ assert(lmc_shell(BOOTSTRAP, UNDEFINED)); }

SCCROLL_TEST(
    rom_error,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // Erreur de programmation : écriture dans la ROM
            "30\n04\n"
            "48\n01\n" // store @ 01 (erreur)
            "04\n00\n" // stop 00
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >? >"
            "? >? >"
        },
        [STDERR_FILENO] = { .content.blob =
            "computer: 01: read only: Bad address"
        },
     }
)
{ lmc_shell(BOOTSTRAP, CMDLINE); }

SCCROLL_TEST(
    notanumber_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // Pas un nombre (ligne de commande)
            "30\n04\n"
            "XY\n"     // erreur
            "01\n42\n" // out 42
            "04\n00\n" // stop 0
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >"
            "? >? >"
            "? >? >"
            "42"
        },
        [STDERR_FILENO] = { .content.blob =
            "computer: Not a hexadecimal value: 'XY': Invalid argument\n"
        },
     }
)
{ assert(!lmc_shell(BOOTSTRAP, CMDLINE)); }

SCCROLL_TEST(
    chbootstrap,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // Un programme simple, car ce n'est pas le sujet de test
            // ici.
            "30\n02\n"
            "04\n00\n" // stop 00
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >? >"
            "ffff"
        },
    }
)
{ assert(!lmc_shell(ALTBOOTSTRAP, NULL)); }
