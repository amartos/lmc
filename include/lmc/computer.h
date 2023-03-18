/**
 * @file      computer.h
 * @version   0.1.0
 * @brief     Module principal de l'ordinateur en papier.
 * @year      2022
 * @author    Alexandre Martos
 * @email     alexandre.martos (at) protonmail.ch
 * @copyright GNU General Public License v3
 * @compilation
 * flex lmc.lang.lex -o lexlang.c
 * gcc -Wall -std=gnu99 computer.c -o computer.o
 * @addtogroup Computer
 * @{
 */

#ifndef LMC_COMPUTER_H_
#define LMC_COMPUTER_H_

#include <err.h>     // pour les fonctions de gestion d'erreur
#include <errno.h>   // pour les codes errno
#include <stdbool.h> // pour le type bool
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// clang-format off

/******************************************************************************
 * @name La mémoire
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @typedef LmcRam
 * @since 0.1.0
 * @brief Type de donnée pour les adresses de la mémoire.
 */
// char est déjà non signé, mais on appuie le fait, ici.
typedef unsigned char LmcRam;

/**
 * @enum LmcMemoryCaracs
 * @since 0.1.0
 * @brief Caractéristiques numériques de la mémoire de l'ordinateur.
 */
typedef enum LmcMemoryCaracs {
    LMC_MAXRAM    = 0x100,              /**< Quantité de mémoire maximale (octets). */
    LMC_MAXROM    = 0x20,               /**< Quantité de mémoire en lecture seule (octets). */
    LMC_MAXDIGITS = sizeof(LmcRam) * 2, /**< Nombre de chiffres des valeurs de la mémoire. */
    LMC_SIGN      = LMC_MAXRAM >> 1,    /**< Masque pour le bit de signe. */
} LmcMemoryCaracs;

/**
 * @def LMC_HEXFMT
 * @since 0.1.0
 * @brief Le format utilisé pour les nombres hexadécimaux affichés.
 * @param * LMC_MAXDIGITS
 */
#define LMC_HEXFMT "%0*x"

/**
 * @struct LmcMemory
 * @since 0.1.0
 * @brief Structure de la mémoire de la machine.
 */
typedef struct LmcMemory {
    struct {
        LmcRam wr;          /**< Le registre mot. */
        LmcRam sr;          /**< Le registre de sélection. */
    } cache;                /**< Le cache de la mémoire. */
    LmcRam ram[LMC_MAXRAM]; /**< Le stockage mémoire. */
} LmcMemory;

// clang-format off

/******************************************************************************
 * @}
 * @name L'unité de contrôle
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcControlUnit
 * @since 0.1.0
 * @brief La structure de l'unité de contrôle de l'ordinateur.
 */
typedef struct LmcControlUnit {
    struct {
        LmcRam op; /**< Le registre de code opératoire. */
        LmcRam ad; /**< Le registre d'adresse. */
    } ir;          /**< Le registre d'instruction. */
    LmcRam pc;     /**< Le registre 'Program Counter'. */
} LmcControlUnit;

// clang-format off

/******************************************************************************
 * @}
 * @name L'unité arithmétique et logique
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcLogicUnit
 * @since 0.1.0
 * @brief La structure de l'unité arithmétique et logique.
 */
typedef struct LmcLogicUnit {
    LmcRam acc;    /**< L'accumulateur. */
    LmcRam opcode; /**< Le registre pour les codes d'opération. */
} LmcLogicUnit;

// clang-format off

/******************************************************************************
 * @}
 * @name Le bus et les Entrées/Sorties
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct Bus
 * @since 0.1.0
 * @brief La structure contenant les entrées/sorties.
 */
typedef struct LmcBus {
    FILE* input;        /**< L'entrée. */
    FILE* output;       /**< La sortie. */
    const char* prompt; /**< L'invite de commande. */
    LmcRam buffer;      /**< Un buffer entre les E/S et la mémoire. */
    bool newline;       /**< Drapeau indiquant s'il faut afficher un
                         * caractère de fin de ligne avant l'output à
                         * venir. */
} LmcBus;

// clang-format off

/******************************************************************************
 * @}
 * @name L'ordinateur en (e-)papier
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @struct LmcComputer
 * @since 0.1.0
 * @brief Structure complète de l'ordinateur.
 */
typedef struct LmcComputer {
    LmcMemory mem;     /**< La mémoire de l'ordinateur. */
    LmcControlUnit cu; /**< L'unité de contrôle. */
    LmcLogicUnit alu;  /**< L'unité arthmétique et logique. */
    LmcBus bus;        /**< Le bus de communication avec le monde extérieur. */
    bool on;           /**< Drapeau indiquant si l'ordinateur est allumé. */
} LmcComputer;

// clang-format off

/******************************************************************************
 * @}
 * @name Fonctions principales de l'ordinateur
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @since 0.1.0
 * @brief Exécute un programme.
 * @param filepath Le chemin d'un fichier compilé à exécuter, ou NULL
 * pour entrer en mode de programmation interactive.
 * @return La valeur du registre mot à l'extinction.
 */
LmcRam lmc_shell(const char* restrict filepath);

// clang-format off

/******************************************************************************
 * @}
 * @name Programmation de l'ordinateur.
 * @{
 ******************************************************************************/
// clang-format on

/**
 * @enum LmcOpCodes
 * @since 0.1.0
 * @brief Les codes opératoires de l'ordinateur en papier.
 */
typedef enum __attribute__((__packed__)) LmcOpCodes {
    // Primitives
    INV = 0x01, /**< Inverse la valeur/signification. */
    NOT = 0x02, /**< Donne le NON booléen de la valeur. */
    HLT = 0x04, /**< Éteint l'ordinateur jusqu'au prochain programme. */
    WRT = 0x08, /**< Écrit une valeur en mémoire. */
    JMP = 0x10, /**< Change l'emplacement mémoire courant. */
    ADD = 0x20, /**< Addition. */
    VAR = 0x40, /**< La valeur est une adresse de la mémoire. */
    PTR = 0x80, /**< La valeur est un pointeur. */

    // Combinaisons
    INDIR = VAR | PTR, /**< Déréférence un pointeur. */

    // Instructions
    // les !WRT sont inutiles ici, mais sont laissés pour montrer la
    // correspondant de la valeur.
    SUB   = ADD | INV,  /**< Opération de soustraction. */
    NAND  = ADD | NOT,  /**< Opération booléenne NON ET. */
    LOAD  = !WRT,       /**< Charge une valeur dans l'accumulateur. */
    STORE = WRT,        /**< Stocke une valeur en mémoire. */
    IN    = WRT | INV,  /**< Enregistre une valeur venant de l'utilisateur. */
    OUT   = !WRT | INV, /**< Affiche une valeur sur le tampon de sortie. */
    JUMP  = JMP,        /**< Saute vers une adresse (alias de #JMP).*/
    BRN   = JMP | INV,  /**< Saute si l'accumulateur est négatif. */
    BRZ   = JMP | NOT,  /**< Saute si l'accumulateur est nul.*/
} LmcOpCodes;

/**
 * @def LMC_PROGRAMMING
 * @since 0.1.0
 * @brief Description générale de la programmation de l'ordinateur.
 */
#define LMC_PROGRAMMING                                                                    \
    "L'ordinateur est programmé à l'aide de codes opératoires accompagnés d'une\n"     \
    "valeur servant d'argument. Il est généralement considéré (surtout pour le\n"      \
    "debugger, voir la section ci-dessous) que le code d'opération est stocké dans\n"    \
    "un emplacement d'index pair. Dans tous les cas, sa valeur-argument **doit**\n"        \
    "être placée dans l'emplacement directement suivant (donc l'index +1).\n"            \
    "\n"                                                                                   \
    "Les codes d'opération supportés sont décrits dans le tableau ci-dessous.\n"        \
    "Toutes les valeurs indiquées à l'ordinateur **doivent** être en base\n"            \
    "hexadécimale (code opératoire ET argument). Si une entrée est demandée à\n"      \
    "l'utilisateur par un programme chargé dans la machine, il est attendu que\n"         \
    "cette valeur soit aussi en base hexadécimale. Le programme ne s'attend pas au\n"     \
    "format 0x00, cependant, et n'attend qu'un nombre à un ou plus chiffres.\n"           \
    "\n"                                                                                   \
    "La programmation de l'ordinateur peut se faire de manière interactive (il\n"         \
    "suffit d'appeler le programme sans arguments), mais également au moyens de\n"        \
    "fichiers de codes. L'en-tête de ces fichiers doit **impérativement** contenir\n"    \
    "deux nombres (en hexadécimal) correspondants, **dans l'ordre**, à:\n"               \
    "  - l'emplacement mémoire de chargement de la première instruction\n"               \
    "  - la taille totale du programme en nombre d'octets.\n"                              \
    "\n"                                                                                   \
    "Si une liste de fichiers est passée à l'ordinateur, celui-ci chargera et\n"         \
    "exécutera un à un les programmes. Il ne s'arrêtera que si une erreur survient\n"   \
    "dans un programme (et renverra le code renvoyé par le programme), ou si tous\n"      \
    "les programmes ont été exécutés correctement.\n"                                  \
    "\n"                                                                                   \
    "L'ordinateur comprend deux \"langages\" de programmation: les codes\n"                \
    "hexadécimaux des opérations, ou les symboles correspondants (cf. la table des\n"    \
    "codes pour la syntaxe). La séparation entre les codes / symboles et leurs\n"         \
    "arguments, et les autres couples codes/symboles/arguments, se fait avec des\n"        \
    "espaces, tabulation ou saut de ligne. Les portions de texte (en début ou pas\n"      \
    "de ligne) précédées de deux (ou plus) caractères '/'\n"                           \
    "(soit \"00 00 // texte ignoré\") indiquent que le reste de la ligne est un\n"        \
    "commentaire qui sera ignoré. Les commentaires multilignes (\"/* commentaire */\")\n" \
    "ne sont pas supportés."

/**
 * @def LMC_OPCODES
 * @since 0.1.0
 * @brief Description des codes opératoires de la machine.
 */
#define LMC_OPCODES                                                                 \
    "Dans la table suivante:\n"                                                     \
    "  -   A  désigne l'accumulateur de l'UAL\n"                                   \
    "  -   V  désigne l'argument du code opératoire\n"                            \
    "  -  (V) désigne le contenu de l'espace d'adresse mémoire V\n"              \
    "  - *(V) désigne et *(V) le contenu de l'espace d'adresse mémoire\n"         \
    "         dont l'index est contenu dans (V) (donc un pointeur)\n"               \
    "  - Y = X indique l'assignation (la copie de valeur) de X dans Y\n"            \
    "  - +- indiquent une addition/soustraction\n"                                  \
    "  - ! désigne le NOT logique, && le AND, == le égal, < le 'inférieur à'\n" \
    "  - <= et => désignent la communication entre le bus (stdout en sortie,\n"    \
    "    stdin en entrée) et la mémoire\n"                                        \
    "\n"                                                                            \
    "La syntaxe des SYMBOLEs est utilisable pour la programmation de\n"             \
    "l'ordinateur (V représentant toujours la valeur-argument). Cette syntaxe\n"   \
    "n'est pas sensible à la casse. La syntaxe 'symbole *@' est synonme de\n"      \
    "'symbole *'.\n"                                                                \
    "\n"                                                                            \
    "---------------------------------------\n"                                     \
    "SYMBOLE   | CODE | OPÉRATION EFFECTUÉE \n"                                   \
    "---------------------------------------\n"                                     \
    "add V     | 20   | A = A + V           \n"                                     \
    "add @ V   | 60   | A = A + (V)         \n"                                     \
    "add * V   | e0   | A = A + *(V)        \n"                                     \
    "sub V     | 21   | A = A - V           \n"                                     \
    "sub @ V   | 61   | A = A - (V)         \n"                                     \
    "sub * V   | e1   | A = A - *(V)        \n"                                     \
    "nand V    | 22   | A = !(A && V)       \n"                                     \
    "nand @ V  | 62   | A = !(A && (V))     \n"                                     \
    "nand * V  | e2   | A = !(A && *(V))    \n"                                     \
    "load V    | 00   | A = V               \n"                                     \
    "load @ V  | 40   | A = (V)             \n"                                     \
    "load * V  | c0   | A = *(V)            \n"                                     \
    "store @ V | 48   | (V) = A             \n"                                     \
    "store * V | c8   | *(V) = A            \n"                                     \
    "in @ V    | 49   | (V) <= (stdin)      \n"                                     \
    "in * V    | c9   | *(V) <= (stdin)     \n"                                     \
    "out @ V   | 41   | (V) => (stdout)     \n"                                     \
    "out * V   | c1   | *(V) => (stdout)    \n"                                     \
    "jump V    | 10   | PC = V              \n"                                     \
    "jump @ V  | 50   | PC = (V)            \n"                                     \
    "jump * V  | d0   | PC = *(V)           \n"                                     \
    "brn V     | 11   | si A < 0, PC = V    \n"                                     \
    "brn @ V   | 51   | si A < 0, PC = (V)  \n"                                     \
    "brn * V   | d1   | si A < 0, PC = *(V) \n"                                     \
    "brz V     | 12   | si A == 0, PC = V   \n"                                     \
    "brz @ V   | 52   | si A == 0, PC = (V) \n"                                     \
    "brz * V   | d2   | si A == 0, PC = *(V)\n"                                     \
    "stop V    | 04   | éteint et renvoie V \n"                                    \
    "---------------------------------------\n"                                     \
    "\n"

// clang-format off
/******************************************************************************
 * @}
 ******************************************************************************/
// clang-format on

#endif // LMC_COMPUTER_H_
/** @} */
