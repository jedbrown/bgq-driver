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


#include "stdlib.h"
#include "stdio.h"
#include <omp.h>





#define NUMTHREADS (64)

int main(int argc, char *argv[])
{
    volatile double d;

    #pragma omp parallel default(none) num_threads(NUMTHREADS)      \
        shared(stderr,d)
    {
        unsigned thdId = omp_get_thread_num();

        double a = 2.0;
        volatile double b = 100.0;
        int i;
        int loops = 0;
        for (i=0; i<=(thdId*1000)+1000; i++) {
            b = b * a;
            loops++;
        }
        d = b;  // keep from being optomized out.

        //#pragma omp ordered
        #pragma omp critical
        {
            fprintf(stderr, "thd %d loops=%d, b=%e\n", thdId, loops, b);
        }
    }
    return 0;
}

