/**
 * @file      common.h
 * @version   0.1.0
 * @brief     Structures communes aux tests unitaires.
 * @year      2023
 * @author    Alexandre Martos
 * @email     contact@amartos.fr
 * @copyright GNU General Public License v3
 * @addtogroup UnitsTests
 * @{
 */

#ifndef LMC_COMMON_H_
#define LMC_COMMON_H_

// Librairie personnelle de tests unitaires
#include <sccroll.h>

/**
 * @def MANUALIN
 * @since 0.1.0
 * @brief Programme de test entré via STDIN.
 *
 * Il y a un total de 20 instructions.
 * La traduction des codes est la suivante:
 * position 0x30, taille 0x12
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
 * @brief Output prévu sur STDOUT pour la programmation en temps réel
 * de #MANUALIN.
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
 * @name Chemins des programmes de tests pour l'ordinateur.
 * @{
 */

/**
 * @def PROGS
 * @since 0.1.0
 * @brief Chemin du dossier contenant les programmes de tests.
 */
#define PROGS "tests/assets/programs/"

/**
 * @def UNDEFINED
 * @since 0.1.0
 * @brief Programme de test inexistant.
 */
#define UNDEFINED "foobar"

/**
 * @def _BOOTSTRAP
 * @since 0.1.0
 * @brief Programme compilé du bootstrap.
 * @param ... Insère un préfixe au nom du fichier.
 */
#define _BOOTSTRAP(...) PROGS __VA_ARGS__ "bootstrap"

/**
 * @def BOOTSTRAP
 * @since 0.1.0
 * @brief Programme compilé du bootstrap de base.
 */
#define BOOTSTRAP _BOOTSTRAP()

/**
 * @def ALTBOOTSTRAP
 * @since 0.1.0
 * @brief Programme compilé d'un bootstrap de tests.
 */
#define ALTBOOTSTRAP _BOOTSTRAP("alt")

/**
 * @def PRODUCT
 * @since 0.1.0
 * @brief Programme calculant le produit de deux entiers.
 */
#define PRODUCT PROGS "product"

/**
 * @def QUOTIENT
 * @since 0.1.0
 * @brief Programme calculant le quotient d'une division euclidienne.
 */
#define QUOTIENT PROGS "quotient"

/**
 * @def CMDLINE
 * @since 0.1.0
 * @brief Valeur indiquant d'entrer en mode interactif.
 */
#define CMDLINE NULL

/**
 * @def MALFORMED
 * @since 0.1.0
 * @brief Programme malformé.
 */
#define MALFORMED PROGS "error"

/**
 * @def DUMMY
 * @since 0.1.0
 * @brief Programme ne faisant rien de concrêt.
 *
 * Le code de ce programme implémente quelques syntaxes analysées par
 * le compilateur. Le but est de vérifier que ces syntaxes sont bien
 * prises en compte.
 */
#define DUMMY PROGS "dummy"

/**
 * @def DUMMYCODE
 * @since 0.1.0
 * @brief Code compilé attendu pour le programme #DUMMY.
 */
#define DUMMYCODE "\xab\x0e\x20\x23\x40\x53\xd0\xaf\x09\x02\x41\xff\x40\x00\x12\x56"

/**
 * @def DUMMYCODELEN
 * @since 0.1.0
 * @brief Nombre de codes contenus par la chaîne DUMMYCODE.
 */
#define DUMMYCODELEN (strlen(DUMMYCODE)+3)
/** @} */

/**
 * @name Simulacres
 *
 * Les simulacres définis ici sont ceux qui ne sont pas encore inclus
 * dans ceux fournis par la librairie Sccroll. À terme ces simulacres
 * y seront transférés (et cette section sera donc supprimée).
 * @{
 */

/**
 * @enum LmcMocks
 * @since 0.1.0
 * @brief Code de déclenchement d'un simulacre donné.
 *
 * Les informations des codes indiquent le nom de la fonction
 * remplacée.
 */
typedef enum LmcMocks {
    ERRFSCANF = 1, /**< fscanf */
    ERRFOPEN,      /**< fopen */
    ERRFREAD,        /**< fread */
    ERRFWRITE,       /**< fwrite */
    ERRHCREATE,      /**< hcreate */
    ERRHSEARCH,      /**< hsearch */
} LmcMocks;

/**
 * @struct LmcMockTrigger
 * @since 0.1.0
 * @brief Structure permettant d'indiquer à un simulacre quand entrer
 * en erreur.
 */
typedef struct LmcMockTrigger {
    LmcMocks errnum; /**< Code du simulacre à déclencher. */
    int delay;       /**< Nombre d'appel du simulacre à ignorer. */
} LmcMockTrigger;

/**
 * @def lmc_mockErr
 * @since 0.1.0
 * @brief Vérifie si le simulacre correspondant à @p name et @p code
 * doit entrer en erreur.
 * @param name Le nom de la fonction.
 * @param code Le code LmcMocks correspondant.
 * @param retval La valeur retournée par @p name en cas d'erreur.
 * @param ... Les arguments pour @p name.
 * @return @p retval en cas de déclenchement du simulacre, sinon la
 * valeur retournée par @p name.
 */
#define lmc_mockErr(name, code, retval, ...)                            \
    (code == trigger.errnum && !trigger.delay--)                        \
    ? retval                                                            \
    : __real_##name(__VA_ARGS__)
/** @} */

#endif // LMC_COMMON_H_
