/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/* ================================================================ */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/*                                                                  */
/* Blue Gene/Q                                                      */
/*                                                                  */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/*                                                                  */
/* US Government Users Restricted Rights -                          */
/* Use, duplication or disclosure restricted                        */
/* by GSA ADP Schedule Contract with IBM Corp.                      */
/*                                                                  */
/* This software is available to you under the                      */
/* Eclipse Public License (EPL).                                    */
/*                                                                  */
/* ================================================================ */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <hwi/include/bqc/A2_inlines.h>
#include <spi/include/l1p/types.h>

L1P_SPIContext           _L1P_Context[68];
L1P_PatternLimitPolicy_t _L1P_PatternLimitMode;
uint64_t                 _L1P_PatternLimit;
uint64_t                 _L1P_PatternCount;
