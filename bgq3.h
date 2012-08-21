/**********************************************************************
 *
 * Copyright (C) 2012 Francesco Sanfilippo
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
 * These are routines and macros for using remapped spinor (color-spinor)
 * using floating point units of the BG/Q processor
 *
 **********************************************************************/

#ifndef _BGQ3_H
#define _BGQ3_H

#define _declare_regs_rem()						\
  vector4double ALIGN r0,r1,r2,r3,r4,r5,r6,r7,r8,r9,r10,r11;		\
  vector4double ALIGN r_minus;						\
  __alignx(16,l);                                                       \
  __alignx(16,k);							\
  r_minus = vec_splats(-1.0);

//########################################################################

#define _complex_biload(r,c)			\
  (r)=vec_lda(0L,(double*)&c);

#define _spin_load(r0,r1,phi)			\
  (r0) = vec_ld(0L, (double*)(&phi.sc0));	\
  (r1) = vec_ld(0L, (double*)(&phi.sc2));

#define _color_spinor_load(c0l,c0h, c1l,c1h, c2l,c2h, phi)	\
  _spin_load(c0l,c0h, (phi)->c0);				\
  _spin_load(c1l,c1h, (phi)->c1);				\
  _spin_load(c2l,c2h, (phi)->c2);

#define _su3_conf_load_rem(c00,c01,c02, c10,c11,c12, c20,c21,c22, U)	\
  _complex_biload(c00,U.c00);						\
  _complex_biload(c01,U.c01);						\
  _complex_biload(c02,U.c02);						\
  _complex_biload(c10,U.c10);						\
  _complex_biload(c11,U.c11);						\
  _complex_biload(c12,U.c12);						\
  _complex_biload(c20,U.c20);						\
  _complex_biload(c21,U.c21);						\
  _complex_biload(c22,U.c22);

//########################################################################

#define _store_res_rem()

//########################################################################

#define _hop_t_p_rem()				\
  _color_spinor_load(r0,r3, r1,r4, r3,r5, sm);	

#define _hop_t_m_rem()
#define _hop_x_p_rem()
#define _hop_x_m_rem()
#define _hop_y_p_rem()
#define _hop_y_m_rem()
#define _hop_z_p_rem()

#define _hop_z_proj_rem(r0,r1)  r0 = vec_xxnpmadd(r1, r_minus, r0);

#define _hop_z_m_rem()							\

#endif
