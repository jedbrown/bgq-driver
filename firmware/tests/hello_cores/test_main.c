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
#include <string.h>

// get the goodies
#include <hwi/include/bqc/A2_core.h>
#include <hwi/include/bqc/A2_inlines.h>

// includes mambo callthru

#include "callthru_config.h"


void worker_1()
{

    MamboWriteConsole("worker_1(): routine called\n");
    //printf("worker_1: core 1, thread 0\n");

}


void worker_2()
{

    MamboWriteConsole("worker_2(): routine called\n");
    //printf("worker_2: core 2, thread 0\n");

}


void worker_3()
{

    MamboWriteConsole("worker_3(): routine called\n");
    //printf("worker_3: core 3, thread 0\n");


}



int main( int argc, char *argv[], char **envp )
{

    // Dispatcher for each thread

    int coreId = ProcessorCoreID();

    int threadId = ProcessorThreadID();


    switch (coreId) {
    //
    case 0:
        // core 0:

        if (threadId == 0) 
        {
            // main: 
            printf("main(): coreId = %d, threadId = %d\n", coreId, threadId);
            return(0);
        }
        else
        {
            // all other threads for core 0 - do nothing
            return(0);
        }

    case 1:
        // core 1:
        if (threadId == 0) 
        {
            // worker_1: 
            worker_1();
            return(0);
        }
        else
        {
            // all other threads for core 1 - do nothing
            return(0);
        }


    case 2:
        // core 2:
        if (threadId == 0) 
        {
            // worker_2: 
            worker_2();
            return(0);
        }
        else
        {
            // all other threads for core 2 - do nothing
            return(0);
        }


    case 3:
        // core 3:
        if (threadId == 0) 
        {
            // worker_3: 
            worker_3();
            return(0);
        }
        else
        {
            // all other threads for core 3 - do nothing
            return(0);
        }

    default:
        return(0);

    
    }

#if 0
   printf("(I) main[%2d:%d]: Hello from thread index %d!\n",
           ProcessorCoreID(),      // 0..16
           ProcessorThreadID(),         // 0..3
           ProcessorID() );    // 0..67
#endif 
                                       

   return( 0 );
}


