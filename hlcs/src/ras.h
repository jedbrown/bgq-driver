/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/* Product(s):                                                      */
/*     Blue Gene/P Licensed Machine Code                            */
/*                                                                  */
/* (C) Copyright IBM Corp.  2007, 2007                              */
/* All rights reserved.                                             */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* Licensed Materials-Property of IBM                               */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#ifndef HLCS_RAS_H
#define HLCS_RAS_H

// BGQ RAS Error Codes for the MMCS Component 
//
// Rules:
//  1) The MMCS range is 0x0006xxxx
//
//  2) We will define subranges within this range, such as 0x00060xxx   and  0x00061xxx

// MMCS Operations = 00060000 - 00060fff  (MMCSOps_xxxx)
        
// MMCS Hardware Monitor =  00061000 - 00061fff
// These are defined in the MMCSEnvMonitor.cc, and start at 0x00061001

// runjob RAS = 00062000 - 00062fff
// these are defined in the runjob source

// security RAS = 00063000 - 00063fff
// these are defined in the security source


#endif

