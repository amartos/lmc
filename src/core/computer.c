/**
 * @file       computer.c
 * @version    0.1.0
 * @brief      The LMC core module.
 * @author     Alexandre Martos
 * @email      contact@amartos.fr
 * @copyright  2023 Alexandre Martos <contact@amartos.fr>
 * @license    GPLv3
 *
 * @addtogroup ComputerInternals Structures internes de l'ordinateur.
 * @{
 */

#include "lmc/computer.h"

// clang-format off

/******************************************************************************
 * @name Execution
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Execute a compiled program with or without the debugger.
 * @param bootstrap The compiled bootstrap file path.
 * @param filepath The file path of the compiled program. @c NULL
 * switches to interactive mode (manual programmation).
 * @param debug Use the debugger if @c true.
 * @return The word register value at shutdown.
 */
static LmcRam lmc_exec(const char* restrict bootstrap, const char* restrict filepath, bool debug);

/**
 * @since 0.1.0
 * @brief Load a bootstrap in #lmc_hal::mem::ram ROM section.
 *
 * This function may raise a fatal error if @p path cannot be
 * @c rb opened.
 *
 * @param path The compiled bootstrap file path.
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
 * @brief The debug mode prompt outputted on #lmc_hal::bus::output.
 */
#define LMC_DBGPROMPT                                       \
    "PC: " LMC_HEXFMT ", ACC: " LMC_HEXFMT " " LMC_PROMPT,  \
        LMC_MAXDIGITS, lmc_hal.cu.pc,                       \
        LMC_MAXDIGITS, lmc_hal.alu.acc

/**
 * @since 0.1.0
 * @brief Step in the debugger.
 * @return @c true to execute the next program instruction, @c false
 * to immediately re-step in the debugger.
 */
static bool lmc_debug(void);

/**
 * @since 0.1.0
 * @brief Debugger phase 1.
 *
 * Print the current PC address value if it is stored in
 * #lmc_hal::dbg::prt and indicate if the computer must go into the
 * debugger second phase.
 *
 * @return @c false to skip the next debug phase, @c true to execute
 * it.
 */
static bool lmc_dbg_phaseOne(void);

/**
 * @since 0.1.0
 * @brief Debugger phase 2.
 *
 * Print #lmc_hal::cu::pc and #lmc_hal::alu:acc, then wait for
 * instructions input.
 */
static void lmc_dbg_phaseTwo(void);

/**
 * @since 0.1.0
 * @brief Debugger phase 3.
 *
 * Execute the instructions input at phase 2.
 *
 * @return @c true to immediately re-step in the debugger, @c false to
 * continue the program execution.
 */
static bool lmc_dbg_phaseThree(void);

/**
 * @since 0.1.0
 * @brief Print all the values between two #lmc_hal::mem::ram
 * addresses.
 * @param start,end The memory start and end address for the dump.
 */
static void lmc_dump(LmcRam start, LmcRam end);

// clang-format off

/******************************************************************************
 * @}
 * @name LMC phases.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief LMC Phase 1: seek for the next instruction.
 */
static void lmc_phaseOne(void);

/**
 * @since 0.1.0
 * @brief LMC phase 2: decode the instruction, seek the operand, and
 * apply the instruction.
 * @param debug Indicate if the phase is executed from the debugger.
 * @return @c true to execute phase 3, @c false to skip it.
 */
static bool lmc_phaseTwo(bool debug);

/**
 * @since 0.1.0
 * @brief LMC phase 3: increment PC.
 */
static void lmc_phaseThree(void);

// clang-format off

/******************************************************************************
 * @}
 * @name IO management.
 *
 * These functions and macros are used for interaction of the LMC with
 * the external world through the bus.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @name Conversion macros
 * @{
 * @param v The value to convert.
 * @return The value of @p v as a string.
 */

/**
 * @def _TOSTR
 * @since 0.1.0
 * @bried This macro directly converts the given value to a string.
 */
#define _TOSTR(v) #v

/**
 * @def TOSTR
 * @since 0.1.0
 * @bried This macro converts the @p v expression result to a string.
 */
#define TOSTR(v) _TOSTR(v)
/** }@ */

/**
 * @def LMC_PROMPT
 * @since 0.1.0
 * @brief The default #lmc_hal::bus::input prompt.
 */
#define LMC_PROMPT "? >"

/**
 * @def LMC_WRDVAL
 * @since 0.1.0
 * @brief #lmc_hal::mem::cache::wr value printable using a printf()
 * family function (no arguments needed).
 */
#define LMC_WRDVAL LMC_HEXFMT, LMC_MAXDIGITS, lmc_hal.mem.cache.wr

/**
 * @since 0.1.0
 * @brief Wait for input from #lmc_hal::bus::input and store it in
 * #lmc_hal::bus::buffer.
 */
static void lmc_busInput(void);

/**
 * @since 0.1.0
 * @brief Handle bus input.
 *
 * @attention This function may raise a fatal error if @p filepath
 * cannot be @c rb opened.
 *
 * @param filepath A compiled program file path, or @c NULL for user
 * direct input.
 * @return @c false if @p filepath is @c NULL and user sends @c EOF,
 * (thus a "QUIT" signal) otherwise @p true.
 */
static bool lmc_setInput(const char* restrict filepath);

/**
 * @since 0.1.0
 * @brief Store in #lmc_hal::bus::buffer a converted number given as a
 * string.
 * @param number The number stored as a string.
 * @return @c EXIT_FAILURE in case of errors, otherwise @c EXIT_SUCCESS.
 */
static int lmc_convert(const char* restrict number) __attribute__((nonnull));

/**
 * @def lmc_busPrint
 * @since 0.1.0
 * @brief Print a formatted message on #lmc_hal::bus::output.
 * @param fmt The format string.
 * @param ... The format string arguments.
 */
#define lmc_busOutput(fmt, ...) fprintf(lmc_hal.bus.output, fmt, ##__VA_ARGS__)

// clang-format off

/******************************************************************************
 * @}
 * @name Operations
 * @{
 ******************************************************************************/
// clang-format on

/**
 * since 0.1.0
 * @brief Check if #lmc_hal::alu::opcode value must change.
 * @param operation The operation bytecode without indirection
 * instructions.
 */
static void lmc_opcalc(LmcRam operation);

/**
 * @since 0.1.0
 * @brief Fetch the value of the current #lmc_hal::mem::cache::sr
 * address, applying the indicated indirection level.
 * @param type The indirection level (@c 0, #VAR or #VAR|#PTR).
 */
static void lmc_indirection(LmcRam type);

/**
 * @since 0.1.0
 * @brief Execute an operation.
 * @param operation The operation bytecode without indirection
 * instructions.
 */
static bool lmc_operation(LmcRam operation);

/**
 * @since 0.1.0
 * @brief Execute the arithmetic instruction store in
 * #lmc_hal::alu::opcode with the #lmc_hal::mem::cache::wr and
 * #lmc_hal::alu::acc operands.
 */
static void lmc_calc(void);

/**
 * @since 0.1.0
 * @brief Read/Write in #lmc_hal::mem::ram.
 *
 * This function checks that the address and operation are valid,
 * i.e. that ROM is read-only and RAM is read-write. The function
 * raises an @c EFAULT error if not and cleanly shutdowns the
 * computer.
 *
 * @param address The read memory address, or write destination address.
 * @param value The storage destination for read mode, the source
 * value for write mode.
 * @param mode The mode, either 'r' for read or 'w' for write.
 */
static void lmc_rwMemory(LmcRam address, LmcRam* value, char mode) __attribute__((nonnull (2)));

#ifdef _UCODES

// clang-format off

/******************************************************************************
 * @}
 * @name Microcodes
 *
 * This section is optional, can optionally be loaded at compile-time
 * for a deeper level of emulation, by defining the #_UCODES macro.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum LmcUcodes
 * @since 0.1.0
 * @brief The microcodes.
 */
typedef enum __attribute__ ((__packed__)) LmcUcodes {
    PCTOSR = 1, /**< 01 Write #lmc_hal::cu:pc in #lmc_hal::mem::cache::sr. */
    WRTOPC,     /**< 02 Write #lmc_hal::mem::cache::wr in #lmc_hal::cu::pc. */
    WRTOAC,     /**< 03 Write #lmc_hal::mem::cache::wr in #lmc_hal::alu::acc. */
    ACTOWR,     /**< 04 Write #lmc_hal::alu::acc in #lmc_hal::mem::cache::wr. */
    WRTOOP,     /**< 05 Write #lmc_hal::mem::cache::wr in #lmc_hal::alu::opcode. */
    WRTOAD,     /**< 06 Write #lmc_hal::mem::cache::wr in #lmc_hal::cu::ir::ad. */
    ADTOSR,     /**< 07 Write #lmc_hal::cu::ir::ad in #lmc_hal::mem::cache::sr. */
    INTOWR,     /**< 08 Write #lmc_hal::bus::buffer in #lmc_hal::mem::cache::wr. */
    WRTOOU,     /**< 09 Write #lmc_hal::mem::cache::wr in #lmc_hal::bus::output. */
    ADDOPD,     /**< 10 Write #ADD in #lmc_hal::alu::opcode. */
    SUBOPD,     /**< 11 Write #SUB in #lmc_hal::alu::opcode. */
    DOCALC,     /**< 12 Execute the instruction stored in #lmc_hal::alu::opcode. */
    SVTOWR,     /**< 13 Write the memory slot value pointed by #lmc_hal::mem::cache::sr in #lmc_hal::mem::cache::wr. */
    WRTOSV,     /**< 14 Write #lmc_hal::mem::cache::wr in the memory slot pointed by #lmc_hal::mem::cache::sr. */
    INCRPC,     /**< 15 Increment #lmc_hal::cu::pc. */
    WINPUT,     /**< 16 Wait for input in #lmc_hal::bus::input. */
    NANDOP,     /**< 17 Write #NAND in #lmc_hal::alu::opcode. */
    LMCHLT,     /**< 18 Set #lmc_hal::on to @c false. */
} LmcUcodes;

/**
 * @since 0.1.0
 * @brief Execute a series of microcodes operations.
 * @attention Needs a @c NULL sentinel value.
 * @param ucode A microcode.
 * @param ... The remaining microcodes with a @c NULL as last
 * argument.
 */
static void lmc_useries(unsigned int ucode, ...) __attribute__((sentinel));

/**
 * @since 0.1.0
 * @brief Execute one microcode operation.
 * @param ucode The microcode.
 */
static void lmc_ucode(LmcUcodes ucode);

#endif // _UCODES

// clang-format off

/******************************************************************************
 * @}
 * @name The LMC.
 * @{
 ******************************************************************************/

/**
 * @var lmc_hal
 * @since 0.1.0
 * @brief The LMC computer.
 */
static LmcComputer lmc_hal = {0};

/**
 * @var lmc_template
 * @since 0.1.0
 * @brief A template for #lmc_hal, including a default bootstrap.
 */
static const LmcComputer lmc_template = {
    .bus = { .prompt = LMC_PROMPT, },
    .mem.ram = {
        // The bootstrap
        // operation    argument adress  instruction translation (base 16)
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
        // The last instruction is at the end of the ROM, the slots
        // in-between are set to 0. This instruction does not have a
        // preset argument, as the latter is the program start address
        // given by the program header. This instruction is in ROM,
        // while the address is in RAM.
        [LMC_MAXROM-1] = JUMP,
    },
};


/******************************************************************************
 * @}
 * Implementation
 ******************************************************************************/
// clang-format on

LmcRam lmc_shell(const char* restrict bootstrap, const char* restrict filepath)
{ return lmc_exec(bootstrap, filepath, false); }

LmcRam lmc_dbgShell(const char* restrict bootstrap, const char* restrict filepath)
{ return lmc_exec(bootstrap, filepath, true); }

static LmcRam lmc_exec(const char* restrict bootstrap, const char* restrict filepath, bool debug)
{
    // reset the computer to avoid mixing data between the programs.
    lmc_hal = lmc_template;
    lmc_bootstrap(bootstrap);

    // FILE* stdin and stdout are not compile-time constants, thus
    // only assignable at run-time.
    lmc_hal.bus.input  = stdin;
    lmc_hal.bus.output = stdout;
    lmc_setInput(filepath);

    lmc_hal.on = true; // Hello Dave. You are looking well today.
    lmc_hal.dbg.opcode = debug ? DEBUG : 0;
    while (lmc_hal.on) {
        while(lmc_debug());
        lmc_phaseOne(), lmc_phaseTwo(false) ? lmc_phaseThree() : 0;
    }
    return lmc_hal.mem.cache.wr;
}

static void lmc_bootstrap(const char* restrict path)
{
    FILE* file = fopen(path, "rb");
    size_t size = LMC_MAXROM;
    size_t final = 0;

    // As the bootstrap is itself compiled using the LMC compiler, the
    // first two values are the start position and the size. The first
    // is ignored, and the second is checked before loading the
    // bootstrap in memory (in case of discrepancy or if the given
    // bootstrap is larger than the ROM).
    if (!file || fseek(file, sizeof(LmcRam), SEEK_SET))
        err(EXIT_FAILURE, "%s: could not load bootstrap", path);
    else if (fread(&size, sizeof(LmcRam), 1, file) < 1) {
        errno = ENOEXEC;
        err(EXIT_FAILURE, "%s: missing size for bootstrap header", path);
    }
    else if (size > LMC_MAXROM) {
        errno = EFBIG;
        err(
            EXIT_FAILURE,
            "The bootstrap size (%lu bytes) is larger than the ROM (%i bytes)",
            size, LMC_MAXROM
        );
    }
    else if (size == 0) {
        errno = ECANCELED;
        warn("%s: the bootstrap indicated size is null", path);
        fprintf(stderr, "Fallback to default bootstrap\n");
    }
    else if ((final = fread(lmc_hal.mem.ram, sizeof(LmcRam), LMC_MAXROM, file)) < size)
        err(
            EXIT_FAILURE,
            "%s: header size (%lu bytes) differs from total read (%lu bytes)",
            path, size, final
        );
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
    if (!(lmc_hal.on && lmc_hal.dbg.opcode))
        return false;

    if (lmc_hal.dbg.prt // we don't want to print the value of address 0x00.
        && lmc_hal.dbg.prt == lmc_hal.cu.pc)
        lmc_dump(lmc_hal.cu.pc, lmc_hal.cu.pc);

    if (lmc_hal.dbg.opcode == CONT
        && lmc_hal.dbg.brk // ibid, skip address 0x00
        && lmc_hal.cu.pc != lmc_hal.dbg.brk)
        return false;

    return true;
}

static void lmc_dbg_phaseTwo(void)
{
    // Set the special prompt for the debugger.
    char prompt[BUFSIZ] = {0};
    sprintf(prompt, LMC_DBGPROMPT);
    lmc_hal.bus.prompt = prompt;

    // The opcode is overwritten without issue because the debug phase
    // is upstream of the LMC phase 1, and the previous opcode is not
    // used anymore.
    lmc_busInput(), lmc_hal.alu.opcode   = lmc_hal.bus.buffer;
    lmc_busInput(), lmc_hal.mem.cache.wr = lmc_hal.bus.buffer;

    // Reset the prompt in case the debug instruction exits the
    // debugger.
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
 * LMC cycle.
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
    // Split the indirection instruction from the operation bytecode.
    LmcRam operation = lmc_hal.alu.opcode & ~(INDIR);
    LmcRam value     = lmc_hal.alu.opcode & INDIR;

    lmc_opcalc(operation);
    // If the caller is the debugger, fetching the operation argument
    // as usual, i.e. from the current PC address, will overwrite the
    // argument given to the debugger and stored in the word
    // register. Hence the branching to avoid this.
    if (debug) { lmc_hal.mem.cache.sr = lmc_hal.mem.cache.wr; }
    else {
#ifdef _UCODES
    lmc_ucode(PCTOSR);
#else
    lmc_hal.mem.cache.sr = lmc_hal.cu.pc;
#endif
    }
    lmc_indirection(value);
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
 * IO handling
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
    char digits[BUFSIZ+1] = { 0 };

    if (lmc_hal.bus.input == stdin) fprintf(lmc_hal.bus.output, "%s", lmc_hal.bus.prompt);

    // Instead of directly using a "%2x" format string, first fetch
    // a generic string, and then convert. This method is prefered as
    // it handles cases where the first character is an hexadecimal
    // digit but not the next one; for example if the string "foobar"
    // is given, the "%2x" value would give 0x0f instead of an error.
    eof = lmc_hal.bus.input == stdin
        ? (fscanf(lmc_hal.bus.input, "%" TOSTR(BUFSIZ) "s", (char*)digits) < 1
           || (error = lmc_convert(digits)))
        : fread(&lmc_hal.bus.buffer, sizeof(LmcRam), 1, lmc_hal.bus.input) < 1;

    if (eof) {
        if (error) {
            errno = errno ? errno : EINVAL;
            warn("Not a hexadecimal value: '%s'", digits);
        }
        else if (ferror(lmc_hal.bus.input)) warn(NULL);

        // Fallback in interactive mode if EOF or an error occurs on a
        // compiled program file. Shutdown at EOF in interactive.
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
 * Operations
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
    op_calc:   lmc_ucode(opcode); break; // Opcode 0 does nothing
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
    // Fallthrough as the indirection operations are cumulative.
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
    // Debugging instructions
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
        // In case the macro _UCODES is undefined.
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
    // LmcRam cannot have a value greater than the max size of RAM,
    // thus it is not checked. This ensures to avoid a real SIGSEGV,
    // but not a valid rw operation (due to overflow).
    case 'r': *value = lmc_hal.mem.ram[address]; break;
    case 'w':
        // Emulate a invalid write error.
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
 * Microcodes handling
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
    // Functions are used for WRTOOU, DOCALC, SVTOWR, WRTOSV, and
    // WINPUT in order to:
    //
    // - allow the two versions (with and without _UCODES) to work
    // - allow WINPUT to agnostically handle multiple input sources
    // - implement ROM protection
    // - distinguish between real SIGSEGV errors from the LMC programs
    //   errors; this is not strictly necessary in production versions
    //   of the LMC software, but greatly help during development

    switch (ucode) {
    case PCTOSR: lmc_hal.mem.cache.sr = lmc_hal.cu.pc; break;
    case WRTOPC: lmc_hal.cu.pc = lmc_hal.mem.cache.wr; break;
    case WRTOAC: lmc_hal.alu.acc = lmc_hal.mem.cache.wr; break;
    case ACTOWR: lmc_hal.mem.cache.wr = lmc_hal.alu.acc; break;
    case WRTOOP: lmc_hal.alu.opcode = lmc_hal.mem.cache.wr; break;
    case WRTOAD: lmc_hal.cu.ir.ad = lmc_hal.mem.cache.wr; break;
    case ADTOSR: lmc_hal.mem.cache.sr = lmc_hal.cu.ir.ad; break;
    case INTOWR: lmc_hal.mem.cache.wr = lmc_hal.bus.buffer; break;
    case WRTOOU: lmc_busOutput(LMC_WRDVAL); break;
    case ADDOPD: lmc_hal.alu.opcode = ADD; break;
    case SUBOPD: lmc_hal.alu.opcode = SUB; break;
    case DOCALC: lmc_calc(); break;
    case SVTOWR: __attribute__((fallthrough));
    case WRTOSV:
        lmc_rwMemory(
            lmc_hal.mem.cache.sr,
            &lmc_hal.mem.cache.wr, ucode == SVTOWR ? 'r' : 'w'
        );
        break;
    case INCRPC: ++lmc_hal.cu.pc; break;
    case WINPUT: lmc_busInput(); break;
    case NANDOP: lmc_hal.alu.opcode = NAND; break;
    case LMCHLT: lmc_hal.on = false; break;
    default: break;
    }
}

#endif // _UCODES
