/**
 * @file    Fresnel_propagate_cube.h
 *
 */

#ifndef _FRESNEL_PROPAGATE_CUBE_H
#define _FRESNEL_PROPAGATE_CUBE_H


errno_t Fresnel_propagate_cube(
    const char *restrict IDcin_name,
    const char *restrict IDout_name_amp,
    const char *restrict IDout_name_pha,
    double PUPIL_SCALE,
    double zstart,
    double zend,
    long NBzpts,
    double lambda
);

#endif
