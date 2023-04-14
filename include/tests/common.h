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
