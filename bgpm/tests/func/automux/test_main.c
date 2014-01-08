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

#include "cnk/include/Config.h"
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"


//#define TEST_CASE_VERBOSE

#include "../../test_utils.h"
TEST_INIT();

#define NUM_EVENTS 16
#define MAX_THREADS 64

int numProcs;
int numThreads;
uint64_t cnts_threads[MAX_THREADS][NUM_EVENTS];   // Should be changed depending on the value we give in Make file.
uint64_t cnts_ref[MAX_THREADS][NUM_EVENTS];       // make it thread specific
uint64_t cnts[NUM_EVENTS];

int OF=0;
int MUX=0;                                               // This parameter specifies whether to enable/disable multiplexing
int period=0;
static int flags[MAX_THREADS];                                  // Flag to inicate whether the counts need initialization

void muxTest(int,int,int,int);

unsigned evtList[] = { // Number of events as 23
  PEVT_AXU_IDLE,
  PEVT_AXU_INSTR_COMMIT,
  PEVT_IU_IL1_MISS,
  PEVT_AXU_FP_EXCEPT,
  PEVT_L1P_BAS_LD,
  PEVT_INST_QFPU_FMUL,   // Enable and see lesset count value
  PEVT_XU_TIMEBASE_TICK,
  PEVT_XU_ANY_FLUSH,
  PEVT_IU_IS1_STALL_CYC, // enable and see lesser count
  PEVT_IU_IS2_STALL_CYC,
  PEVT_L1P_BAS_LU_STALL_LIST_RD,
  PEVT_IU_IBUFF_FLUSH,
  PEVT_L1P_STRM_WRT_INVAL,
  PEVT_AXU_CR_COMMIT,
  PEVT_AXU_DENORM_FLUSH,
  PEVT_LSU_COMMIT_AXU_LDS
};
int nummeasevts = sizeof(evtList) / sizeof(int);

void sampletest()
{
   volatile double a=9.87678;
   volatile double b=8.767876;
   volatile int x[1024],y[1024],z[2048]; 
   int i;
   for(i=0;i<6000;i++)
   {
     a=a*5;
     a=a+b;
     b=b*5;
   }
   for(i=0;i<1024;i++)
   {
    x[i]=i*4;
    y[i]=i*8;
    z[i]=x[i]+y[i];
   }  
}

void ReadEvents(int hEvtSet,int thdId)
{
    VERBOSE_PRINT("Event handle: %d\n",hEvtSet);
    unsigned numEvts=NUM_EVENTS;

    int i;
    //printf("I am in ReadEvents...Thread ID: %d\n",thdId); 
    //printf("It is %d Thread flags[%d]= %d\n",thdId,thdId,flags[thdId]);
    ////////Inittialize counts array//////////////
    if(flags[thdId]==0){
        VERBOSE_PRINT("Initializing cnts_ref for thd: %d\n",thdId);
        for (i=0;i<NUM_EVENTS;i++) {
            cnts_ref[thdId][i]=0;
        }
    }
    //////////////////////////////////////////////
    TEST_CHECK_EQUAL(Bgpm_ReadEventList(hEvtSet,&(cnts_threads[thdId][0]),&numEvts),0);
    TEST_CHECK_EQUAL(numEvts, NUM_EVENTS);

    for(i=0;i<NUM_EVENTS;i++)
    {
       // cnts_threads[thdId][i]=cnts[i];
        IFVERBOSE(const char *label = Bgpm_GetEventLabel(hEvtSet, i));
        VERBOSE_PRINT("%d  bfr %20ld %20ld %s \n", thdId,  cnts_ref[thdId][i], cnts_threads[thdId][i], label );
    }

    for (i=0;i<NUM_EVENTS;i++){
        const char *ll = Bgpm_GetEventLabel(hEvtSet, i);
        VERBOSE_PRINT("thdid: %d Comparing : %20ld %20ld %s\n",thdId,cnts_ref[thdId][i], cnts_threads[thdId][i], ll);
        if(cnts_ref[thdId][i] <= cnts_threads[thdId][i]) {
            VERBOSE_PRINT1("Greater Count   ...  \n");
            cnts_ref[thdId][i]=cnts_threads[thdId][i];
        }
        else {
            printf("%02d  0x%016lx %20ld %s \n", thdId, cnts_threads[thdId][i], cnts_threads[thdId][i], ll );
            printf("%02d  priv count: %20ld, comparing with : %20ld\n", thdId, cnts_ref[thdId][i],cnts_threads[thdId][i]);
            printf("%02d  Count is less than earlier\n", thdId);
            exit(0);
        }
   }
   if(flags[thdId] == 0 ){
       flags[thdId]=1;
   }
   #if TEST_CASE_VERBOSE
   for(i=0;i<NUM_EVENTS;i++)
   {
      // cnts_threads[thdId][i]=cnts[i];
       const char *label = Bgpm_GetEventLabel(hEvtSet, i);
       printf("%d  aft  %20ld %20ld %s \n", thdId, cnts_ref[thdId][i], cnts_threads[thdId][i], label );
   }
   #endif
}


int main(int argc, char *argv[])
{

    //Bgpm_EnableTracePrint();
    numThreads = atoi(argv[1]);

    MUX=BGPM_TRUE;
    OF=BGPM_FALSE;
    period=0;
    TEST_MODULE_PRINT0( TEST: AUTOMATIC MUXING TEST);
    muxTest(MUX,period,OF,numThreads);

#if 0
    MUX=BGPM_TRUE;
    OF=BGPM_FALSE;
    period=60000;
    TEST_MODULE_PRINT0( TEST: MUXING TEST WITH PERIOD=60000);
    muxTest(MUX,period,OF,numThreads);
    
//#endif 
    MUX=BGPM_TRUE;
    OF=BGPM_TRUE;
    period=0;
    TEST_MODULE_PRINT0( TEST: AUTOMATIC MUXING TEST WITH OVERFLOW);
    muxTest(MUX,period,OF,numThreads);

//#if 0
    MUX=BGPM_TRUE;
    OF=BGPM_TRUE;
    period=60;
    TEST_MODULE_PRINT0( TEST: MUXING WITH ACTIVE PERIOD AND  OVERFLOW);
    muxTest(MUX,period,OF,numThreads);
#endif
   
    TEST_RETURN();
}

void muxTest(mux,period,of,numThreads)
{
     numProcs= Kernel_ProcessCount();
     printf("numProcs=%d numThreads: %d\n", numProcs,numThreads);

     Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error
     int hEvtSet;
     int seconds = 60;
     fprintf(stderr, "seconds=%d\n",seconds);
     uint64_t cpuSpeed = Upci_GetNodeParms()->cpuSpeed;
     uint64_t targCycles = cpuSpeed * 1024*1024 *seconds;
     fprintf(stderr, "cpuspeed=%ld  targCycles=%ld\n", cpuSpeed, targCycles);
     uint64_t startTB = GetTimeBase();
     //startTB = startTB - 1;

     SHMC_Init_Barriers(numProcs, numThreads)

     #pragma omp parallel default(none) num_threads(numThreads)    \
        shared(stderr,hEvtSet,evtList,startTB,MUX,OF,period,targCycles,numProcs,numThreads,flags,numFailures,cnts_ref,cnts_threads)               
     {
       printf("Kernel_ProcessorID : %d \n",Kernel_ProcessorID());
       TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

       unsigned thdId;
       thdId = Kernel_ProcessorID();
 
        #pragma omp barrier
        if(thdId == 0 ){
          hEvtSet= Bgpm_CreateEventSet();
          if(MUX){
            int normalize= 0; //BGPM_NORMAL;
            printf("Period: %d\n",period);
            TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hEvtSet, period, normalize,BGPMMuxMinEvts),0);
            }
            Bgpm_AddEventList(hEvtSet, evtList, sizeof(evtList)/sizeof(unsigned));

          if(OF){
             printf("Setting Overflow\n");
            int i;
            for(i=0;i<NUM_EVENTS;i++){
             TEST_CHECK_EQUAL(Bgpm_SetOverflow(hEvtSet,i,100000),0);
             }
            }

            TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);
            TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0);
        }
        #pragma omp barrier

        //Bgpm_EnableTracePrint();

        flags[thdId]=0; 
        uint64_t loops = 0;
        while ((GetTimeBase()-startTB) < targCycles) { // reduce target cycles
            //////Sample test////////////////////
            int j;
            if(thdId == 0)
            {
              for(j=0;j<5;j++) {
                sampletest();
              }
              #if 1   // Disabling Switch multiplex because we are seeing lesser count
              if(period==0){
                 //#pragma omp critical (Eventscompare)
                 // {
                  //fprintf(stderr, _AT_ " Thread %d\n", Kernel_ProcessorID());
                  //ReadEvents(hEvtSet,thdId);

                  VERBOSE_PRINT1("I am Switching MUX\n");
                  Bgpm_SwitchMux(hEvtSet);
                 // }
              }
              #endif
            }
            else {
                Upci_Delay(10000);  // give thread 0 some breathing room
            }
             //////////Sample Tets end/////////////
             
             #pragma omp critical (Eventscompare)
             {
                  VERBOSE_PRINT1("\n");
                  ReadEvents(hEvtSet,thdId);
             }  // closing critical

            //#pragma omp barrier
             if ((thdId == 0) && ((loops % 1000) == 0)) {
                 fprintf(stderr, "%02d  loops=%ld\n", thdId, loops);
             }
             else if ((loops % 10000) == 0) {
                 fprintf(stderr, "%02d  loops=%ld\n", thdId, loops);
             }
             loops++;

        }//closing while
        #pragma omp barrier

        fprintf(stderr, "%02d  totalloops=%ld\n", thdId, loops);

        //Bgpm_DisableTracePrint();

         if(thdId ==0 ){
           TEST_CHECK_EQUAL(Bgpm_Stop(hEvtSet),0);
          }
           TEST_CHECK_EQUAL(Bgpm_Disable(),0);
     }
}
