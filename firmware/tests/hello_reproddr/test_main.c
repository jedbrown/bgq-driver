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

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

int main( int argc, char *argv[], char **envp )
{
   printf("(I) main[%2d:%d]: Hello from thread index %d!\n",
           ProcessorCoreID(),      // 0..16
           ProcessorThreadID(),         // 0..3
           ProcessorID() );    // 0..67

   return( 0 );
}


