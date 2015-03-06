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
/**
 * \file test/internals/misc/math.cc
 * \brief ???
 */

#include <stdio.h>

#include "Math.h"

int main(int argc, char **argv)
{
  unsigned utmp[1];
  PAMI::Math::sum (utmp, utmp);
  PAMI::Math::sum<unsigned> (utmp, utmp, 1);
  PAMI::Math::sum (utmp, utmp, 1);

  double tmp[10];
  PAMI::Math::sum (tmp, tmp);
  PAMI::Math::sum (tmp, tmp, 10);

  double tmp64[64];
  PAMI::Math::sum (tmp64, tmp64);

  return 0;
}
