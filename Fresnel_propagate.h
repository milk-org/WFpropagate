/**
 * @file    Fresnel_propagate.h
 *
 */

#ifndef _FRESNEL_PROPAGATE_H
#define _FRESNEL_PROPAGATE_H

errno_t CLIADDCMD_Fresnel_propagate_wavefront();

errno_t Fresnel_propagate_wavefront(const char *restrict in,
                                    const char *restrict out,
                                    double PUPIL_SCALE,
                                    double z,
                                    double lambda);

#endif
