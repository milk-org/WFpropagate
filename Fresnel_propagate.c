/**
 * @file    Fresnel_propagate.c
 *
 */


#include "CommandLineInterface/CLIcore.h"

#include "COREMOD_memory/COREMOD_memory.h"










static errno_t Fresnel_propagate_wavefront__cli()
{

    if(0
            + CLI_checkarg(1, CLIARG_IMG)
            + CLI_checkarg(2, CLIARG_STR_NOT_IMG)
            + CLI_checkarg(3, CLIARG_FLOAT)
            + CLI_checkarg(4, CLIARG_FLOAT)
            + CLI_checkarg(5, CLIARG_FLOAT)
            == 0)
    {
        Fresnel_propagate_wavefront(
            data.cmdargtoken[1].val.string,
            data.cmdargtoken[2].val.string,
            data.cmdargtoken[3].val.numf,
            data.cmdargtoken[4].val.numf,
            data.cmdargtoken[5].val.numf);

        return CLICMD_SUCCESS;
    }
    else
    {
        return CLICMD_INVALID_ARG;
    }
}







// ==========================================
// Register CLI command(s)
// ==========================================

errno_t Fresnel_propagate_wavefront_addCLIcmd()
{

    RegisterCLIcommand(
		"fresnelpw",
		__FILE__, 
		Fresnel_propagate_wavefront__cli, 
		"Fresnel propagate WF", 
		"<input image> <output image> <pupil scale m/s> <prop dist> <lambda>", 
		"fresnelpw in out 0.01 1000 0.0000005", 
		"int Fresnel_propagate_wavefront(char *in, char *out, double PUPIL_SCALE, double z, double lambda)"
		);	
	
    return RETURN_SUCCESS;
}











errno_t Fresnel_propagate_wavefront(
    const char *restrict in,
    const char *restrict out,
    double PUPIL_SCALE,
    double z,
    double lambda
)
{
    /* all units are in m */
    double coeff;
    long ii, jj, ii1, jj1;
    long naxes[2];
    imageID ID;
    double sqdist;
    double re, im;
    double angle;
    double co1;
    long ii2, jj2;
    long n0h, n1h;
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
    n1h = naxes[1] / 2;


//	printf("coeff = %g     co1 = %g\n", coeff, co1);

    if(datatype == _DATATYPE_COMPLEX_FLOAT)
    {
        for(jj = 0; jj < naxes[1]; jj++)
        {
            jj1 = naxes[0] * jj;
            jj2 = (jj - naxes[1] / 2) * (jj - naxes[1] / 2);
            for(ii = 0; ii < naxes[0]; ii++)
            {
                ii1 = jj1 + ii;
                ii2 = ii - n0h;
                sqdist = ii2 * ii2 + jj2;
                angle = -coeff * sqdist;
                re = data.image[ID].array.CF[ii1].re / co1;
                im = data.image[ID].array.CF[ii1].im / co1;
                data.image[ID].array.CF[ii1].re = re * cos(angle) - im * sin(angle);
                data.image[ID].array.CF[ii1].im = re * sin(angle) + im * cos(angle);
            }
        }
    }
    else
    {
        for(jj = 0; jj < naxes[1]; jj++)
        {
            jj1 = naxes[0] * jj;
            jj2 = (jj - naxes[1] / 2) * (jj - naxes[1] / 2);
            for(ii = 0; ii < naxes[0]; ii++)
            {
                ii1 = jj1 + ii;
                ii2 = ii - n0h;
                sqdist = ii2 * ii2 + jj2;
                angle = -coeff * sqdist;
                re = data.image[ID].array.CD[ii1].re / co1;
                im = data.image[ID].array.CD[ii1].im / co1;
                data.image[ID].array.CD[ii1].re = re * cos(angle) - im * sin(angle);
                data.image[ID].array.CD[ii1].im = re * sin(angle) + im * cos(angle);
            }
        }
    }

    permut("tmp");


    do2dffti("tmp", out);


    delete_image_ID("tmp");

    return RETURN_SUCCESS;
}

