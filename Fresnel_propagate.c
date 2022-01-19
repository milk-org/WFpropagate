/**
 * @file    Fresnel_propagate.c
 *
 */

#include "CommandLineInterface/CLIcore.h"

#include "COREMOD_memory/COREMOD_memory.h"
#include "fft/dofft.h"
#include "fft/permut.h"
#include <math.h>

// variables local to this translation unit
static char *inimname;
static char *outimname;
static double *pupscale;
static double *propz;
static double *proplambda;

// CLI function arguments and parameters
static CLICMDARGDEF farg[] = {{CLIARG_IMG, ".in_name", "input image", "imin", CLICMDARG_FLAG_DEFAULT, FPTYPE_AUTO,
                               FPFLAG_DEFAULT_INPUT, (void **)&inimname, NULL},
                              {CLIARG_STR_NOT_IMG, ".out_name", "output image", "imout", CLICMDARG_FLAG_DEFAULT,
                               FPTYPE_AUTO, FPFLAG_DEFAULT_INPUT, (void **)&inimname, NULL},
                              {CLIARG_FLOAT, ".pupscale", "pupil scale [m/pix]", "1.0", CLICMDARG_FLAG_DEFAULT,
                               FPTYPE_AUTO, FPFLAG_DEFAULT_INPUT, (void **)&pupscale, NULL},
                              {CLIARG_FLOAT, ".propz", "propagation distance [m]", "1.0", CLICMDARG_FLAG_DEFAULT,
                               FPTYPE_AUTO, FPFLAG_DEFAULT_INPUT, (void **)&propz, NULL},
                              {CLIARG_FLOAT, ".proplambda", "wavelength [m]", "0.000001", CLICMDARG_FLAG_DEFAULT,
                               FPTYPE_AUTO, FPFLAG_DEFAULT_INPUT, (void **)&proplambda, NULL}};

// CLI function initialization data
static CLICMDDATA CLIcmddata = {"fresnelpw", "Fresnel propagate WF", CLICMD_FIELDS_DEFAULTS};

// detailed help
static errno_t help_function() { return RETURN_SUCCESS; }

// Computation code
errno_t Fresnel_propagate_wavefront(const char *__restrict in, const char *__restrict out, double PUPIL_SCALE, double z,
                                    double lambda)
{
    DEBUG_TRACE_FSTART();
    DEBUG_TRACEPOINT("FARG %s %s %lf %lf %lf", in, out, PUPIL_SCALE, z, lambda);

    /* all units are in m */
    double coeff;
    uint32_t naxes[2];
    imageID ID;
    double co1;
    long n0h;
    uint8_t datatype;

    do2dfft(in, "tmp");
    permut("tmp");
    ID = image_ID("tmp");
    datatype = data.image[ID].md[0].datatype;

    naxes[0] = data.image[ID].md[0].size[0];
    naxes[1] = data.image[ID].md[0].size[1];
    coeff = PI * z * lambda / (PUPIL_SCALE * naxes[0]) / (PUPIL_SCALE * naxes[0]);

    co1 = 1.0 * naxes[0] * naxes[1];
    n0h = naxes[0] / 2;

    if (datatype == _DATATYPE_COMPLEX_FLOAT)
    {
        for (uint32_t jj = 0; jj < naxes[1]; jj++)
        {
            uint32_t jj1 = naxes[0] * jj;
            uint32_t jj2 = (jj - naxes[1] / 2) * (jj - naxes[1] / 2);
            for (uint32_t ii = 0; ii < naxes[0]; ii++)
            {
                uint32_t ii1 = jj1 + ii;
                uint32_t ii2 = ii - n0h;
                double sqdist = ii2 * ii2 + jj2;
                double angle = -coeff * sqdist;
                double re = data.image[ID].array.CF[ii1].re / co1;
                double im = data.image[ID].array.CF[ii1].im / co1;
                data.image[ID].array.CF[ii1].re = re * cos(angle) - im * sin(angle);
                data.image[ID].array.CF[ii1].im = re * sin(angle) + im * cos(angle);
            }
        }
    }
    else
    {
        for (uint32_t jj = 0; jj < naxes[1]; jj++)
        {
            uint32_t jj1 = naxes[0] * jj;
            uint32_t jj2 = (jj - naxes[1] / 2) * (jj - naxes[1] / 2);
            for (uint32_t ii = 0; ii < naxes[0]; ii++)
            {
                uint32_t ii1 = jj1 + ii;
                uint32_t ii2 = ii - n0h;
                double sqdist = ii2 * ii2 + jj2;
                double angle = -coeff * sqdist;
                double re = data.image[ID].array.CD[ii1].re / co1;
                double im = data.image[ID].array.CD[ii1].im / co1;
                data.image[ID].array.CD[ii1].re = re * cos(angle) - im * sin(angle);
                data.image[ID].array.CD[ii1].im = re * sin(angle) + im * cos(angle);
            }
        }
    }

    permut("tmp");

    do2dffti("tmp", out);

    FUNC_CHECK_RETURN(delete_image_ID("tmp", DELETE_IMAGE_ERRMODE_WARNING));

    DEBUG_TRACE_FEXIT();
    return RETURN_SUCCESS;
}

// Wrapper function, used by all CLI calls
// Defines how local variables are fed to computation code
// Always local to this translation unit
static errno_t compute_function()
{
    DEBUG_TRACE_FSTART();

    INSERT_STD_PROCINFO_COMPUTEFUNC_START

    Fresnel_propagate_wavefront(inimname, outimname, *pupscale, *propz, *proplambda);

    INSERT_STD_PROCINFO_COMPUTEFUNC_END

    DEBUG_TRACE_FEXIT();
    return RETURN_SUCCESS;
}

INSERT_STD_FPSCLIfunctions

    // Register function in CLI
    errno_t
    CLIADDCMD_Fresnel_propagate_wavefront()
{
    INSERT_STD_CLIREGISTERFUNC

    return RETURN_SUCCESS;
}
