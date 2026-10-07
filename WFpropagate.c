// SPDX-FileCopyrightText: 2026 Olivier Guyon et al
//
// SPDX-License-Identifier: LGPL-3.0-or-later

/**
 * @file    WFpropagate.c
 * @brief   Wavefront propagation module initialization and CLI registration
 */

#define MODULE_SHORTNAME_DEFAULT "wfprop"
#define MODULE_DESCRIPTION       "Wavefront Propagation (Fresnel Diffraction)"

#include "CLIcore.h"
#include "WFpropagate.h"
#include "wfprop_fresnel_FPS.h"

MODULE_DEPS("milkCOREMODmemory", "milkCOREMODarith", "milkCOREMODiofits",
            "milkfft", "milkOpticsMaterials");

extern DATA data;

/**
 * init_WFpropagate - Initialize module
 *
 * Return: 0 on success.
 */
int init_WFpropagate(void)
{
    return 0;
}

static errno_t init_module_CLI(void)
{
    init_WFpropagate();
    CLIADDCMD_WFpropagate__fresnel_FPS();
    return RETURN_SUCCESS;
}

MILK_MODULE(WFpropagate, init_module_CLI, _module_deps);
