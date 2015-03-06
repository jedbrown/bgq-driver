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
// split bytes == 128*m + rem
// num_dbls = 16*m + rem

#ifdef OP
  register unsigned n,i,j;
  register double res1=0.0, res2=0.0;
  register unsigned m  = num_dbls >> 4;
  i = 0;          //first stream beginning
  j = (m << 3);   //second stream start from half-way 

  if (m)
  {
    for (n=0; n < (m<<4); n+=2,i++,j++)
    {
      res1  = OP(src0[i], src1[i]);
      res2  = OP(src0[j], src1[j]);
      res1  = OP(res1, src2[i]);
      res2  = OP(res2, src2[j]);
      dst[i]  = res1;
      dst[j]  = res2;
    }
  }

  //remainder of the doubles
  for (n=(m<<4); n < num_dbls; n++)
  {
    res1  = OP(src0[n], src1[n]);
    res1  = OP(res1, src2[n]);
    dst[n]  = res1;
  }
  return;
#endif	/* OP */
