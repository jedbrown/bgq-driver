/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q 5765-PER 5765-PRP                                    */
/*                                                                  */
/* (C) Copyright IBM Corp. 2011, 2012 All Rights Reserved           */
/* US Government Users Restricted Rights -                          */
/* Use, duplication, or disclosure restricted                       */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
// 4way operation on doubles
// split num_dbls == 128*m + rem

#ifdef OP
  register double f0, f1, f2, f3, f4, f5, f6, f7;
  register double res1=0.0, res2=0.0, res3=0.0, res4=0.0;
  register unsigned n,i,j,k,l;

  register unsigned m  = num_dbls >> 5;
  i = 0;          //first stream beginning
  j = (m << 3);   //second stream start from 1st quarter
  k = (m << 4);   //third stream start from half
  l = (m << 5) -  (m << 3);  //fourth stream starts from 3rd quarter

  if (m)
  { 
    f0  = src0[i];
    f1  = src0[j];
    f2  = src0[k];
    f3  = src0[l];
    f4  = src1[i];
    f5  = src1[j];
    f6  = src1[k];
    f7  = src1[l];
    i+=1;
    j+=1;
    k+=1;
    l+=1;

    for (n=4; n < (m << 5); n+=4,i++,j++,k++,l++)
    {
      res1 = OP(f0, f4);
      f0  = src0[i];
      f4  = src1[i];

      res2 = OP(f1, f5);
      f1  = src0[j];
      f5  = src1[j];

      res3 = OP(f2, f6);
      f2  = src0[k];
      f6  = src1[k];

      res4 = OP(f3, f7);
      f3  = src0[l];
      f7  = src1[l];

      dst[i-1]  = res1;
      dst[j-1]  = res2;
      dst[k-1]  = res3;
      dst[l-1]  = res4;
      res1 = res2 = res3  = res4  = 0.0;
    }
  }

  //remainder of the doubles
  for (n=(m<<5); n < num_dbls; n++)
  {
    dst[n]  = OP(src0[n], src1[n]);
  }
#endif	/* OP */
