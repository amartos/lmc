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

static LmcMockTrigger trigger = {0};

void sccroll_before(void) { trigger.errnum = 0, trigger.delay = 0; }

SCCROLL_MOCK(FILE*, fopen, const char* restrict pathname, const char* restrict mode)
{ return lmc_mockErr(fopen, ERRFOPEN, NULL, pathname, mode); }

SCCROLL_MOCK(int, ferror, FILE* stream)
{
    // Ce simulacre est nécessaire pour le déclenchement d'erreur de
    // fscanf et fread (puisque l'on analyse l'erreur de stream avec
    // ferror).
    return trigger.errnum ? 1 : __real_ferror(stream);
}

// BUG: __wrap_fscanf n'est pas appelé, il ne peut donc être défini
// comme simulacre. Peut-être à cause du côté variadique de la
// fonction?
int fscanf(FILE* restrict stream, const char* restrict format, ...)
{
    // puisque l'on ne peut pas vraiment passer les paramètres
    // variables directement à fscanf, ont utilise vfscanf comme
    // __real_fscanf. Le code source original de fscanf fait la même
    // chose.
    // TODO: rajouter référence code source
    va_list ap;
    va_start(ap, format);
    int status = vfscanf(stream, format, ap);
    va_end(ap);
    return trigger.errnum == ERRFSCANF && !trigger.delay-- ? EOF : status;
}

SCCROLL_MOCK(size_t, fread, void* ptr, size_t size, size_t nmemb, FILE* restrict stream)
{ return lmc_mockErr(fread, ERRFREAD, 0, ptr, size, nmemb, stream); }

// clang-format off

/******************************************************************************
 * Tests unitaires.
 ******************************************************************************/
// clang-format on

SCCROLL_TEST(
    manual_prog,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            "30\n12\n"
            "01\n42\n" // out 42
            "01\n23\n" // out 23
            "22\n00\n" // nand 0
            "20\n01\n" // add 1
            "22\n00\n" // nand 0
            "22\n00\n" // nand 1
            "48\n30\n" // store @ 30
            "41\n30\n" // out @ 30
            "04\n00\n" // stop 00,
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "422301"
        },
     }
)
{ assert(!lmc_shell(CMDLINE)); }

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
            "? >? >"
        }
    }
)
{
    assert(!lmc_shell(PRODUCT));
    assert(!lmc_shell(PRODUCT));
    assert(!lmc_shell(QUOTIENT));
    assert(!lmc_shell(QUOTIENT));
    assert(lmc_shell(QUOTIENT) == 1);
}

SCCROLL_TEST(
    cmdline_eof,
    .std = {
        [STDIN_FILENO]  = { .content.blob = "" },
        [STDOUT_FILENO] = { .content.blob = "? >" },
    }
)
{ assert(!lmc_shell(CMDLINE)); }

SCCROLL_TEST(
    file_not_found,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE },
    .std = {
        [STDERR_FILENO] = { .content.blob = "computer: foobar: No such file or directory" },
    }
)
{ assert(lmc_shell(UNDEFINED)); }

SCCROLL_TEST(
    fopen_errors_handling,
    .code = { .type = SCCSTATUS, .value = EXIT_FAILURE, },
    .std = {
        [STDERR_FILENO] = { .content.blob = "computer: " PRODUCT ": Success\n" },
     }
)
{ trigger.errnum = ERRFOPEN, lmc_shell(PRODUCT); }

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
{ lmc_shell(CMDLINE); }

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
{ assert(!lmc_shell(CMDLINE)); }

SCCROLL_TEST(
    fscanf_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // L'erreur ici est provoquée à la première lecture de
            // fscanf, et donc l'adresse de départ sera différente.
            "50\n"
            "25\n04\n"
            "41\n20\n" // out @ 20 (adresse après le jump du bootstrap)
            "04\n00\n" // stop 00
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >"
            "? >? >"
            "? >? >"
            "? >? >"
            "25"
        },
        [STDERR_FILENO] = { .content.blob = "computer: Success" },
     }
)
{ trigger.errnum = ERRFSCANF, assert(!lmc_shell(CMDLINE)); }

SCCROLL_TEST(
    fread_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            "30\n04\n"
            "01\n73\n" // out 73
            "04\n00\n" // stop 00
        },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >? >"
            "? >? >"
            "73"
        },
        [STDERR_FILENO] = { .content.blob = "computer: Success" },
     }
)
{ trigger.errnum = ERRFREAD, assert(!lmc_shell(PRODUCT)); }
