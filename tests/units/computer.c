/**
 * @file      computer.c
 * @version   0.1.0
 * @brief     LMC unit tests for the computer module.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2022-2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 */

#include "tests/common.h"
#include "lmc/computer.h"

#include <limits.h>
#include <stdio.h>
#include <stdarg.h>

// clang-format off

/******************************************************************************
 * Preparation
 ******************************************************************************/
// clang-format on

static const char* bootstrap = NULL;
static const char* file = NULL;

static const unsigned long long too_much = ~(0ull);
static char* too_much_prog = NULL;
static char* too_much_err = NULL;

__attribute__((constructor)) // This must be set before the tests registration.
void too_much_init(void)
{
    char buffer[BUFSIZ] = {0};
    sprintf(buffer, "30\n04\n%llu\n01\n42\n04\n00\n", too_much);
    too_much_prog = strdup(buffer);
    sprintf(buffer, "computer: Not a valid hexadecimal value: '%llu': Numerical result out of range\n", too_much);
    too_much_err = strdup(buffer);
}

__attribute__((destructor))
void too_much_clean(void)
{
    free(too_much_prog);
    free(too_much_err);
}

void sccroll_before(void) { bootstrap = NULL, file = NULL; }

void test_lmc_shell(void)
{
    int status = lmc_shell(bootstrap, file);
    if (!sccroll_mockGetTrigger()) assert(status == 0);
}

// clang-format off

/******************************************************************************
 * Tests
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
            // Ensure to have enough data for all the tests.
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
            // operation = result (base 16)
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
            // operation = result (base 16)
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
            // Programming error: write in ROM
            "30\n04\n"
            "48\n01\n" // store @ 01 (error)
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
            "30\n04\n"
            "XY\n"     // error: not a number
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
            "computer: Not a valid hexadecimal value: 'XY': Invalid argument\n"
        },
     }
)
{ assert(!lmc_shell(BOOTSTRAP, CMDLINE)); }

SCCROLL_TEST(
    invalid_number_errors_handling,
    .std = {
        [STDIN_FILENO]  = { .content.blob = too_much_prog },
        [STDOUT_FILENO] = { .content.blob =
            "? >? >"
            "? >"
            "? >? >"
            "? >? >"
            "42"
        },
        [STDERR_FILENO] = { .content.blob = too_much_err  },
     }
)
{ assert(!lmc_shell(BOOTSTRAP, CMDLINE)); }

SCCROLL_TEST(
    chbootstrap,
    .std = {
        [STDIN_FILENO]  = { .content.blob =
            // dummy simple program, not the focus of the test.
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

SCCROLL_TEST(
    bigbootstrap,
    .code = {
        .type  = SCCSTATUS,
        .value = EXIT_FAILURE,
    },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "computer: The bootstrap size (56 bytes) is larger than the ROM (32 bytes): File too large"
        },
    }
)
{ assert(!lmc_shell(BIGBOOTSTRAP, NULL)); }

SCCROLL_TEST(
    no_size_bootstrap,
    .code = {
        .type  = SCCSTATUS,
        .value = EXIT_FAILURE,
    },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "computer: " NOSIZEBOOTSTRAP ": missing size for bootstrap header: Exec format error"
        },
    }
)
{ assert(!lmc_shell(NOSIZEBOOTSTRAP, NULL)); }

SCCROLL_TEST(
    null_size_bootstrap,
    .std = {
        [STDIN_FILENO]  = { .content.blob = "30\n02\n04\n00" },
        [STDOUT_FILENO] = { .content.blob = "? >? >? >? >" },
        [STDERR_FILENO] = { .content.blob =
            "computer: " NULLBOOTSTRAP ": the bootstrap indicated size is null: Operation canceled\n"
            "Fallback to default bootstrap"
        },
    }
)
{ assert(!lmc_shell(NULLBOOTSTRAP, NULL)); }

SCCROLL_TEST(
    bad_size_bootstrap,
    .code = {
        .type  = SCCSTATUS,
        .value = EXIT_FAILURE,
    },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "computer: " WRONGBOOTSTRAP ": header size (16 bytes) differs from total read (2 bytes): Success"
        },
    }
)
{ assert(!lmc_shell(WRONGBOOTSTRAP, NULL)); }

SCCROLL_TEST(
    truncated_bootstrap,
    .code = {
        .type  = SCCSTATUS,
        .value = EXIT_FAILURE,
    },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "computer: " TRUNCBOOTSTRAP ": header size (16 bytes) differs from total read (0 bytes): Success"
        },
    }
)
{ assert(!lmc_shell(TRUNCBOOTSTRAP, NULL)); }

SCCROLL_TEST(
    bootstrap_is_stdstream,
    .code = {
        .type  = SCCSTATUS,
        .value = EXIT_FAILURE,
    },
    .std = {
        [STDERR_FILENO] = { .content.blob =
            "computer: " STDOUTPATH ": could not load bootstrap: Illegal seek"
        },
    }
)
{ assert(!lmc_shell(STDOUTPATH, NULL)); }
