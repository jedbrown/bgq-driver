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

#define MIN(a,b) ((a<b)?a:b)

// Require that UDEC and PPC timebase are within 'n' pclk of each other
#define ACCURACY      16

// Run 'n' loops
#define MAXLOOPS      8

// Run 'n' iterations per loop
#define MAXITERATIONS 8

// Delay per iteration
#define DELAYTIME     1600*1000*100   // 0.1 seconds

// Note: Total runtime = DELAYTIME * MAXLOOPS * MAXITERATIONS * 4


int test_main( int argc, char *argv[], char **envp )
{
    int rc = 0;
    int threadid = ProcessorThreadID();
    int y;
    
    for(y=0; y<MAXLOOPS; y++)
    {
        uint64_t startdelay = GetTimeBase();
        
        // Serialize threads on the core
        if(threadid == 0)
        {
            mtspr(SPRN_TENC, 0xe);
        }
        else
        {
            while(GetTimeBase() - startdelay < DELAYTIME)
            {
            }
        }
        
        // Setup UDEC timer
        mtspr(SPRN_TCR, (mfspr(SPRN_TCR) | TCR_UD) & ~(TCR_UDIE));
        isync();
        mtspr(SPRN_TSR, TSR_UDIS);
        isync();
        mtspr(SPRN_UDEC, ~(0ul));
        isync();
        
        // Run 
        int x;
        uint64_t ppctime[MAXITERATIONS];
        uint64_t udectime[MAXITERATIONS];
        for(x=0; x<MAXITERATIONS; x++)
        {
            ppctime[x] = GetTimeBase();
            udectime[x] = mfspr(SPRN_UDEC);
            while(GetTimeBase() - ppctime[x] < DELAYTIME)
            {
            }
        }
        
        for(x=0; x<MAXITERATIONS-1; x++)
        {
            uint64_t ppcdelta  = ppctime[x+1]-ppctime[x];
            uint64_t udecdelta = udectime[x]-udectime[x+1];
            
            if((ppcdelta  + ACCURACY < udecdelta) ||
               (udecdelta + ACCURACY < ppcdelta))
            {
                printf("FAILURE:  Iteration=%d, %d  ppcdelta=%lx  udecdelta=%lx  diff=%ld\n", y, x, ppcdelta, udecdelta, MIN(ppcdelta-udecdelta, udecdelta-ppcdelta));
                rc=1;
            }
        }
        mtspr(SPRN_UDEC, 0);
        if(threadid < 3)
        {
            mtspr(SPRN_TENS, 1 << (threadid + 1));
            mtspr(SPRN_TENC, 1 << threadid);
        }
        else
        {
            mtspr(SPRN_TENS, 0xf);
        }
    }
    
    return( rc );
}
