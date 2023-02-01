/**
 * @file      lmc.c
 * @version   0.1.0
 * @brief     Fichier source principal de l'ordinateur en papier.
 * @year      2023
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright GNU General Public License v3
 *
 * @addtogroup LMCInternals Structures internes de l'ordinateur en papier.
 * @{
 */

#include "lmc.h"

// clang-format off

/******************************************************************************
 * @name Analyse de la ligne de commande et documentation.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcArguments
 * @since 0.1.0
 * @brief Structure les valeurs des arguments du programme.
 */
typedef struct LmcArguments {
    size_t cur;   /**< Index courant de LmcArguments::files. */
    size_t max;   /**< Taille max de LmcArguments::files. */
    char** files; /**< Les chemins de fichiers supplémentaires donnés en arguments. */
    char* source; /**< Le fichier source du programme. */
    const char* bootstrap; /**< Le chemin du bootstrap compilé. */
    bool debug;   /**< Le drapeau indiquant d'allumer ou non le debugger. */
} LmcArguments;

/**
 * @var cmdargs
 * @since 0.1.0
 * @brief Les arguments donnés sur la ligne de commande.
 */
static LmcArguments cmdargs = { .cur = -1, };

/**
 * @enum LmcOptions
 * @since 0.1.0
 * @brief Identifiants des options courtes du programme.
 */
typedef enum LmcOptions {
    LICENSEOPT = 'w', /**< Afficher la licence. */
    VERSIONOPT = 'v', /**< Afficher la version. */
    COMPILEOPT = 'c', /**< Compile au lieu d'exécuter. */
    DEBUGONOPT = 'd', /**< Allume le debugger. */
    BOOTSTPOPT = 'b', /**< Utiliser un bootstrap personnalisé. */
    MAXOPT = 0xff,    /**< Nombre maximal d'options. */
} LmcOptions;

/**
 * @struc LmcDoc
 * @since 0.1.0
 * @brief Contient les informations pour la documentation du programme.
 */
typedef struct LmcDoc {
    const char* const help;             /**< Un template de l'aide du programme et argp-compatible */
    struct argp_option options[MAXOPT]; /**< Les options du programme. */
    const char* const argsdesc;         /**< Description des arguments du programme.*/
} LmcDoc;

/**
 * @def LMC_VERSION
 * @since 0.1.0
 * @brief Texte descriptif de la version du programme.
 */
#define LMC_VERSION "LMC (Little Man Computer) version 0.1.0"

/**
 * @def LMC_LICENSE
 * @since 0.1.0
 * @brief Texte descriptif de la licence et copyright.
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
 * @brief La documentation du programme.
 */
static LmcDoc lmc_doc = {
    .argsdesc = "[FICHIER...]",
    .options  = {
        { .name = "license", .group = -1, .arg = NULL, .key = LICENSEOPT, .doc = "Affiche la licence" },
        { .name = "version", .group = -1, .arg = NULL, .key = VERSIONOPT, .doc = "Affiche la version" },
        { .name = "compile", .group = 1, .arg = "source", .key = COMPILEOPT, .doc = "Compile une source vers FICHIER" },
        { .name = "debug",   .group = 1, .arg = NULL,     .key = DEBUGONOPT, .doc = "Allume le debugger" },
        { .name = "bootstrap", .group = 1, .arg = "FILE", .key = BOOTSTPOPT, .doc = "Le fichier contenant le bootstrap des programmes" },
        { .name = NULL, .group = 0, .arg = NULL, .key = 0, .doc = NULL },
    },
    .help =
    "\n"
    LMC_VERSION
    "\n"
    "DESCRIPTION:\n"
    "\n"
    "Simule un ordinateur en exécutant les lignes de code données via la ligne de"
    "commandes ou inscrites dans les FICHIERs. Une liste de fichiers n'est traitée"
    "qu'un à la fois; une fois le programme du précédent terminé, le prochain est "
    "utilisé.\n"
    ""
    "LICENSE:\n"
    LMC_LICENSE,
};

/**
 * @since 0.1.0
 * @brief Analyse une option de la ligne de commande.
 * @param key La clé LmcOptions d'identification de l'option.
 * @param arg Un argument de l'option.
 * @param state L'état actuel de l'analyse.
 * @return Une valeur non nulle en cas d'erreur, sinon 0.
 */
static error_t lmc_parseOpts(int key, char* arg, struct argp_state* state);

/**
 * @since 0.1.0
 * @brief Augmente de 1 la taille de LmcArguments::files.
 * @note Incrémente également LmcArguments::max.
 */
static void lmc_increaseFilesList(void);

// clang-format off

/******************************************************************************
 * @}
 * @name Nettoyage du module
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Effectue un nettoyage des variables du module.
 */
static void lmc_cleanup(void) __attribute__((destructor));

// clang-format off

/******************************************************************************
 * @}
 * Implémentation
 ******************************************************************************/
// clang-format on

int main(int argc, char** argv)
{
    int status = EXIT_SUCCESS;

    // On initialise les options et leur structure d'accueil, et on
    // analyse la ligne de commande.
    struct argp argp = {
        .options = lmc_doc.options,
        .parser = lmc_parseOpts,
        .args_doc = lmc_doc.argsdesc,
        .doc = lmc_doc.help,
    };
    argp_parse(&argp, argc, argv, 0, 0, &cmdargs);

    // On cherche à compiler un programme. L'exécution se fera dans un
    // second temps, donc on quitte à ce point.
    if (cmdargs.source)
        return lmc_compile(cmdargs.source, *cmdargs.files);

    // Les fichiers donnés éventuellement en arguments sont des
    // programmes censément compilés. On allume l'ordinateur et on
    // lui donne les programmes. Si aucun fichier n'est donné, on
    // entre en mode interactif et l'utilisateur pourra taper des
    // commandes via le prompt.
    LmcExec execfunc = cmdargs.debug ? lmc_shell : lmc_dbgShell;
    for (size_t i = 0; i < cmdargs.max && i <= cmdargs.cur && !status; ++i)
        status = execfunc(cmdargs.bootstrap, cmdargs.files[i]);

    // Le code de status est celui du dernier programme exécuté.
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
    // *2 pour limiter les appels à reallocarray (on augmente la
    // taille de la table de manière exponentielle).
    int newsize = cmdargs.max ? cmdargs.max * 2 : 1;
    char** new = reallocarray(cmdargs.files, newsize, sizeof(char*));
    if (!new) err(EXIT_FAILURE, "could not allocate for file list");
    memset(&new[cmdargs.max], 0, (newsize - cmdargs.max)*sizeof(char*));
    cmdargs.files = new;
    cmdargs.max = newsize;
}

static void lmc_cleanup(void) { free(cmdargs.files); }
