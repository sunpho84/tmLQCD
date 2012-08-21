/**********************************************************************
 *
 *
 * Copyright (C) 2012 Carsten Urbach
 *
 * This file is based on an implementation of the Dirac operator 
 * written by Martin Luescher, modified by Martin Hasenbusch in 2002 
 * and modified and extended by Carsten Urbach from 2003-2008
 *
 * This file is part of tmLQCD.
 *
 * tmLQCD is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * tmLQCD is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with tmLQCD.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************/

#include "bgq.h"
#include "bgq3.h"
#include "xlc_prefetch.h"
#include "remap.h"

void Hopping_Matrix_Standard(const int ieo, spinor * const l, spinor * const k) {
#pragma disjoint(*l, *k)
#ifdef _GAUGE_COPY
  if(g_update_gauge_copy) {
    update_backward_gauge(g_gauge_field);
  }
#endif

#if (defined MPI && !(defined _NO_COMM))
  xchange_field(k, ieo);
#endif

#ifdef OMP
#  pragma omp parallel
  {
#endif
  int ioff;
  int * hi;
  su3 * restrict ALIGN up;
  su3 * restrict ALIGN um;
  spinor * restrict ALIGN sp;
  spinor * restrict ALIGN sm;
  spinor * restrict ALIGN rn;
  
#pragma disjoint(*sp, *sm, *rn, *up, *um, *l)
  _declare_regs();

  if(ieo == 0){
    ioff = 0;
  } 
  else{
    ioff = (VOLUME+RAND)/2;
  }

#ifndef OMP
  if(ieo == 0) {
    hi = &g_hi[0];
  }
  else {
    hi = &g_hi[16*VOLUME/2];
  }

#  if ((defined _GAUGE_COPY))
  up=&g_gauge_field_copy[ioff][0];
#  else
  up=&g_gauge_field[(*hi)][0];
#  endif
  hi++;
  sp=k+(*hi);
  hi++;
#endif

  /**************** loop over all lattice sites ******************/
#ifdef OMP
#  pragma omp for
#endif
  for(int icx = ioff; icx < (VOLUME/2+ioff); icx++){
#ifdef OMP
    hi = &g_hi[16*icx];
#  if ((defined _GAUGE_COPY))
    up=&g_gauge_field_copy[icx][0];
#  else
    up=&g_gauge_field[(*hi)][0];
#  endif
    hi++;
    sp=k+(*hi);
    hi++;
#endif
    rn=l+(icx-ioff);
    /*********************** direction +t ************************/
#    if (!defined _GAUGE_COPY)
    um=&g_gauge_field[(*hi)][0]; 
#    else
    um=up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_t_p();

    /*********************** direction -t ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;
    
    _hop_t_m();

    /*********************** direction +1 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][1]; 
#    else
    um = up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_x_p();

    /*********************** direction -1 ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;

    _hop_x_m();

    /*********************** direction +2 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][2]; 
#    else
    um= up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_y_p();

    /*********************** direction -2 ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;

    _hop_y_m();

    /*********************** direction +3 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][3]; 
#    else
    um=up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi++;

    _hop_z_p();

    /*********************** direction -3 ************************/
#ifndef OMP
#  if ((defined _GAUGE_COPY))
    up=um+1;
#  else
    up=&g_gauge_field[(*hi)][0];
#  endif
    hi++;
    sp=k+(*hi);
    hi++;
#endif
    _hop_z_m();

    _store_res();
  }
#ifdef OMP
  } /* OpenMP closing brace */
#endif
  return;
}

void Hopping_Matrix_Remapped(const int ieo, color_spinor * const l, color_spinor * const k) {
#pragma disjoint(*l, *k)
#ifdef _GAUGE_COPY
  if(g_update_gauge_copy) {
    update_backward_gauge(g_gauge_field);
  }
#endif

#if (defined MPI && !(defined _NO_COMM))
  xchange_field((spinor*)k, ieo);
#endif

#ifdef OMP
#  pragma omp parallel
  {
#endif
  int ioff;
  int * hi;
  su3 * restrict ALIGN up;
  su3 * restrict ALIGN um;
  color_spinor * restrict ALIGN sp;
  color_spinor * restrict ALIGN sm;
  color_spinor * restrict ALIGN rn;
  
#pragma disjoint(*sp, *sm, *rn, *up, *um, *l)
  _declare_regs_rem();

  if(ieo == 0){
    ioff = 0;
  } 
  else{
    ioff = (VOLUME+RAND)/2;
  }

#ifndef OMP
  if(ieo == 0) {
    hi = &g_hi[0];
  }
  else {
    hi = &g_hi[16*VOLUME/2];
  }

#  if ((defined _GAUGE_COPY))
  up=&g_gauge_field_copy[ioff][0];
#  else
  up=&g_gauge_field[(*hi)][0];
#  endif
  hi++;
  sp=k+(*hi);
  hi++;
#endif

  /**************** loop over all lattice sites ******************/
#ifdef OMP
#  pragma omp for
#endif
  for(int icx = ioff; icx < (VOLUME/2+ioff); icx++){
#ifdef OMP
    hi = &g_hi[16*icx];
#  if ((defined _GAUGE_COPY))
    up=&g_gauge_field_copy[icx][0];
#  else
    up=&g_gauge_field[(*hi)][0];
#  endif
    hi++;
    sp=k+(*hi);
    hi++;
#endif
    rn=l+(icx-ioff);
    /*********************** direction +t ************************/
#    if (!defined _GAUGE_COPY)
    um=&g_gauge_field[(*hi)][0]; 
#    else
    um=up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_t_p_rem();

    /*********************** direction -t ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;
    
    _hop_t_m_rem();

    /*********************** direction +1 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][1]; 
#    else
    um = up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_x_p_rem();

    /*********************** direction -1 ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;

    _hop_x_m_rem();

    /*********************** direction +2 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][2]; 
#    else
    um= up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi+=2;

    _hop_y_p_rem();

    /*********************** direction -2 ************************/
#    if ((defined _GAUGE_COPY))
    up=um+1;
#    else
    up+=1;
#    endif
    sp=k+(*hi);
    hi++;

    _hop_y_m_rem();

    /*********************** direction +3 ************************/
#    ifndef _GAUGE_COPY
    um=&g_gauge_field[(*hi)][3]; 
#    else
    um=up+1;
#    endif
    hi++;
    sm=k+(*hi);
    hi++;

    _hop_z_p_rem();

    /*********************** direction -3 ************************/
#ifndef OMP
#  if ((defined _GAUGE_COPY))
    up=um+1;
#  else
    up=&g_gauge_field[(*hi)][0];
#  endif
    hi++;
    sp=k+(*hi);
    hi++;
#endif
    _hop_z_m_rem();

    _store_res_rem();
  }
#ifdef OMP
  } /* OpenMP closing brace */
#endif
  return;
}

void Hopping_Matrix(const int ieo,spinor *const l,spinor *const k)
{
  color_spinor *rema_k=(color_spinor*)k;
  color_spinor *rema_l=(color_spinor*)l;
  
  switch(g_remapping)
    {
      //standard case: no reamp
    case NO_REMAP:
      Hopping_Matrix_Standard(ieo,l,k);
      break;
      //benchmark the remapping
    case BENCH_REMAP:
      global_remap_spinor_into_color_spinor(rema_k,k,VOLUME/2);
      global_remap_color_spinor_into_spinor(k,rema_k,VOLUME/2);
      Hopping_Matrix_Standard(ieo,l,k);
      //global_remap_spinor_into_color_spinor(rema_l,l,VOLUME/2);
      //global_remap_color_spinor_into_spinor(l,rema_l,VOLUME/2);
      break;
      //break trough the default since remapped operator has not yet been implemented
    case DO_REMAP:
    default:
      if(g_proc_id==0)
	{
	  fprintf(stderr,"Operation %d not implemented yet\n",g_remapping);
	  MPI_Abort(MPI_COMM_WORLD,1);
	}
    }  
}
