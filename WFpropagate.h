#ifndef _WFPROPAGATEMODULE_H
#define _WFPROPAGATEMODULE_H




void __attribute__ ((constructor)) libinit_WFpropagate();
int init_WFpropagate();



int Init_Fresnel_propagate_wavefront(char *Cim, long size, double PUPIL_SCALE, double z, double lambda, double FPMASKRAD, int Precision);

int Fresnel_propagate_wavefront1(char *in, char *out, char *Cin);


//long WFpropagate_run();

#endif
