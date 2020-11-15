/**
 * @file    Fresnel_propagate_cube.c
 *
 */

#include <math.h>

#include "CommandLineInterface/CLIcore.h"
#include "COREMOD_memory/COREMOD_memory.h"

#include "Fresnel_propagate.h"


errno_t Fresnel_propagate_cube(
    const char *restrict IDcin_name,
    const char *restrict IDout_name_amp,
    const char *restrict IDout_name_pha,
    double PUPIL_SCALE,
    double zstart,
    double zend,
    long NBzpts,
    double lambda
)
{
    imageID IDouta, IDoutp;
    imageID IDcin;
    uint32_t xsize, ysize;
    uint32_t ii, jj, kk;
    double zprop;
    long IDtmp;
    double re, im, amp, pha;
    uint8_t datatype;

    IDcin = image_ID(IDcin_name);
    xsize = data.image[IDcin].md[0].size[0];
    ysize = data.image[IDcin].md[0].size[1];
    datatype = data.image[IDcin].md[0].datatype;

    if(datatype == _DATATYPE_COMPLEX_FLOAT)
    {
        IDouta = create_3Dimage_ID(IDout_name_amp, xsize, ysize, NBzpts);
        IDoutp = create_3Dimage_ID(IDout_name_pha, xsize, ysize, NBzpts);
    }
    else
    {
        IDouta = create_3Dimage_ID_double(IDout_name_amp, xsize, ysize, NBzpts);
        IDoutp = create_3Dimage_ID_double(IDout_name_pha, xsize, ysize, NBzpts);
    }

    for(kk = 0; kk < NBzpts; kk++)
    {
        zprop = zstart + (zend - zstart) * kk / NBzpts;
        printf("[%u] propagating by %f m\n", kk, zprop);
        Fresnel_propagate_wavefront(IDcin_name, "_propim", PUPIL_SCALE, zprop, lambda);
        IDtmp = image_ID("_propim");
        if(datatype == _DATATYPE_COMPLEX_FLOAT)
        {
            for(ii = 0; ii < xsize; ii++)
                for(jj = 0; jj < ysize; jj++)
                {
                    re = data.image[IDtmp].array.CF[jj * xsize + ii].re;
                    im = data.image[IDtmp].array.CF[jj * xsize + ii].im;
                    amp = sqrt(re * re + im * im);
                    pha = atan2(im, re);
                    data.image[IDouta].array.F[kk * xsize * ysize + jj * xsize + ii] = amp;
                    data.image[IDoutp].array.F[kk * xsize * ysize + jj * xsize + ii] = pha;
                }
        }
        else
        {
            for(ii = 0; ii < xsize; ii++)
                for(jj = 0; jj < ysize; jj++)
                {
                    re = data.image[IDtmp].array.CD[jj * xsize + ii].re;
                    im = data.image[IDtmp].array.CD[jj * xsize + ii].im;
                    amp = sqrt(re * re + im * im);
                    pha = atan2(im, re);
                    data.image[IDouta].array.D[kk * xsize * ysize + jj * xsize + ii] = amp;
                    data.image[IDoutp].array.D[kk * xsize * ysize + jj * xsize + ii] = pha;
                }
        }

        delete_image_ID("_propim");
    }

    return RETURN_SUCCESS;
}
