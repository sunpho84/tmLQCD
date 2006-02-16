/* $Id$ */
/*************************************************************
 *
 * This is an implementation of bicgstab(l)
 * corresponding to the paper of G. L.G. Sleijpen and
 * D.R. Fokkema
 * Transactions on Numerical Analysis
 * Volume1, pp. 11-32, 1993
 *
 * Author: Carsten Urbach
 *         urbach@physik.fu-berlin.de
 *
 *************************************************************/

#ifdef HAVE_CONFIG_H
# include<config.h>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include "global.h"
#include "complex.h"
#include "su3.h"
#include "linalg_eo.h"
#include "start.h"
#include "solver/matrix_mult_typedef.h"
#include "bicgstab2.h"

int bicgstab2(spinor * const x0, spinor * const b, const int max_iter, 
		double eps_sq, const int rel_prec, const int N, matrix_mult f) {

  const int l = 2;
  double err;
  int i, j, k;
  int update_app = 0, update_res = 0;
  double rho0, rho1, beta, alpha, omega, gamma_hat,
    sigma, kappa0, kappal, rho, zeta, zeta0;
  double squarenorm, Mx=0., Mr=0.;
  spinor * r[5], * u[5], * r0_tilde, * u0, * x, * xp, * bp;
  double Z[3][3], y0[3], yl[3], yp[3], ypp[3];


  k = -l;
/*   init_solver_field(2*(l+1)+2); */
  r0_tilde = g_spinor_field[DUM_SOLVER];
  u0 = g_spinor_field[DUM_SOLVER+1];
  r[0] = g_spinor_field[DUM_SOLVER+2];
  u[0] = g_spinor_field[DUM_SOLVER+3];
  r[1] = g_spinor_field[DUM_SOLVER+4];
  u[1] = g_spinor_field[DUM_SOLVER+5];
  r[2] = g_spinor_field[DUM_SOLVER+6];
  u[2] = g_spinor_field[DUM_SOLVER+7];
  bp = g_spinor_field[DUM_SOLVER+8];
  xp = x0;
  x = g_spinor_field[DUM_SOLVER+9];

  zero_spinor_field(x, N);
  assign(u[0], b, N);
  f(r0_tilde, xp);
  diff(r[0], u[0], r0_tilde, N);
  zero_spinor_field(u0, N);
/*   assign(r0_tilde, r[0], N); */
  random_spinor_field(r0_tilde, N);
  assign(bp, r[0], N);
  squarenorm = square_norm(b, N);

  rho0 = 1.;
  alpha = rho0;
  omega = rho0;
  err = square_norm(r[0], N);
  Mr = err;
  Mx = err;
  zeta0 = err;
  while( k < max_iter && (((err > eps_sq) && (rel_prec == 0)) 
			  || ((err > eps_sq*squarenorm) && (rel_prec == 1)) 
			  )) {
    k+=l;

    /* The BiCG part */
    rho0 *= -omega; 
    for(j = 0; j < l; j++) {
      rho1 = scalar_prod_r(r[j], r0_tilde, N);
      beta = alpha*(rho1/rho0); 
      rho0 = rho1;
/*       if(g_proc_id == 0) {printf("beta = %e, alpha = %e, rho0 = %e\n", beta, alpha, rho0);fflush(stdout);} */
      for(i = 0; i <= j; i++) {
	/* u_i = r_i - \beta u_i */
	assign_mul_add_r(u[i], -beta, r[i], N);
      }
      f(u[j+1], u[j]);
      sigma = scalar_prod_r(u[j+1], r0_tilde, N);
      alpha = rho1/sigma;
/*       if(g_proc_id == 0) {printf("sigma = %e, alpha = %e\n", sigma, alpha);fflush(stdout);} */
      /* x = x + \alpha u_0 */
      assign_add_mul_r(x, u[0], alpha, N);
      /* r_i = r_i - \alpha u_{i+1} */
      for(i = 0; i <= j; i++) {
	assign_add_mul_r(r[i], u[i+1], -alpha, N);
      }
      f(r[j+1], r[j]);
      err = square_norm(r[j+1], N);
      if(g_proc_id == 0) {printf("%d %d err = %e\n", k, j, err);fflush(stdout);}
      if(err > Mr) Mr = err;
      if(err > Mx) Mx = err;
    }

    /* The polynomial part */

    /* Z = R* R */
    for(i = 0; i <= l; i++){
      for(j = 0; j <= i; j++){
	Z[i][j] = scalar_prod_r(r[j], r[i], N);
	Z[j][i] = Z[i][j];
      }
    }

    /* r0tilde and rl_tilde */
    y0[0] = -1;
    y0[2] = 0.;
    y0[1] = Z[1][0]/Z[1][1];
    yl[0] = 0.;
    yl[2] = -1.;
    yl[1] = Z[1][2]/Z[1][1];

    /* Convex combination */
    for(i = 0; i < l+1; i++){
      yp[i] = 0.;
      ypp[i] = 0.;
      for(j = 0; j < l+1; j++) {
	yp[i] +=Z[i][j]*y0[j];
	ypp[i] +=Z[i][j]*yl[j];
      }
    }
    kappa0 = sqrt( y0[0]*yp[0] + y0[1]*yp[1] + y0[2]*yp[2] );
    kappal = sqrt( yl[0]*ypp[0] + yl[1]*ypp[1] + yl[2]*ypp[2] );
    rho = (yl[0]*yp[0] + yl[1]*yp[1] + yl[2]*yp[2])/kappa0/kappal;
    if(fabs(rho) > 0.7) {
      gamma_hat = rho;
    }
    else {
      gamma_hat = rho*0.7/fabs(rho);
    }
    for(i = 0; i <= l; i++) {
      y0[i] -= gamma_hat*kappa0*yl[i]/kappal;
    }

    /* Update */
    omega = y0[l];
    for(i = 1; i < l+1; i++) {
      assign_add_mul_r(u[0], u[i], -y0[i], N);
      assign_add_mul_r(x, r[i-1], y0[i], N);
      assign_add_mul_r(r[0], r[i], -y0[i], N);
    }
    zeta = kappa0;
    /* Reliable update part */
    err = square_norm(r[0], N);
    if(err > Mr) Mr = err;
    if(err > Mx) Mx = err;    
    update_app = (err < 1.e-4*zeta0 && zeta0 <= Mx);
    update_res = (err < 1.e-4*Mr && zeta0 <= Mr);
    if(update_res) {
      if(g_proc_id == 0) printf("Update res\n");
      f(r[0], x);
      diff(r[0], bp, r[0], N);
      Mr = 0.;
      if(update_app) {
	if(g_proc_id == 0) printf("Update app\n");
	Mx = 0.;
	assign_add_mul_r(xp, x, 1., N);
	zero_spinor_field(x, N);
	assign(bp, r[0], N);
      }
    }
    update_app = 0;
    update_res = 0;
    if(g_proc_id == 0){
      printf(" BiCGstabell iterated %d %d, %e rho0 = %e, alpha = %e, gamma_hat= %e\n", l, k, err, rho0, alpha, gamma_hat);
/*       printf(" %e %e %e %e\n", kappa0, kappal, rho1, rho); */
      fflush( stdout );
    }
  }
  assign_add_mul_r(x, xp, 1., N);
  assign(x0, x, N);
  if(k == max_iter) return(-1);
  return(k);
}
