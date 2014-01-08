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
/* This File tests Bgpm_Apply() Bgpm_Attch() collisions  in thread level.
* Here process0 will set Bgpm_Mode as HWDISTRIB
* proces 0 will attch an event set to the currect thread  by using Bgpm_Attach()
* after Bgpm_Attach() if we do Bgpm_Apply(), Apply() need to say events alredy attched error.
* but the case is passing.
*/

#include "stdlib.h"
#include "stdio.h"
#include <omp.h>

#include "cnk/include/Config.h"
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


#define TEST_CASE_VERBOSE
#include "../../test_utils.h"
TEST_INIT();

#define INVALID_PARM -1
int Dummy=-1;
int numProcs;
int numThreads;

void Bgpm_Attach_test(int);

unsigned measList[] = {
PEVT_IU_IS1_STALL_CYC,
PEVT_CYCLES,
PEVT_INST_ALL
};
int numMeasEvts = sizeof(measList) / sizeof(int);
int hPuSet,hPuSet2; 


int main(int argc, char *argv[])
{
     TEST_MODULE_PRINT0(Check Bgpm Apply and Attach Collisions);
     numProcs= Kernel_ProcessCount();
     printf("numProcs=%d\n", numProcs);
     numThreads = atoi(argv[1]);

     Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error

     SHMC_Init_Barriers(numProcs, numThreads)
     #pragma omp parallel default(none) num_threads(numThreads)    \
        shared(stderr,numFailures)               
     {
      // uint64_t appMask = 0;  uint64_t agentMask = 0;
       int hPuSet;

       unsigned measList[] = {
       PEVT_IU_IS1_STALL_CYC,
       PEVT_CYCLES,
       PEVT_INST_ALL
       };
       int numMeasEvts = sizeof(measList) / sizeof(int);

       printf("Kernel_ProcessorID : %d \n",Kernel_ProcessorID());
       TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_HWDISTRIB),0);  /* Setting Bgpm Mode to HWDISTRIB Mode */
       hPuSet = Bgpm_CreateEventSet();
       Bgpm_AddEventList(hPuSet, measList, numMeasEvts);
   
       //ShmBarrier1();
       ShmBarrier2(); 
       unsigned thdId = Kernel_ProcessorID();
       uint64_t thdCount;
       uint64_t curThdMask = UPC_APP_HWTID_MASK(thdId);
       Bgpm_ReadThreadEvent(hPuSet, 1, curThdMask, 0, &thdCount);


       if(Kernel_ProcessorID() == 0) {
        printf("I am in Thread 0 check \n");
        //appMask   |= UPC_APP_HWTID_MASK(0);
        //agentMask |= UPC_AGENT_HWTID_MASK(0);
        //TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet, appMask, agentMask),0);
        TEST_CHECK_EQUAL(Bgpm_Apply(hPuSet),0);
       }
       else{
        //appMask   |= UPC_APP_HWTID_MASK(Kernel_ProcessorID());
        //agentMask |= UPC_AGENT_HWTID_MASK(Kernel_ProcessorID());
        //TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet, appMask, agentMask),0);
        TEST_CHECK_EQUAL(Bgpm_Apply(hPuSet),0);
       }
       ShmBarrier1();
      
       #pragma omp critical  
//#if 0
       if((Kernel_ProcessorID() != 0)) {
        //appMask   |= UPC_APP_HWTID_MASK(Kernel_ProcessorID());
        //agentMask |= UPC_AGENT_HWTID_MASK(Kernel_ProcessorID());
        //TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet, appMask, agentMask),0);
        TEST_CHECK_EQUAL(Bgpm_Apply(hPuSet),0);
       }
       else{
        //appMask   |= UPC_APP_HWTID_MASK(Kernel_ProcessorID());
        //agentMask |= UPC_AGENT_HWTID_MASK(Kernel_ProcessorID());
        //TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet, appMask, agentMask),0);
 
        TEST_CHECK_EQUAL(Bgpm_Apply(hPuSet),0);
       }
//#endif
     Bgpm_Disable();      
     }
     TEST_RETURN();
}
