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
// 4way operation on ints
// split bytes == 256*m + rem
// num_ints ==  64*m + rem

#ifdef OP
#ifdef  DTYPE
  register DTYPE  r0, r1, r2, r3, r4, r5, r6, r7;
  register DTYPE res1=0, res2=0, res3=0, res4=0;
  register unsigned n,i,j,k,l;

  register unsigned m  = num_ints >> 6;
  i = 0;                     //first stream beginning
  j = (m << 4);              //second stream start from 1st quarter
  k = (m << 5);              //third stream start from half
  l = (m << 6) -  (m << 4);  //fourth stream starts from 3rd quarter

  if (m)
  { 
    r0  = src0[i];
    r1  = src0[j];
    r2  = src0[k];
    r3  = src0[l];
    r4  = src1[i];
    r5  = src1[j];
    r6  = src1[k];
    r7  = src1[l];
    i++;
    j++;
    k++;
    l++;

    for (n=4; n < (m << 6); n+=4,i++,j++,k++,l++)
    {
      res1 = OP(r0, r4);
      r0  = src0[i];
      r4  = src1[i];

      res2 = OP(r1, r5);
      r1  = src0[j];
      r5  = src1[j];

      res3 = OP(r2, r6);
      r2  = src0[k];
      r6  = src1[k];

      res4 = OP(r3, r7);
      r3  = src0[l];
      r7  = src1[l];

      dst[i-1]  = res1;
      dst[j-1]  = res2;
      dst[k-1]  = res3;
      dst[l-1]  = res4;
      res1 = res2 = res3  = res4  = 0;
    }
      res1 = OP(r0, r4);
      res2 = OP(r1, r5);
      res3 = OP(r2, r6);
      res4 = OP(r3, r7);

      dst[i-1]  = res1;
      dst[j-1]  = res2;
      dst[k-1]  = res3;
      dst[l-1]  = res4;
  }

  //remainder of the ints
  for (n=(m<<6); n < num_ints; n++)
  {
    dst[n]  = OP(src0[n], src1[n]);
  }
#endif	/* DTYPE */
#endif	/* OP */
