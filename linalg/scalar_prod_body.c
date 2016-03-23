_Complex double _PSWITCH(scalar_prod)(const _PTSWITCH(spinor) * const S, const _PTSWITCH(spinor) * const R, 
			      const int N, const int parallel) {
  _Complex double ALIGN res = 0.0;
#ifdef MPI
  _Complex double ALIGN mres;
#endif

#ifdef OMP
#pragma omp parallel
  {
  int thread_num = omp_get_thread_num();
#endif

  _Complex double ALIGN ds,tr,ts,tt,ks,kc;
  const _PTSWITCH(spinor) *s,*r;

  ks = 0.0;
  kc = 0.0;

#if (defined BGL && defined XLC)
  __alignx(16, S);
  __alignx(16, R);
#endif

#ifdef OMP
#pragma omp for
#endif
  for (int ix = 0; ix < N; ix++)
  {
    s= S + ix;
    r= R + ix;
    
    ds = r->s0.c0 * conj(s->s0.c0) + r->s0.c1 * conj(s->s0.c1) + r->s0.c2 * conj(s->s0.c2) +
         r->s1.c0 * conj(s->s1.c0) + r->s1.c1 * conj(s->s1.c1) + r->s1.c2 * conj(s->s1.c2) +
	 r->s2.c0 * conj(s->s2.c0) + r->s2.c1 * conj(s->s2.c1) + r->s2.c2 * conj(s->s2.c2) + 
         r->s3.c0 * conj(s->s3.c0) + r->s3.c1 * conj(s->s3.c1) + r->s3.c2 * conj(s->s3.c2);

    /* Kahan Summation */
    tr=ds+kc;
    ts=tr+ks;
    tt=ts-ks;
    ks=ts;
    kc=tr-tt;
  }
  kc=ks+kc;

#ifdef OMP
  g_omp_acc_cp[thread_num] = kc;

  } /* OpenMP closing brace */

  /* having left the parallel section, we can now sum up the Kahan
     corrected sums from each thread into kc */
  for(int i = 0; i < omp_num_threads; ++i)
    res += g_omp_acc_cp[i];
#else
  res=kc;
#endif

#ifdef MPI
  if(parallel == 1)
  {
    MPI_Allreduce(&res, &mres, 1, MPI_DOUBLE_COMPLEX, MPI_SUM, MPI_COMM_WORLD);
    return(mres);
  }
#endif
  return(res);
}

// threadsafe versions

_Complex double _PSWITCH(scalar_prod_ts)(const _PTSWITCH(spinor) * const S, const _PTSWITCH(spinor) * const R, 
			      const int N, const int parallel) {
  _Complex double ALIGN res = 0.0;
#ifdef MPI
  _Complex double ALIGN mres;
#endif

#ifdef OMP2
#pragma omp parallel reduction(+:res)
  {
#endif

  _Complex double ALIGN ds,tr,ts,tt,ks,kc;
  const _PTSWITCH(spinor) *s,*r;

  ks = 0.0;
  kc = 0.0;

#if (defined BGL && defined XLC)
  __alignx(16, S);
  __alignx(16, R);
#endif

#ifdef OMP2
#pragma omp for
#endif
  for (int ix = 0; ix < N; ix++)
  {
    s= S + ix;
    r= R + ix;
    
    ds = r->s0.c0 * conj(s->s0.c0) + r->s0.c1 * conj(s->s0.c1) + r->s0.c2 * conj(s->s0.c2) +
         r->s1.c0 * conj(s->s1.c0) + r->s1.c1 * conj(s->s1.c1) + r->s1.c2 * conj(s->s1.c2) +
	 r->s2.c0 * conj(s->s2.c0) + r->s2.c1 * conj(s->s2.c1) + r->s2.c2 * conj(s->s2.c2) + 
         r->s3.c0 * conj(s->s3.c0) + r->s3.c1 * conj(s->s3.c1) + r->s3.c2 * conj(s->s3.c2);

    /* Kahan Summation */
    tr=ds+kc;
    ts=tr+ks;
    tt=ts-ks;
    ks=ts;
    kc=tr-tt;
  }
  kc=ks+kc;

  res = kc;
#ifdef OMP2
  } /* OpenMP closing brace */
#endif

#ifdef MPI
  if(parallel == 1)
  {
    MPI_Allreduce(&res, &mres, 1, MPI_DOUBLE_COMPLEX, MPI_SUM, MPI_COMM_WORLD);
    return(mres);
  }
#endif
  return(res);
}
