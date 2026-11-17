// SPDX-FileCopyrightText: 2026 Olivier Guyon et al
//
// SPDX-License-Identifier: LGPL-3.0-or-later

/**
 * @file    WFpropagate.c
 * @brief   Wavefront propagation module initialization and CLI registration
 */

#define MODULE_SHORTNAME_DEFAULT "wfprop"
#define MODULE_DESCRIPTION       "Wavefront Propagation (Fresnel Diffraction)"

#include "milk_runnable.h"
#include "WFpropagate.h"
#include "wfprop_fresnel_FPS.h"

MODULE_DEPS("milkCOREMODmemory", "milkCOREMODarith", "milkCOREMODiofits",
            "milkfft", "milkimagegen", "milkOpticsMaterials");

/**
 * init_WFpropagate - Initialize module
 *
 * Return: 0 on success.
 */
int init_WFpropagate(void)
{
    return 0;
}

static const MILK_RUNNABLE *const runnables[] = {
    &runnable_wfprop_fresnel,
};

MILK_MODULE_RUNNABLES(WFpropagate, _module_deps, runnables);
