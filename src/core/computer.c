/**
 * @file       computer.c
 * @version    0.1.0
 * @brief      Ordinateur en papier.
 * @year       2023
 * @author     Alexandre Martos
 * @email      contact@amartos.fr
 * @copyright  GNU General Public License v3
 *
 * @addtogroup ComputerInternals Structures internes de l'ordinateur.
 * @{
 */

#include "lmc/computer.h"

// clang-format off

/******************************************************************************
 * @name Exécution
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute un programme avec ou sans le debugger.
 * @param filepath Le chemin d'un fichier compilé à exécuter, ou NULL
 * pour entrer en mode de programmation interactive.
 * @param debug Drapeau indiquant d'allumer (@c true) ou pas (@c
 * false) le debugger.
 * @param bootstrap Un fichier compilé contenant le bootstrap à
 * utiliser.
 * @return La valeur du registre mot à l'extinction.
 */
static LmcRam lmc_exec(const char* restrict bootstrap, const char* restrict filepath, bool debug);

/**
 * @since 0.1.0
 * @brief Charge un bootstrap dans l'ordinateur.
 * @param path Le chemin du fichier compilé contenant le bootstrap.
 */
static void lmc_bootstrap(const char* restrict path) __attribute__((nonnull));

// clang-format off

/******************************************************************************
 * @}
 * @name Debugging
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @def LMC_DBGPROMPT
 * @since 0.1.0
 * @brief L'invite de commande en mode de debug.
 */
#define LMC_DBGPROMPT                                       \
    "PC: " LMC_HEXFMT ", ACC: " LMC_HEXFMT " " LMC_PROMPT,  \
        LMC_MAXDIGITS, lmc_hal.cu.pc,                       \
        LMC_MAXDIGITS, lmc_hal.alu.acc

/**
 * @since 0.1.0
 * @brief Lance l'étape du debugger.
 * @return @c true pour passer à l'étape suivante, @c false pour
 * rester dans le mode debug.
 */
static bool lmc_debug(void);

/**
 * @since 0.1.0
 * @brief Effectue la phase 1 du debuggage.
 *
 * Affiche la valeur de l'adresse courante si elle celle-ci est
 * stockée dans LmcDebugger::prt, et indique si le debugger doit
 * s'arrêter pour demande d'instructions.
 * @return false pour sauter la phase de debug, true pour la
 * continuer.
 */
static bool lmc_dbg_phaseOne(void);

/**
 * @since 0.1.0
 * @brief Effectue la phase 2 du debugger.
 *
 * Affiche les valeurs de PC et ACC, puis attend des instructions de
 * l'utilisateur.
 */
static void lmc_dbg_phaseTwo(void);

/**
 * @since 0.1.0
 * @brief Effectue la phase 3 du debugger.
 *
 * Exécute la commande entrée par l'utilisateur en phase 2.
 * @return false pour stopper la phase de debug, true pour la
 * continuer.
 */
static bool lmc_dbg_phaseThree(void);

/**
 * @since 0.1.0
 * @brief Affiche le contenu de la mémoire de l'ordinateur entre deux
 * adresses.
 * @param start L'adresse de début.
 * @param end L'adresse de fin.
 */
static void lmc_dump(LmcRam start, LmcRam end);

// clang-format off

/******************************************************************************
 * @}
 * @name Phases du cycle.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Effectue la phase I: "recherche de l'instruction".
 */
static void lmc_phaseOne(void);

/**
 * @since 0.1.0
 * @brief Effectue la phase II: "décodage de l'instruction, recherche
 * de l'opérande et calcul".
 * @param debug Indique si la phase 2 est lancée par le debugger.
 * @return true si la phase III doit être sautée, sinon false.
 */
static bool lmc_phaseTwo(bool debug);

/**
 * @since 0.1.0
 * @brief Effectue la phase III: "opération suivante".
 */
static void lmc_phaseThree(void);

// clang-format off

/******************************************************************************
 * @}
 * @name Gestion de l'I/O.
 *
 * Les fonctions et structures de ce groupe sont liées à l'interaction
 * de LmcBus avec l'ordinateur et le monde extérieur.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @def LMC_PROMPT
 * @since 0.1.0
 * @brief L'invite de commandes.
 */
#define LMC_PROMPT "? >"

/**
 * @def LMC_WRDVAL
 * @since 0.1.0
 * @brief La valeur contenue dans LmcComputer::mem::cache::wr dans un
 * format d'affichage avec les fonctions de la famille @c printf.
 */
#define LMC_WRDVAL LMC_HEXFMT, LMC_MAXDIGITS, lmc_hal.mem.cache.wr

/**
 * @since 0.1.0
 * @brief Récupère un code exadécimal à deux chiffres sur
 * LmcBus::input, et le stocke dans LmcComputer::bus::buffer.
 */
static void lmc_busInput(void);

/**
 * @since 0.1.0
 * @brief Gère l'entrée du bus.
 * @param filepath Un chemin de fichier de programme, ou @c NULL pour
 * une programmation interactive.
 * @return @c false si l'entée est interactive et que l'utilisateur
 * souhaite quitter le programme (EOF), sinon false.
 */
static bool lmc_setInput(const char* restrict filepath);

/**
 * @since 0.1.0
 * @brief Convertit une chaîne de caractères contenant des chiffres
 * hexadécimaux en un entier stocké dans LmcComputer::bus::buffer.
 * @param number La chaîne de caractères représentant le nombre.
 * @return EXIT_FAILURE en cas d'erreur, sinon EXIT_SUCCESS.
 */
static int lmc_convert(const char* restrict number) __attribute__((nonnull));

/**
 * @def lmc_busPrint
 * @since 0.1.0
 * @brief Affiche un message sur LmcComputer::bus::output.
 * @param fmt Une chaîne de formatage type printf.
 * @param ... Les arguments de la chaîne de formatage.
 */
#define lmc_busOutput(fmt, ...) fprintf(lmc_hal.bus.output, fmt, ##__VA_ARGS__)

// clang-format off

/******************************************************************************
 * @}
 * @name Opérations.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * since 0.1.0
 * @brief Détermine si l'opcode doit changer.
 * @param operation Le code d'opération sans les informations
 * d'indirection.
 */
static void lmc_opcalc(LmcRam operation);

/**
 * @since 0.1.0
 * @brief Retrouve l'opérande de l'opération stockée en mémoire, en
 * fonction du niveau d'indirection indiqué.
 * @param type Le type d'indirection voulu.
 */
static void lmc_indirection(LmcRam type);

/**
 * @since 0.1.0
 * @brief Effectue une opération.
 * @param operation Le code d'opération sans les informations
 * d'indirection.
 */
static bool lmc_operation(LmcRam operation);

/**
 * @since 0.1.0
 * @brief Effectue l'opération d'arithmétique décrite par
 * LmcComputer::alu::opcode avec les opérandes
 * LmcComputer::mem::cache::wr et LmcComputer::alu:acc.
 */
static void lmc_calc(void);

/**
 * @since 0.1.0
 * @brief Lis et écrit dans la mémoire.
 *
 * La fonction vérifie que l'adresse mémoire est correcte pour le mode
 * d'écriture (adresse >= #LMC_MAXROM) et lève une erreur @c EFAULT si
 * elle ne l'est pas. L'ordinateur s'arrête en cas d'erreur.
 *
 * @param address L'adresse de la source (mode 'r') ou de destination
 * (mode 'w').
 * @param value La destination de la donnée (mode 'r') ou la source de
 * la donnée (mode 'w').
 * @param mode caractère 'r' pour lire l'emplacement mémoire, ou 'w'
 * pour y écrire.
 */
static void lmc_rwMemory(LmcRam address, LmcRam* value, char mode) __attribute__((nonnull (2)));

#ifdef _UCODES

// clang-format off

/******************************************************************************
 * @}
 * @name Gestion des microcodes.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum LmcUcodes
 * @since 0.1.0
 * @brief Les microcodes.
 */
typedef enum __attribute__ ((__packed__)) LmcUcodes {
    PCTOSR = 1, /**< 1 Écrit la valeur du *program counter* dans le registre de sélection. */
    WRTOPC,     /**< 2 Écrit le contenu du registre mot dans *program counter*. */
    WRTOAC,     /**< 3 Écrit le contenu du registre mot dans l'accumulateur. */
    ACTOWR,     /**< 4 Écrit le contenu de l'accumulateur dans le registre mot.  */
    WRTOOP,     /**< 5 Écrit le contenu du registre mot dans le registre de code opératoire
                 * de l'unité arithmétique et logique (UAL). */
    WRTOAD,     /**< 6 Écrit le contenu du registre mot dans le regitre d'adresse. */
    ADTOSR,     /**< 7 Écrit le contenu du registre d'adresse dans le registre de sélection. */
    INTOWR,     /**< 8 Écrit le contenu du tampon d'entrée du bus dans le registre mot. */
    WRTOOU,     /**< 9 Écrit le contenu du registre mot sur le tampon d'ouput du bus. */
    ADDOPD,     /**< 10 Écrit la valeur ADD dans le registre de code opératoire de l'UAL. */
    SUBOPD,     /**< 11 Écrit la valeur SUB dans le registre de code opératoire de l'UAL. */
    DOCALC,     /**< 12 Effectue l'opération inscrite dans le registre de code opératoire de
                 * l'UAL. */
    SVTOWR,     /**< 13 Écrit la valeur stockée à l'adresse située dans le registre de
                 * sélection dans le registre mot. */
    WRTOSV,     /**< 14 Écrit la valeur du registre mot dans l'emplacement dont l'adresse est
                 * située dans le registre de sélection. */
    INCRPC,     /**< 15 Incrémente le *program counter*. */
    WINPUT,     /**< 16 Attend une entrée de l'utilisateur. */
    NANDOP,     /**< 17 Écrit la valeur NAND dans le registre de code opératoire de l'UAL. */
    LMCHLT,     /**< 18 Ajout personnel: éteint l'ordinateur. */
} LmcUcodes;

/**
 * @since 0.1.0
 * @brief Effectue une série d'opérations de microcodes.
 * @attention La valeur @c NULL est utilisée comme sentinelle de la
 * fonction (elle doit obligatoirement être la dernière donnée en
 * arguments).
 * @param ucode Un microcode.
 * @param ... Le reste des microcodes, avec un NULL en dernière
 * position.
 */
static void lmc_useries(unsigned int ucode, ...) __attribute__((sentinel));

/**
 * @since 0.1.0
 * @brief Effectue une opération de microcode.
 * @param ucode Le code de l'opération à effectuer.
 */
static void lmc_ucode(LmcUcodes ucode);

#endif // _UCODES

// clang-format off

/******************************************************************************
 * @}
 * @name L'ordinateur et son bootstrap.
 * @{
 ******************************************************************************/

/**
 * @var lmc_hal
 * @since 0.1.0
 * @brief L'ordinateur utilisé pour exécuter les programmes.
 */
static LmcComputer lmc_hal = {0};

/**
 * @var lmc_template
 * @since 0.1.0
 * @brief Un modèle de l'ordinateur à l'état initial.
 */
static const LmcComputer lmc_template = {
    .bus = { .prompt = LMC_PROMPT, },
    // le bootstrap
    .mem.ram = {
        // Le bootstrap.
        // opération    valeur   adresse traduction (en base 16)
        IN | VAR,       0x20, // 00      in @ 20
        IN | VAR,       0x22, // 02      in @ 22
        LOAD | VAR,     0x20, // 04      load @ 20
        STORE | VAR,    0x21, // 06      store @ 21
        IN | INDIR,     0x21, // 08      in *@ 21
        LOAD | VAR,     0x22, // 0a      load @ 22
        SUB,            0x01, // 0c      sub 01
        BRZ,    LMC_MAXROM-1, // 0e      brz 1f
        STORE | VAR,    0x22, // 10      store @ 22
        LOAD | VAR,     0x21, // 12      load @ 21
        ADD,            0x01, // 14      add 01
        JUMP,           0x06, // 16      jump 06
        // On saute plusieurs emplacements.
        [LMC_MAXROM-1] = JUMP,
    },
};


/******************************************************************************
 * @}
 * Implémentation
 ******************************************************************************/
// clang-format on

LmcRam lmc_shell(const char* restrict bootstrap, const char* restrict filepath)
{ return lmc_exec(bootstrap, filepath, false); }

LmcRam lmc_dbgShell(const char* restrict bootstrap, const char* restrict filepath)
{ return lmc_exec(bootstrap, filepath, true); }

static LmcRam lmc_exec(const char* restrict bootstrap, const char* restrict filepath, bool debug)
{
    // on reset l'ordinateur pour éviter de mélanger les données de
    // plusieurs programmes.
    lmc_hal = lmc_template;

    // On charge le bootstrap.
    lmc_bootstrap(bootstrap);

    // stdin et stdout ne sont pas des constantes, donc assignables
    // uniquement en runtime. Si aucun programme n'est donné au
    // départ, on utilise stdin par défaut.
    lmc_hal.bus.input  = stdin;
    lmc_hal.bus.output = stdout;
    lmc_setInput(filepath);

    // On exécute le programme.
    lmc_hal.on = true; // Hello Dave. You are looking well today.
    lmc_hal.dbg.opcode = debug ? DEBUG : 0;
    while (lmc_hal.on) {
        while(lmc_debug());
        lmc_phaseOne(), lmc_phaseTwo(false) ? lmc_phaseThree() : 0;
    }
    return lmc_hal.mem.cache.wr; // code de status du programme
}

static void lmc_bootstrap(const char* restrict path)
{
    FILE* file = fopen(path, "rb");
    // Puisque le bootstrap est compilé avec le même programme,
    // les deux premières valeurs seront la position initiale et
    // la taille du programme. Or, on sait où le programme doit se
    // situer, et la taille importe peu. Donc, on les saute.
    if (!file
        || fseek(file, sizeof(LmcRam)*2, SEEK_SET)
        || (!fread(lmc_hal.mem.ram, sizeof(LmcRam), LMC_MAXROM, file) && ferror(file)))
        err(EXIT_FAILURE, "%s: could not load bootstrap", path);
    fclose(file);
}

// clang-format off

/******************************************************************************
 * Debugging
 ******************************************************************************/
// clang-format on

static bool lmc_debug(void)
{
    return
        lmc_dbg_phaseOne()
        ? (lmc_dbg_phaseTwo(), lmc_dbg_phaseThree())
        : false;
}

static bool lmc_dbg_phaseOne(void)
{
    // On vérifie si l'ordinateur n'est pas en phase d'extinction, et
    // si le debugger est allumé.
    if (!(lmc_hal.on && lmc_hal.dbg.opcode))
        return false;

    // On vérifie si l'adresse courante est dans LmcDebugger::prt et
    // on affiche sa valeur si c'est le cas.
    if (lmc_hal.dbg.prt // on ne veut pas de l'adresse 0
        && lmc_hal.dbg.prt == lmc_hal.cu.pc)
        lmc_dump(lmc_hal.cu.pc, lmc_hal.cu.pc);

    // On vérifie que le debugger n'est pas en phase "continue", et
    // donc on saute toutes les adresses sauf celle de
    // LmcDebugger::brk ("break").
    if (lmc_hal.dbg.opcode == CONT
        && lmc_hal.dbg.brk // on ne veut pas s'arrêter à l'adresse 0
        && lmc_hal.cu.pc != lmc_hal.dbg.brk)
        return false;

    return true;
}

static void lmc_dbg_phaseTwo(void)
{
    // On modifie le prompt pour afficher les valeurs de PC et ACC.
    char prompt[BUFSIZ] = {0};
    sprintf(prompt, LMC_DBGPROMPT);
    lmc_hal.bus.prompt = prompt;

    // On récupère les codes d'opération et d'argument pour la phase
    // du debugger. On peut se permettre ici d'écraser l'opcode, car
    // la phase de debug est située en amont de la phase 1 de
    // l'ordinateur.
    lmc_busInput(), lmc_hal.alu.opcode   = lmc_hal.bus.buffer;
    lmc_busInput(), lmc_hal.mem.cache.wr = lmc_hal.bus.buffer;

    // On rétablit le prompt originel au cas où la commande arrête la
    // phase de debug.
    lmc_hal.bus.prompt = LMC_PROMPT;
}

static bool lmc_dbg_phaseThree(void) { return lmc_phaseTwo(true); }

static void lmc_dump(LmcRam start, LmcRam end)
{
    for (int addr = start; addr <= end && addr < LMC_MAXRAM; ++addr)
    {
        lmc_rwMemory(addr, &lmc_hal.mem.cache.wr, 'r');
        if (!(addr & LMC_MEMCOL) || start == end) {
            lmc_busOutput("\n" LMC_HEXFMT ": ", LMC_MAXDIGITS, addr);
        }
        lmc_busOutput(LMC_HEXFMT " ", LMC_MAXDIGITS, lmc_hal.mem.cache.wr);
    }
}

// clang-format off

/******************************************************************************
 * Phases du cycle.
 ******************************************************************************/
// clang-format on

static void lmc_phaseOne(void) {
#ifdef _UCODES
    lmc_useries(PCTOSR, SVTOWR, WRTOOP, INCRPC, NULL);
#else
    lmc_rwMemory(lmc_hal.cu.pc++, &lmc_hal.alu.opcode, 'r');
#endif
}

static bool lmc_phaseTwo(bool debug)
{
    // On sépare le code indiquant où chercher l'opérande de
    // celui de l'opération.
    LmcRam operation = lmc_hal.alu.opcode & ~(INDIR);
    LmcRam value     = lmc_hal.alu.opcode & INDIR;

    // On modifie le code opératoire au besoin.
    lmc_opcalc(operation);

    // Si le debugger lance une phase 2, aller chercher la valeur de
    // l'argument dans l'adresse courante de PC va écraser l'argument
    // donné via le debugger. On modifie donc la phase 2 pour éviter
    // cet écrasement.
    if (debug) { lmc_hal.mem.cache.sr = lmc_hal.mem.cache.wr; }
    else {
#ifdef _UCODES
    lmc_ucode(PCTOSR);
#else
    lmc_hal.mem.cache.sr = lmc_hal.cu.pc;
#endif
    }
    // On va chercher la valeur de l'opérande.
    lmc_indirection(value);

    // On lance l'opération.
    return lmc_operation(operation);
}

static void lmc_phaseThree(void) {
#ifdef _UCODES
    lmc_ucode(INCRPC);
#else
    ++lmc_hal.cu.pc;
#endif
}

// clang-format off

/******************************************************************************
 * Gestion de l'I/O
 ******************************************************************************/
// clang-format on

static bool lmc_setInput(const char* restrict filepath)
{
    if (filepath && !(lmc_hal.bus.input = fopen(filepath, "rb")))
        err(EXIT_FAILURE, "%s", filepath);
    else if (!filepath) {
        if (lmc_hal.bus.input != stdin) {
            fclose(lmc_hal.bus.input);
            lmc_hal.bus.input = stdin;
        }
        else if (feof(lmc_hal.bus.input))
            return (lmc_hal.on = false);
    }
    return true;
}

static void lmc_busInput(void)
{
    bool error = false, eof = false;
    char digits[BUFSIZ] = { 0 };

    if (lmc_hal.bus.input == stdin) fprintf(lmc_hal.bus.output, "%s", lmc_hal.bus.prompt);

    // Au lieu d'utiliser le format "%2x",on capte tout d'abord la
    // chaîne de caractères, et on la convertit en entière. Cette
    // méthode permet de gérer les cas où le premier caractère est
    // compatible avec un nombre hexadécimal, mais pas le suivant (ce
    // qui validerait tout de même le scan), exemple: 'foobar' qui
    // donnerait '0f'. Ici, si la conversion échoue, on sait que l'un
    // des caractères au moins n'est pas un chiffre hexadécimal.
    eof = lmc_hal.bus.input == stdin
        ? (fscanf(lmc_hal.bus.input, "%s", (char*)digits) < 1
           || (error = lmc_convert(digits)))
        : fread(&lmc_hal.bus.buffer, sizeof(LmcRam), 1, lmc_hal.bus.input) < 1;

    if (eof) {
        if (error) {
            errno = errno ? errno : EINVAL;
            warn("Not a hexadecimal value: '%s'", digits);
        }
        else if (ferror(lmc_hal.bus.input)) warn(NULL);

        // Si l'entrée n'est pas sur la ligne de commande, autant
        // ne pas risquer des erreurs supplémentaires à la lecture
        // du fichier. On abandonne, et on repasse sur stdin. Si
        // l'entrée est stdin, et que le signal EOF a été donné, on
        // quitte.
        return lmc_setInput(NULL) ? lmc_busInput() : NULL;
    }
}

static int lmc_convert(const char* restrict number)
{
    lmc_hal.bus.buffer = 0;
    if (!*number) return EXIT_SUCCESS;

    char* endptr;
    errno = 0;
    lmc_hal.bus.buffer = (LmcRam) strtoul(number, &endptr, 16);
    return strlen(number) > LMC_MAXDIGITS
        || errno
        || *endptr
        ? EXIT_FAILURE
        : EXIT_SUCCESS;
}

// clang-format off

/******************************************************************************
 * Opérations
 ******************************************************************************/
// clang-format on

static void lmc_opcalc(LmcRam operation)
{
    LmcRam opcode = 0;
    switch (operation) {
#ifdef _UCODES
    case ADD:  opcode = ADDOPD; goto op_calc;
    case SUB:  opcode = SUBOPD; goto op_calc;
    case NAND: opcode = NANDOP; goto op_calc;
    default:
    op_calc:   lmc_ucode(opcode); break; // si opcode est nul, ne fait rien
#else
    case ADD:  __attribute__((fallthrough));
    case SUB:  __attribute__((fallthrough));
    case NAND: opcode = operation;          goto op_calc;
    default:   opcode = lmc_hal.alu.opcode; goto op_calc;
    op_calc:   lmc_hal.alu.opcode = opcode; break;
#endif
    }
}

static void lmc_indirection(LmcRam type)
{
    // On va chercher la valeur de l'opérande.
    switch (type) {
#ifdef _UCODES
    case INDIR: lmc_useries(SVTOWR, WRTOAD, ADTOSR, NULL); __attribute__((fallthrough));
    case VAR:   lmc_useries(SVTOWR, WRTOAD, ADTOSR, NULL); __attribute__((fallthrough));
    default:    lmc_ucode(SVTOWR); break;
#else
    case INDIR: lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.mem.cache.sr, 'r'); __attribute__((fallthrough));
    case VAR:   lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.mem.cache.sr, 'r'); __attribute__((fallthrough));
    default:    lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.mem.cache.wr, 'r'); break;
#endif
    }
}

static bool lmc_operation(LmcRam operation)
{
    LmcRam* value = NULL;
    switch (operation) {
    case BRN:   if (!(lmc_hal.alu.acc & LMC_SIGN)) break; goto op_jump;
    case BRZ:   if (lmc_hal.alu.acc != 0) break; goto op_jump;
    case ADD:   __attribute__((fallthrough));
    case SUB:   __attribute__((fallthrough));
#ifdef _UCODES
    case NAND:  lmc_ucode(DOCALC); break;
    case LOAD:  lmc_ucode(WRTOAC); break;
    case OUT:   lmc_useries(SVTOWR, WRTOOU, NULL); break;
    case IN:    lmc_useries(WINPUT, INTOWR, WRTOSV, NULL); break;
    case STORE: lmc_useries(ACTOWR, WRTOSV, NULL); break;
    case JUMP:
    op_jump:    lmc_ucode(WRTOPC); return false;
    case HLT:   lmc_ucode(LMCHLT); return false;
#else
    case NAND:  lmc_calc(); break;
    case LOAD:  lmc_hal.alu.acc = lmc_hal.mem.cache.wr; break;
    case OUT:
        lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.mem.cache.wr, 'r');
        lmc_busOutput(LMC_WRDVAL);
        break;
    case IN:
        lmc_busInput();
        lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.bus.buffer, 'w');
        break;
    case STORE:
        lmc_rwMemory(lmc_hal.mem.cache.sr, &lmc_hal.alu.acc, 'w');
        break;
    case JUMP:
    op_jump:    lmc_hal.cu.pc = lmc_hal.mem.cache.wr; return false;
    case HLT:   return (lmc_hal.on = false);
#endif
    // opérations de debuggage.
    case DEBUG: return (lmc_hal.dbg.opcode = lmc_hal.mem.cache.wr);
    case CONT:  lmc_hal.dbg.opcode = lmc_hal.mem.cache.wr; return false;
    case NEXT:  break;
    case BREAK: lmc_hal.dbg.brk = lmc_hal.mem.cache.wr; break;
    case FREE:  lmc_hal.dbg.brk = 0; break;
    case PRINT: lmc_hal.dbg.prt = lmc_hal.mem.cache.wr; break;
    case CLEAR: lmc_hal.dbg.prt = 0; break;
    case DUMP:
        lmc_busInput();
        lmc_dump(lmc_hal.mem.cache.wr, lmc_hal.bus.buffer);
        break;
    default:
        // Pour _UCODES où la variable est inutilisée.
        (void) value;
        break;
    }
    return true;
}

static void lmc_calc(void)
{
    switch(lmc_hal.alu.opcode) {
    case ADD:  lmc_hal.alu.acc += lmc_hal.mem.cache.wr; break;
    case SUB:  lmc_hal.alu.acc -= lmc_hal.mem.cache.wr; break;
    case NAND: lmc_hal.alu.acc = !(lmc_hal.alu.acc && lmc_hal.mem.cache.wr); break;
    default: break;
    }
}

static void lmc_rwMemory(LmcRam address, LmcRam* value, char mode)
{
    switch (mode) {
    // On ne vérifie pas les adresses en dehors de la RAM ici car
    // LmcRam ne peut prendre de valeur en dehors. On est donc
    // toujours assuré d'avoir une adresse "dans les clous" (ce
    // qui ne veut pas dire correcte).
    case 'r': *value = lmc_hal.mem.ram[address]; break;
    case 'w':
        // On simule une erreur d'écriture dans la ROM.
        if (address < LMC_MAXROM) {
            lmc_hal.on         = false;
            errno              = EFAULT;
            warn(LMC_HEXFMT ": read only", LMC_MAXDIGITS, address);
            return;
        }
        lmc_hal.mem.ram[address] = *value;
        break;
    default: break;
    }
}

#ifdef _UCODES

// clang-format off

/******************************************************************************
 * Gestion des microcodes
 ******************************************************************************/
// clang-format on

static void lmc_useries(unsigned int ucode, ...)
{
    va_list ucodes;
    va_start(ucodes, ucode);
    do { lmc_ucode((LmcUcodes)ucode); }
    while ((ucode = va_arg(ucodes, unsigned int)) > 0);
    va_end(ucodes);
}

static void lmc_ucode(LmcUcodes ucode)
{
    // On utilise des fonctions pour WRTOOU, DOCALC, SVTOWR, WRTOSV,
    // et WINPUT pour plusieurs raisons:
    //
    // - permettre d'accomoder le code aux deux versions du logiciel
    //   (avec/sans microcodes), puisque ces opérations sont communes
    //   aux deux;
    // - permettre à WINPUT de gérer plusieurs sources d'input
    //   possibles (utilisateur/fichier compilé);
    // - émuler la capacité de lecture seule de la mémoire morte;
    // - séparer les erreurs de segmentation dûes au code de
    //   l'ordinateur lui-même des erreurs des programmes qui lui sont
    //   fournis -- bien qu'un debugger suffirait, cette
    //   fonctionnalité accélère le processus et le rend moins
    //   complexe;

    switch (ucode) {
    case PCTOSR: lmc_hal.mem.cache.sr = lmc_hal.cu.pc; break;      // 1
    case WRTOPC: lmc_hal.cu.pc = lmc_hal.mem.cache.wr; break;      // 2, skip phase suivante
    case WRTOAC: lmc_hal.alu.acc = lmc_hal.mem.cache.wr; break;    // 3
    case ACTOWR: lmc_hal.mem.cache.wr = lmc_hal.alu.acc; break;    // 4
    case WRTOOP: lmc_hal.alu.opcode = lmc_hal.mem.cache.wr; break; // 5
    case WRTOAD: lmc_hal.cu.ir.ad = lmc_hal.mem.cache.wr; break;   // 6
    case ADTOSR: lmc_hal.mem.cache.sr = lmc_hal.cu.ir.ad; break;   // 7
    case INTOWR: lmc_hal.mem.cache.wr = lmc_hal.bus.buffer; break; // 8
    case WRTOOU: lmc_busOutput(LMC_WRDVAL); break;                    // 9
    case ADDOPD: lmc_hal.alu.opcode = ADD; break;                     // 10
    case SUBOPD: lmc_hal.alu.opcode = SUB; break;                     // 11
    case DOCALC: lmc_calc(); break;                                   // 12
    case SVTOWR: __attribute__((fallthrough)); // 13
    case WRTOSV:                               // 14
        lmc_rwMemory(
            lmc_hal.mem.cache.sr,
            &lmc_hal.mem.cache.wr,
            ucode == SVTOWR ? 'r' : 'w'
        );
        break;
    case INCRPC: ++lmc_hal.cu.pc; break;           // 15
    case WINPUT: lmc_busInput(); break;            // 16
    case NANDOP: lmc_hal.alu.opcode = NAND; break; // 17
    case LMCHLT: lmc_hal.on = false; break;        // 18, extinction
    default: break;
    }
}

#endif // _UCODES
