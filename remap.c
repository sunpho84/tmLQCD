#define _REMAP_C

#include "remap.h"
#include "su3.h"
#include "global.h"

void global_remap_spinor_into_color_spinor(color_spinor *out,spinor *in,int N)
{
  for(int ivol=0;ivol<N;ivol++)
    {
      /*
      if(g_proc_id==0 && ivol==0)
	{
	  printf("# ivol %d/%d\n",ivol+1,N);
	  printf("# before remap:\n");
          printf("# in.s0.c0: %lg\n",creal(in[ivol].s0.c0));
          printf("# in.s0.c1: %lg\n",creal(in[ivol].s0.c1));
          printf("# in.s0.c2: %lg\n",creal(in[ivol].s0.c2));
          printf("# in.s1.c0: %lg\n",creal(in[ivol].s1.c0));
          printf("# in.s1.c1: %lg\n",creal(in[ivol].s1.c1));
          printf("# in.s1.c2: %lg\n",creal(in[ivol].s1.c2));
          printf("# in.s2.c0: %lg\n",creal(in[ivol].s2.c0));
          printf("# in.s2.c1: %lg\n",creal(in[ivol].s2.c1));
          printf("# in.s2.c2: %lg\n",creal(in[ivol].s2.c2));
          printf("# in.s3.c0: %lg\n",creal(in[ivol].s3.c0));
          printf("# in.s3.c1: %lg\n",creal(in[ivol].s3.c1));
          printf("# in.s3.c2: %lg\n",creal(in[ivol].s3.c2));
	}
      */
      //bufferize
      color_spinor temp;
      temp.c0.sc0=in[ivol].s0.c0;
      temp.c1.sc0=in[ivol].s0.c1;
      temp.c2.sc0=in[ivol].s0.c2;
      temp.c0.sc1=in[ivol].s1.c0;
      temp.c1.sc1=in[ivol].s1.c1;
      temp.c2.sc1=in[ivol].s1.c2;
      temp.c0.sc2=in[ivol].s2.c0;
      temp.c1.sc2=in[ivol].s2.c1;
      temp.c2.sc2=in[ivol].s2.c2;
      temp.c0.sc3=in[ivol].s3.c0;
      temp.c1.sc3=in[ivol].s3.c1;
      temp.c2.sc3=in[ivol].s3.c2;
      
      //copy into the output
      memcpy(out+ivol,&temp,sizeof(color_spinor));
    }
}

void global_remap_color_spinor_into_spinor(spinor *out,color_spinor *in,int N)
{
  for(int ivol=0;ivol<N;ivol++)
    {
      //if(g_proc_id==0 && ivol==0) printf("# unmapping  ivol %d/%d\n",ivol+1,N);
      //bufferize
      spinor temp;
      temp.s0.c0=in[ivol].c0.sc0;
      temp.s0.c1=in[ivol].c1.sc0;
      temp.s0.c2=in[ivol].c2.sc0;
      temp.s1.c0=in[ivol].c0.sc1;
      temp.s1.c1=in[ivol].c1.sc1;
      temp.s1.c2=in[ivol].c2.sc1;
      temp.s2.c0=in[ivol].c0.sc2;
      temp.s2.c1=in[ivol].c1.sc2;
      temp.s2.c2=in[ivol].c2.sc2;
      temp.s3.c0=in[ivol].c0.sc3;
      temp.s3.c1=in[ivol].c1.sc3;
      temp.s3.c2=in[ivol].c2.sc3;
      
      //copy into the output
      memcpy(out+ivol,&temp,sizeof(color_spinor));
    }
}
