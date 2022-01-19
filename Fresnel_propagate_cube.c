/**
 * @file    Fresnel_propagate_cube.c
 *
 */

#include <math.h>

#include "COREMOD_memory/COREMOD_memory.h"
#include "CommandLineInterface/CLIcore.h"

#include "Fresnel_propagate.h"

errno_t Fresnel_propagate_cube(const char *restrict IDcin_name, const char *restrict IDout_name_amp,
                               const char *restrict IDout_name_pha, double PUPIL_SCALE, double zstart, double zend,
                               long NBzpts, double lambda)
{
    imageID IDouta, IDoutp;

    imageID IDcin = image_ID(IDcin_name);
    uint32_t xsize = data.image[IDcin].md[0].size[0];
    uint32_t ysize = data.image[IDcin].md[0].size[1];
    uint8_t datatype = data.image[IDcin].md[0].datatype;

    if (datatype == _DATATYPE_COMPLEX_FLOAT)
    {
        create_3Dimage_ID(IDout_name_amp, xsize, ysize, NBzpts, &IDouta);
        create_3Dimage_ID(IDout_name_pha, xsize, ysize, NBzpts, &IDoutp);
    }
    else
    {
        create_3Dimage_ID_double(IDout_name_amp, xsize, ysize, NBzpts, &IDouta);
        create_3Dimage_ID_double(IDout_name_pha, xsize, ysize, NBzpts, &IDoutp);
    }

    for (uint32_t kk = 0; kk < NBzpts; kk++)
    {
        double zprop = zstart + (zend - zstart) * kk / NBzpts;
        printf("[%u] propagating by %f m\n", kk, zprop);
        Fresnel_propagate_wavefront(IDcin_name, "_propim", PUPIL_SCALE, zprop, lambda);
        imageID IDtmp = image_ID("_propim");
        if (datatype == _DATATYPE_COMPLEX_FLOAT)
        {
            for (uint32_t ii = 0; ii < xsize; ii++)
                for (uint32_t jj = 0; jj < ysize; jj++)
                {
                    double re = data.image[IDtmp].array.CF[jj * xsize + ii].re;
                    double im = data.image[IDtmp].array.CF[jj * xsize + ii].im;
                    double amp = sqrt(re * re + im * im);
                    double pha = atan2(im, re);
                    data.image[IDouta].array.F[kk * xsize * ysize + jj * xsize + ii] = amp;
                    data.image[IDoutp].array.F[kk * xsize * ysize + jj * xsize + ii] = pha;
                }
        }
        else
        {
            for (uint32_t ii = 0; ii < xsize; ii++)
                for (uint32_t jj = 0; jj < ysize; jj++)
                {
                    double re = data.image[IDtmp].array.CD[jj * xsize + ii].re;
                    double im = data.image[IDtmp].array.CD[jj * xsize + ii].im;
                    double amp = sqrt(re * re + im * im);
                    double pha = atan2(im, re);
                    data.image[IDouta].array.D[kk * xsize * ysize + jj * xsize + ii] = amp;
                    data.image[IDoutp].array.D[kk * xsize * ysize + jj * xsize + ii] = pha;
                }
        }

        delete_image_ID("_propim", DELETE_IMAGE_ERRMODE_WARNING);
    }

    return RETURN_SUCCESS;
}
