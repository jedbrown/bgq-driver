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
#ifndef Timer_h
#define Timer_h

#include "spi/include/kernel/process.h"


class PerfTimer
{
  public:
    PerfTimer(): startCycle(0),endCycle(0),elapsedCycles(0) {
        if (hz == 0) {
            Personality_t personality;
            Kernel_GetPersonality(&personality, sizeof(Personality_t));
            hz = personality.Kernel_Config.FreqMHz * 1.0e6;
        }
    }
    void Reset() { elapsedCycles = startCycle = endCycle = 0; }
    void Start() { startCycle = GetTimeBase(); }
    void Stop()  { if (startCycle > 0) endCycle = GetTimeBase(); elapsedCycles += (endCycle-startCycle); }
    double ElapsedTime() { return (1.0 * elapsedCycles / hz); }
    unsigned long ElapsedCycles() { return elapsedCycles; }


    static double hz; // only init once first time perfTimer created.
  //private:
    volatile unsigned long startCycle;
    volatile unsigned long endCycle;
    unsigned long elapsedCycles;

};


#endif
