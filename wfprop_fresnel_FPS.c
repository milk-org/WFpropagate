// SPDX-FileCopyrightText: 2026 Olivier Guyon et al
//
// SPDX-License-Identifier: LGPL-3.0-or-later

/**
 * @file    wfprop_fresnel_FPS.c
 * @brief   FPS V2 compute unit for Fresnel wavefront propagation
 */

#include "milk_runnable.h"
#include "COREMOD_memory/COREMOD_memory.h"
#include "WFpropagate/WFpropagate.h"
#include "wfprop_fresnel_FPS.h"

/* ================================================================
 * 1.  LOCAL PARAMETER VARIABLES
 * ============================================================= */

static double param_pupilscale = 0.01;
static double param_z          = 1000.0;
static double param_lambda     = 0.5e-6;
static char   param_inname[FUNCTION_PARAMETER_STRMAXLEN]  = "wfin";
static char   param_outname[FUNCTION_PARAMETER_STRMAXLEN] = "wfout";

/* ================================================================
 * 2.  UNIFIED PARAMETER TABLE (X-Macro)
 * ============================================================= */

#define FPS_PARAMS(X)                                                           \
    X(".inname", param_inname, FPTYPE_STREAMNAME, 1, FPFLAG_DEFAULT_INPUT,      \
      "Input complex image")                                                    \
    X(".outname", param_outname, FPTYPE_STREAMNAME, 1, FPFLAG_DEFAULT_INPUT,    \
      "Output complex image")                                                   \
    X(".pupilscale", &param_pupilscale, FPTYPE_FLOAT64, 1,                      \
      FPFLAG_DEFAULT_INPUT, "Pupil sampling scale [m/pixel]")                    \
    X(".distance", &param_z, FPTYPE_FLOAT64, 1, FPFLAG_DEFAULT_INPUT,           \
      "Propagation distance [m]")                                               \
    X(".lambda", &param_lambda, FPTYPE_FLOAT64, 1, FPFLAG_DEFAULT_INPUT,         \
      "Optical wavelength [m]")

MILK_PARAMS_DEFINE(wfprop_fresnel, FPS_PARAMS);

/* ================================================================
 * 3.  COMPUTATION LOGIC
 * ============================================================= */

/**
 * fpsexec - Execute Fresnel diffractive propagation
 *
 * Return: RETURN_SUCCESS on success, error code otherwise.
 */
static MILK_HOT errno_t fpsexec(void)
{
    Fresnel_propagate_wavefront(param_inname, param_outname, param_pupilscale,
                                param_z, param_lambda);

    return RETURN_SUCCESS;
}

/**
 * wfprop_fresnel_compute - Execute Fresnel propagation within procinfo loop
 * @self: Pointer to this MILK_RUNNABLE descriptor.
 *
 * Return: RETURN_SUCCESS on completion.
 */
static MILK_HOT errno_t wfprop_fresnel_compute(
    const MILK_RUNNABLE *self)
{
    DEBUG_TRACE_FSTART();
    errno_t ret = RETURN_SUCCESS;

    MILK_PROCINFO_INIT(self)
    MILK_PROCINFO_LOOPSTART(self)
    ret = fpsexec();
    MILK_PROCINFO_END(self)

    DEBUG_TRACE_FEXIT();
    return ret;
}

/* ================================================================
 * 4.  MILK RUNNABLE REGISTRATION
 * ============================================================= */

const MILK_RUNNABLE runnable_wfprop_fresnel = {
    .kind = MILK_RUNNABLE_FPS,
    .app  = {
        .fps_name         = "wfprop_fresnel",
        .cmdkey           = "fresnelprop",
        .description      = "Fresnel diffractive propagation of optical wavefront",
        .description_long = "Propagates a 2D complex optical field over a distance z "
                            "using the Fourier-domain Fresnel quadratic phase transfer "
                            "function."
    },
    .srcfile = __FILE__,
    MILK_PARAMS_REF(wfprop_fresnel),
    .compute = wfprop_fresnel_compute,
};

EXPORT_FPSEXEC(milk, "wfprop-fresnel", runnable_wfprop_fresnel);
