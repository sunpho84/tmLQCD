#ifndef _REMAP_H
#define _REMAP_H

#define NO_REMAP 0
#define BENCH_REMAP 1
#define FAKE_APPL_REMAP 2
#define DO_REMAP 3

#ifdef _REMAP_C
 int g_remapping=NO_REMAP;
#else
 extern int g_remapping;
#endif

#include "su3.h"

void global_remap_spinor_into_color_spinor(color_spinor *out,spinor *in,int N);
void global_remap_color_spinor_into_spinor(spinor *out,color_spinor *in,int N);

#endif
