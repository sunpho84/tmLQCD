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

#define _quad_vec_add(a,b,c) a=vec_add(b,c);
#define _quad_vec_addself(a,b) _quad_vec_add(a,a,b);

#define _quad_vec_mul_add(a,b,c,d) a=vec_madd(c,d,b);
#define _quad_vec_mul_addself(a,b,c) _quad_vec_mul_add(a,a,b,c);

#define _quad_vec_i_mul_imag_part_add(a,b,c,d) a=vec_xxnpmadd(c,d,b);
#define _quad_vec_i_mul_imag_part_addself(a,b,c) _quad_vec_i_mul_imag_part_add(a,a,b,c)

#define _quad_vec_mul_sub(a,b,c,d) a=vec_nmsub(c,d,b);
#define _quad_vec_mul_subself(a,b,c) _quad_vec_mul_sub(a,a,b,c);

#define _quad_vec_sub(a,b,c) a=vec_sub(b,c);
#define _quad_vec_subself(a,b) _quad_vec_sub(a,a,b);

#define _quad_vec_i_mul_imag_part_sub(a,b,c,d) a=vec_xxcpnmadd(c,d,b);
#define _quad_vec_i_mul_imag_part_subself(a,b,c) _quad_vec_i_mul_imag_part_sub(a,a,b,c)


#define _declare_regs_rem()						\
  vector4double ALIGN r_low_flip_patt;					\
  vector4double ALIGN r0,r1,r2, r3,r4,r5, r6,r7,r8, r9,r10,r11, r12,r13,r14, r15,r16,r17, r18,r19,r20; \
  vector4double ALIGN r_one,r_pm_one,r_phase;				\
  __alignx(16,l);							\
  __alignx(16,k);							\
  r_low_flip_patt =vec_gpci(02301);					\
  r_one = vec_splats(1.0);						\
  {									\
    double temp[4]={+1,+1,-1,-1};					\
    r_pm_one = vec_ld(0L,temp);						\
  }

//########################################################################

#define _complex_biload(r,c) (r)=vec_lda(0L,(double*)&(c));
#define _complex_perm(a,b,c) a=vec_perm(b,b,c);
#define _complex_flip(a,b) _complex_perm(a,b,r_low_flip_patt);
#define _complex_flipassign(a) _complex_flip(a,a);

#define _spin_load(r0,r1,phi)			\
  (r0) = vec_ld(0L, (double*)(&phi.sc0));	\
  (r1) = vec_ld(0L, (double*)(&phi.sc2));

#define _spin_store(phi,r0,r1)			\
  vec_st(r0,0L, (double*)(&phi.sc0));		\
  vec_st(r1,0L, (double*)(&phi.sc2));

#define _color_spinor_load(c0h,c0l, c1h,c1l, c2h,c2l, phi)	\
  _spin_load(c0h,c0l, (phi)->c0);				\
  _spin_load(c1h,c1l, (phi)->c1);				\
  _spin_load(c2h,c2l, (phi)->c2);

#define _color_spinor_load_flipping_low(c0h,c0l, c1h,c1l, c2h,c2l, phi)	\
  _color_spinor_load(c0h,c0l, c1h,c1l, c2h,c2l, phi)			\
  _complex_flipassign(c0l);						\
  _complex_flipassign(c1l);						\
  _complex_flipassign(c2l);

#define _su3_load_rem(u00,u01,u02, u10,u11,u12, u20,u21,u22, u)		\
  _complex_biload(u00,u->c00);						\
  _complex_biload(u01,u->c01);						\
  _complex_biload(u02,u->c02);						\
  _complex_biload(u10,u->c10);						\
  _complex_biload(u11,u->c11);						\
  _complex_biload(u12,u->c12);						\
  _complex_biload(u20,u->c20);						\
  _complex_biload(u21,u->c21);						\
  _complex_biload(u22,u->c22);

#define _su3_transp_load_rem(u00,u01,u02, u10,u11,u12, u20,u21,u22, u)	\
  _complex_biload(u00,u->c00);						\
  _complex_biload(u01,u->c10);						\
  _complex_biload(u02,u->c20);						\
  _complex_biload(u10,u->c01);						\
  _complex_biload(u11,u->c11);						\
  _complex_biload(u12,u->c21);						\
  _complex_biload(u20,u->c02);						\
  _complex_biload(u21,u->c12);						\
  _complex_biload(u22,u->c22);

//########################################################################

#define _store_res_rem()			\
  _spin_store(rn->c0,r15,r18);			\
  _spin_store(rn->c1,r16,r19);			\
  _spin_store(rn->c2,r17,r20);

//########################################################################

#define _bicomplex_prod_quad_vec(out,bi,qv)	\
  out=vec_xmul(bi,qv);				\
  out=vec_xxnpmadd(qv,bi,out);

#define _bicomplex_summ_the_prod_quad_vec(out,bi,qv)	\
  out=vec_xmadd(bi,qv,out);				\
  out=vec_xxnpmadd(qv,bi,out);

#define _bicomplex_conj_prod_quad_vec(out,bi,qv)		\
  out=vec_xmul(bi,qv);						\
  out=vec_xxcpnmadd(qv,bi,out);

#define _bicomplex_conj_summ_the_prod_quad_vec(out,bi,qv)	\
  out=vec_xmadd(bi,qv,out);					\
  out=vec_xxcpnmadd(qv,bi,out);

#define _color_addself(r0,r1,r2, r3,r4,r5)			\
  _quad_vec_addself(r0,r3);					\
  _quad_vec_addself(r1,r4);					\
  _quad_vec_addself(r2,r5);

#define _color_subself(r0,r1,r2, r3,r4,r5)			\
  _quad_vec_subself(r0,r3);					\
  _quad_vec_subself(r1,r4);					\
  _quad_vec_subself(r2,r5);

#define _color_mul_addself(r0,r1,r2, r3,r4,r5, c)	\
  _quad_vec_mul_addself(r0,r3,c);			\
  _quad_vec_mul_addself(r1,r4,c);			\
  _quad_vec_mul_addself(r2,r5,c);

#define _color_mul_subself(r0,r1,r2, r3,r4,r5, c)	\
  _quad_vec_mul_subself(r0,r3,c);			\
  _quad_vec_mul_subself(r1,r4,c);			\
  _quad_vec_mul_subself(r2,r5,c);

#define _color_prod_bicomplex(out, u0,u1,u2, in0,in1,in2)	\
  _bicomplex_prod_quad_vec(out,u0,in0);				\
  _bicomplex_prod_quad_vec(out,u1,in1);				\
  _bicomplex_prod_quad_vec(out,u2,in2);

#define _color_summ_the_prod_bicomplex(out, u0,u1,u2, in0,in1,in2)	\
  _bicomplex_summ_the_prod_quad_vec(out,u0,in0);			\
  _bicomplex_summ_the_prod_quad_vec(out,u1,in1);			\
  _bicomplex_summ_the_prod_quad_vec(out,u2,in2);

#define _color_conj_prod_bicomplex(out, u0,u1,u2, in0,in1,in2)	\
  _bicomplex_conj_prod_quad_vec(out,u0,in0);			\
  _bicomplex_conj_prod_quad_vec(out,u1,in1);			\
  _bicomplex_conj_prod_quad_vec(out,u2,in2);

#define _color_conj_summ_the_prod_bicomplex(out, u0,u1,u2, in0,in1,in2)	\
  _bicomplex_conj_summ_the_prod_quad_vec(out,u0,in0);			\
  _bicomplex_conj_summ_the_prod_quad_vec(out,u1,in1);			\
  _bicomplex_conj_summ_the_prod_quad_vec(out,u2,in2);

#define _color_i_mul_imag_part_addself(r0,r1,r2, r3,r4,r5, c)	\
  _quad_vec_i_mul_imag_part_addself(r0,r3,c);			\
  _quad_vec_i_mul_imag_part_addself(r1,r4,c);			\
  _quad_vec_i_mul_imag_part_addself(r2,r5,c);

#define _color_i_mul_imag_part_subself(r0,r1,r2, r3,r4,r5, c)	\
  _quad_vec_i_mul_imag_part_subself(r0,r3,c);			\
  _quad_vec_i_mul_imag_part_subself(r1,r4,c);			\
  _quad_vec_i_mul_imag_part_subself(r2,r5,c);

#define _su3_summ_the_prod_bicomplex(u)					\
  _su3_load_rem(r6,r7,r8, r9,r10,r11, r12,r13,r14, u);			\
  _color_summ_the_prod_bicomplex(r3, r6,r7,r8,    r0,r1,r2);		\
  _color_summ_the_prod_bicomplex(r4, r9,r10,r11,  r0,r1,r2);		\
  _color_summ_the_prod_bicomplex(r5, r12,r13,r14, r0,r1,r2);

#define _su3_prod_bicomplex(u)						\
  _su3_load_rem(r6,r7,r8, r9,r10,r11, r12,r13,r14, u);			\
  _color_prod_bicomplex(r3, r6,r7,r8,    r0,r1,r2);			\
  _color_prod_bicomplex(r4, r9,r10,r11,  r0,r1,r2);			\
  _color_prod_bicomplex(r5, r12,r13,r14, r0,r1,r2);

#define _su3_dag_summ_the_prod_bicomplex(u)				\
  _su3_transp_load_rem(r6,r7,r8, r9,r10,r11, r12,r13,r14, u);		\
  _color_conj_summ_the_prod_bicomplex(r3, r6,r7,r8,    r0,r1,r2);	\
  _color_conj_summ_the_prod_bicomplex(r4, r9,r10,r11,  r0,r1,r2);	\
  _color_conj_summ_the_prod_bicomplex(r5, r12,r13,r14, r0,r1,r2);

#define _su3_dag_prod_bicomplex(u)					\
  _su3_transp_load_rem(r6,r7,r8, r9,r10,r11, r12,r13,r14, u);		\
  _color_conj_prod_bicomplex(r3, r6,r7,r8,    r0,r1,r2);		\
  _color_conj_prod_bicomplex(r4, r9,r10,r11,  r0,r1,r2);		\
  _color_conj_prod_bicomplex(r5, r12,r13,r14, r0,r1,r2);

//########################################################################

#define _hop_t_p_rem()						\
  _color_spinor_load(r0,r3, r1,r4, r2,r5, sp);			\
  _color_addself(r0,r1,r2, r3,r4,r5);				\
  _su3_prod_bicomplex(up);					\
  _complex_biload(r_phase,ka0);					\
  _vec_cmplx_mul_double2c(r15,r16,r17, r3,r4,r5, r_phase);	\
  r18=r15;r19=r16;r20=r17;

#define _hop_t_m_rem()						\
  _color_spinor_load(r0,r3, r1,r4, r2,r5, sm);			\
  _color_subself(r0,r1,r2, r3,r4,r5);				\
  _su3_dag_prod_bicomplex(um);					\
  _vec_cmplxcg_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);	\
  _color_addself(r15,r16,r17, r6,r7,r8);			\
  _color_subself(r18,r19,r20, r6,r7,r8);


#define _hop_x_p_rem()							\
  _color_spinor_load_flipping_low(r0,r3, r1,r4, r2,r5, sp);		\
  _color_i_mul_imag_part_addself(r0,r1,r2, r3,r4,r5, r_one);		\
  _su3_prod_bicomplex(up);						\
  _complex_biload(r_phase,ka1);						\
  _vec_cmplx_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);			\
  _color_addself(r15,r16,r17, r6,r7,r8);				\
  _color_i_mul_imag_part_subself(r18,r19,r20, r6,r7,r8, r_one);

#define _hop_x_m_rem()							\
  _color_spinor_load_flipping_low(r0,r3, r1,r4, r2,r5, sm);		\
  _color_i_mul_imag_part_subself(r0,r1,r2, r3,r4,r5, r_one);		\
  _su3_dag_prod_bicomplex(um);						\
  _vec_cmplxcg_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);		\
  _color_addself(r15,r16,r17, r6,r7,r8);				\
  _color_i_mul_imag_part_addself(r18,r19,r20, r6,r7,r8, r_one);


#define _hop_y_p_rem()						\
  _color_spinor_load_flipping_low(r0,r3, r1,r4, r2,r5, sp);	\
  _color_mul_addself(r0,r1,r2, r3,r4,r5, r_pm_one);		\
  _su3_prod_bicomplex(up);					\
  _complex_biload(r_phase,ka2);					\
  _vec_cmplx_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);		\
  _color_addself(r15,r16,r17, r6,r7,r8);			\
  _color_mul_subself(r18,r19,r20, r6,r7,r8, r_pm_one);

#define _hop_y_m_rem()						\
  _color_spinor_load_flipping_low(r0,r3, r1,r4, r2,r5, sm);	\
  _color_mul_subself(r0,r1,r2, r3,r4,r5, r_pm_one);		\
  _su3_dag_prod_bicomplex(um);					\
  _vec_cmplxcg_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);	\
  _color_addself(r15,r16,r17, r6,r7,r8);			\
  _color_mul_addself(r18,r19,r20, r6,r7,r8, r_pm_one);


#define _hop_z_p_rem()							\
  _color_spinor_load(r0,r3, r1,r4, r2,r5, sp);				\
  _color_i_mul_imag_part_subself(r0,r1,r2, r3,r4,r5, r_pm_one);		\
  _su3_prod_bicomplex(up);						\
  _complex_biload(r_phase,ka3);						\
  _vec_cmplx_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);			\
  _color_addself(r15,r16,r17, r6,r7,r8);				\
  _color_i_mul_imag_part_addself(r18,r19,r20, r6,r7,r8, r_pm_one);

#define _hop_z_m_rem()							\
  _color_spinor_load(r0,r3, r1,r4, r2,r5, sm);				\
  _color_i_mul_imag_part_addself(r0,r1,r2, r3,r4,r5, r_pm_one);		\
  _su3_dag_prod_bicomplex(um);						\
  _vec_cmplxcg_mul_double2c(r6,r7,r8, r3,r4,r5, r_phase);		\
  _color_addself(r15,r16,r17, r6,r7,r8);				\
  _color_i_mul_imag_part_subself(r18,r19,r20, r6,r7,r8, r_pm_one);

#endif
