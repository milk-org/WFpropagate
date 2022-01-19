/**
 * @file    WFpropagate.c
 * @brief   Optical propagation of wavefronts
 */

#define MODULE_SHORTNAME_DEFAULT "wfprop"
#define MODULE_DESCRIPTION "Optical propagation of wavefronts"

#include "CommandLineInterface/CLIcore.h"

#include "Fresnel_propagate.h"
// #include "Fresnel_propagate_cube.h"

INIT_MODULE_LIB(WFpropagate)

static errno_t init_module_CLI()
{

    CLIADDCMD_Fresnel_propagate_wavefront();

    return RETURN_SUCCESS;
}
