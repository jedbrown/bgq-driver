/* begin_generated_IBM_copyright_prolog                             */
/*                                                                  */
/* This is an automatically generated copyright prolog.             */
/* After initializing,  DO NOT MODIFY OR MOVE                       */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* Licensed Materials - Property of IBM                             */
/* Blue Gene/Q                                                      */
/* (C) Copyright IBM Corp.  2010, 2012                              */
/* US Government Users Restricted Rights - Use, duplication or      */
/*   disclosure restricted by GSA ADP Schedule Contract with IBM    */
/*   Corp.                                                          */
/*                                                                  */
/* This software is available to you under the Eclipse Public       */
/* License (EPL).                                                   */
/*                                                                  */
/*  --------------------------------------------------------------- */
/*                                                                  */
/* end_generated_IBM_copyright_prolog                               */


#include <stdint.h>
#include <hwi/include/bqc/A2_inlines.h>

char* fwext_getenv(const char* key) { return 0; }


void fwext_udelay( uint64_t  microseconds ) {
    uint64_t end = GetTimeBase() + ( microseconds * 1600ull /*FWExtState.personality->Kernel_Config.FreqMHz*/ );
    while ( GetTimeBase() < end ) 
	/* spin */
    ;

}


