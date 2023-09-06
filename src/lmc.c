/**
 * @file      lmc.c
 * @version   0.1.0
 * @brief     Main file of the LMC computer software.
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright 2023 Alexandre Martos <contact@amartos.fr>
 * @license   GPLv3
 *
 * @addtogroup LMCInternals
 * @{
 */

#include "lmc.h"

// clang-format off

/******************************************************************************
 * @name Command line analysis and documentation.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcArguments
 * @since 0.1.0
 * @brief Arguments handler structure.
 */
typedef struct LmcArguments {
    size_t cur;   /**< Last free LmcArguments::files index. */
    size_t max;   /**< Max size of LmcArguments::files. */
    char** files; /**< Programs file paths. */
    char* source; /**< Source file of the program to compile. */
    const char* bootstrap; /**< Compiled bootstrap file path. */
    bool debug;   /**< Option flag to use the debugger (@c true) or
                   * not (@c false). */
} LmcArguments;

/**
 * @var cmdargs
 * @since 0.1.0
 * @brief Arguments given on the commandline.
 */
static LmcArguments cmdargs = { .cur = -1, };

/**
 * @enum LmcOptions
 * @since 0.1.0
 * @brief LMC software options.
 */
typedef enum LmcOptions {
    LICENSEOPT = 'w', /**< Print the license. */
    VERSIONOPT = 'v', /**< Print the  version. */
    COMPILEOPT = 'c', /**< Compile a source file instead of running the LMC. */
    DEBUGONOPT = 'd', /**< Turn on the debugger. */
    BOOTSTPOPT = 'b', /**< Use a custom bootstrap. */
    MAXOPT = 0xff,    /**< Max value of options. */
} LmcOptions;

/**
 * @struc LmcDoc
 * @since 0.1.0
 * @brief Handler for documentation.
 */
typedef struct LmcDoc {
    const char* const help;             /**< Argp-compatible help template. */
    struct argp_option options[MAXOPT]; /**< Argp-compatible options. */
    const char* const argsdesc;         /**< Arguments descriptions.*/
} LmcDoc;

/**
 * @def LMC_VERSION
 * @since 0.1.0
 * @brief LMC version number.
 */
#define LMC_VERSION "LMC (Little Man Computer) version 0.1.0"

/**
 * @def LMC_LICENSE
 * @since 0.1.0
 * @brief The LMC copyright and license.
 */
#define LMC_LICENSE                                                     \
    LMC_VERSION "\n"                                                    \
    "Copyright (C) 2023 Alexandre Martos - contact@amartos.fr\n"        \
    "License GPLv3:\n"                                                  \
    "This program comes with ABSOLUTELY NO WARRANTY.\n"                 \
    "This is free software, and you are welcome to redistribute it\n"   \
    "under certain conditions; see <https://www.gnu.org/licenses/> for details."

/**
 * @var lmc_doc
 * @since 0.1.0
 * @brief The LMC documentation and options.
 */
static LmcDoc lmc_doc = {
    .argsdesc = "[FILE...]",
    .options  = {
        { .name = "license", .group = -1, .arg = NULL, .key = LICENSEOPT, .doc = "Print the licence" },
        { .name = "version", .group = -1, .arg = NULL, .key = VERSIONOPT, .doc = "Print the version" },
        { .name = "compile", .group = 1, .arg = "SOURCE", .key = COMPILEOPT, .doc = "Compile SOURCE to FILE" },
        { .name = "debug",   .group = 1, .arg = NULL,     .key = DEBUGONOPT, .doc = "Use the debugger" },
        { .name = "bootstrap", .group = 1, .arg = "BOOTFILE", .key = BOOTSTPOPT, .doc = "Use a custom compiled bootstrap stored in BOOTFILE" },
        { .name = NULL, .group = 0, .arg = NULL, .key = 0, .doc = NULL },
    },
    .help =
    "\n"
    LMC_VERSION
    "\n"
    "DESCRIPTION:\n"
    "\n"
    "This program emulates a computer based on the von Neumann\n"
    "architecture. It can be programmed in real-time or using pre-compiled\n"
    "binaries.\n"
    "\n"
    ""
    "LICENSE:\n"
    LMC_LICENSE,
};

/**
 * @since 0.1.0
 * @brief Parse the command line options.
 * @param key The option identifier.
 * @param arg The option argument.
 * @param state The current parsing state.
 * @return non-null in case of errors, otherwise @c 0.
 */
static error_t lmc_parseOpts(int key, char* arg, struct argp_state* state);

/**
 * @since 0.1.0
 * @brief Increment LmcArguments::files size.
 */
static void lmc_increaseFilesList(void);

// clang-format off

/******************************************************************************
 * @}
 * @name Cleanup
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Cleanup the LMC.
 */
static void lmc_cleanup(void) __attribute__((destructor));

// clang-format off

/******************************************************************************
 * @}
 * Implementation
 ******************************************************************************/
// clang-format on

int main(int argc, char** argv)
{
    int status = EXIT_SUCCESS;

    struct argp argp = {
        .options = lmc_doc.options,
        .parser = lmc_parseOpts,
        .args_doc = lmc_doc.argsdesc,
        .doc = lmc_doc.help,
    };
    argp_parse(&argp, argc, argv, 0, 0, &cmdargs);

    // The compile option was given.
    if (cmdargs.source)
        return lmc_compile(cmdargs.source, *cmdargs.files);

    LmcExec execfunc = cmdargs.debug ? lmc_shell : lmc_dbgShell;
    for (size_t i = 0; i < cmdargs.max && i <= cmdargs.cur && !status; ++i)
        status = execfunc(cmdargs.bootstrap, cmdargs.files[i]);

    // The status code is the last returned value of the programs,
    // thus the status of the last executed program. The
    // specifications prevent to return a value greater than 255.
    return status;
}

static error_t lmc_parseOpts(int key, char* arg, struct argp_state* state)
{
    (void) state;
    switch (key)
    {
    case VERSIONOPT: puts(LMC_VERSION); exit(EXIT_SUCCESS);
    case LICENSEOPT: puts(LMC_LICENSE); exit(EXIT_SUCCESS);
    case ARGP_KEY_ARG:
        if (cmdargs.cur >= cmdargs.max) lmc_increaseFilesList();
        cmdargs.files[++cmdargs.cur] = arg;
        break;
    case COMPILEOPT: cmdargs.source = arg; break;
    case DEBUGONOPT: cmdargs.debug = true; break;
    case BOOTSTPOPT: cmdargs.bootstrap = arg; break;
    case ARGP_KEY_END: break;
    default: return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static void lmc_increaseFilesList(void)
{
    // exponential growth to reduce the reallocarray calls.
    int newsize = cmdargs.max ? cmdargs.max * 2 : 1;
    char** new = reallocarray(cmdargs.files, newsize, sizeof(char*));
    if (!new) err(EXIT_FAILURE, "could not allocate for file list");
    memset(&new[cmdargs.max], 0, (newsize - cmdargs.max)*sizeof(char*));
    cmdargs.files = new;
    cmdargs.max = newsize;
}

static void lmc_cleanup(void) { free(cmdargs.files); }
