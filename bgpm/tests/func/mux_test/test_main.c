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
#include "bgpm/include/bgpm.h"
#include "spi/include/upci/testlib/upci_debug.h"

#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

__thread uint64_t cnts_SinglePass[23];
__thread uint64_t cnts_Multiple[23];
__thread uint64_t cnts_overflow[23];
__thread uint64_t cnts_reset[23]; 

TEST_INIT();
__thread int OF=0;
__thread int MUX=0;     // This parameter specifies whether to enable/disable multiplexing

__thread  unsigned evtL6[] = {
    PEVT_AXU_FP_DS_ACTIVE,
    PEVT_AXU_FP_EXCEPT,
    PEVT_INST_QFPU_FPGRP2,
    PEVT_WAKE_HIT_10,
    PEVT_INST_XU_FST,
    PEVT_INST_QFPU_FMUL
};
__thread unsigned evtL12[] = {
    PEVT_AXU_FP_DS_ACTIVE,
    PEVT_AXU_FP_EXCEPT,
    PEVT_INST_QFPU_FPGRP2,
    PEVT_WAKE_HIT_10,
    PEVT_INST_XU_FST,
    PEVT_INST_QFPU_FMUL,
    //PEVT_IU_IL1_MISS,  //This event is not scaling with nunber of times we execute sampletest
    PEVT_INST_QFPU_QCVT,
    PEVT_INST_QFPU_FPGRP1,
    PEVT_AXU_FP_EXCEPT,
    PEVT_XU_RUN_INSTR,
    //PEVT_L1P_BAS_MISS, //This event is not scaling with nunber of times we execute sampletest
    PEVT_INST_XU_FLD,
    PEVT_CYCLES
    };

__thread unsigned evtL23[] = {
    PEVT_AXU_FP_DS_ACTIVE,
    PEVT_AXU_FP_EXCEPT,
    PEVT_INST_QFPU_FPGRP2,
    PEVT_WAKE_HIT_10,
    PEVT_INST_XU_FST,
    PEVT_INST_QFPU_FMUL,
    PEVT_INST_QFPU_QCVT,
    PEVT_XU_TIMEBASE_TICK,  
    PEVT_INST_QFPU_FPGRP1,    
    PEVT_XU_RUN_INSTR,         
    PEVT_INST_XU_FLD,
    PEVT_CYCLES,                   

    PEVT_IU_RSV_ANY_L2_BACK_INV,
    PEVT_INST_QFPU_QRND,  
    PEVT_INST_QFPU_QADD,
    PEVT_WAKE_HIT_11,
    PEVT_INST_QFPU_QCVT,
    PEVT_INST_XU_FLD,
    PEVT_XU_ICSWX_COMMIT,
    PEVT_INST_QFPU_GRP_MASK,
    PEVT_L1P_BAS_LU_STALL_LIST_RD,
    //PEVT_IU_IU0_REDIR_CYC,
    PEVT_XU_ICSWX_FAILED
   };
   
#if 0
 unsigned evtL24[] = {
    PEVT_AXU_FP_DS_ACTIVE,
    //PEVT_AXU_FP_DS_ACTIVE,
    PEVT_AXU_FP_EXCEPT,
    PEVT_AXU_INSTR_COMMIT,
    PEVT_AXU_DENORM_FLUSH,
    PEVT_INST_XU_FST,
    PEVT_INST_QFPU_FMUL,
    PEVT_MMU_IERAT_MISS,
    //PEVT_MMU_HTW_MISS_IERAT,
    PEVT_INST_XU_FLD,
    PEVT_INST_XU_FLD,
    PEVT_INST_QFPU_FMUL,
    //PEVT_IU_IL1_MISS,
    PEVT_XU_TIMEBASE_TICK,
    PEVT_XU_PPC_COMMIT,
    PEVT_XU_RUN_INSTR,
    PEVT_L1P_BAS_MISS,
    PEVT_CYCLES,
    PEVT_CYCLES,
    PEVT_L1P_BAS_LD,
    //PEVT_INST_QFPU_FMUL,   
    //PEVT_INST_QFPU_FMUL,   
    //PEVT_XU_TIMEBASE_TICK,
    //PEVT_XU_ANY_FLUSH,
    //PEVT_IU_IS1_STALL_CYC, 
    //PEVT_IU_IS2_STALL_CYC,
    //PEVT_IU_IS2_STALL_CYC,
    PEVT_L1P_BAS_LU_STALL_LIST_RD,
    PEVT_L1P_BAS_LU_STALL_LIST_RD,
    //PEVT_IU_IBUFF_FLUSH,
    PEVT_L1P_STRM_WRT_INVAL,
    PEVT_AXU_CR_COMMIT,
    PEVT_AXU_DENORM_FLUSH,
    PEVT_LSU_COMMIT_AXU_LDS,
    PEVT_LSU_COMMIT_AXU_LDS
    //PEVT_LSU_COMMIT_AXU_LDS,
    };
#endif

void SetAttributes(unsigned hEvtSet,int setoverflow)
{
    unsigned idx0;
    unsigned idx8=-1;
    unsigned idx20=-1;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    idx0 = Bgpm_GetEventIndex(hEvtSet, PEVT_AXU_FP_DS_ACTIVE, 0);
    if(numEvts > 7 )
    {
     idx8 = Bgpm_GetEventIndex(hEvtSet,PEVT_AXU_FP_EXCEPT,0);
    }

    if(numEvts > 13)
    {
     idx20=Bgpm_GetEventIndex(hEvtSet, PEVT_L1P_BAS_LU_STALL_LIST_RD,0);
    } 

    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hEvtSet,idx0,BGPM_TRUE),0);
    TEST_CHECK_EQUAL(Bgpm_SetEventInvert(hEvtSet,idx0,BGPM_TRUE),0);
    if(setoverflow){
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(hEvtSet,idx0,5000000),0);
    }
    if(numEvts > 7)
    {
    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hEvtSet,idx8,BGPM_TRUE),0);
    TEST_CHECK_EQUAL(Bgpm_SetEventInvert(hEvtSet,idx8,BGPM_TRUE),0);
    }
    if(numEvts >13)
   {
    TEST_CHECK_EQUAL(Bgpm_SetEventEdge(hEvtSet,idx20,BGPM_TRUE),0);
    if(setoverflow){
    TEST_CHECK_EQUAL(Bgpm_SetOverflow(hEvtSet,idx20,5000000),0);
    }
   }  
   
}

void ReadAttributs(unsigned hEvtSet)
{
    uint64_t  muxCycles;
    int numEvts = Bgpm_NumEvents(hEvtSet);
    unsigned i;
    for(i=0;i<numEvts;i++)
    {  
        muxCycles = Bgpm_GetMuxEventElapsedCycles(hEvtSet, i);
        const char *label = Bgpm_GetEventLabel(hEvtSet, i);
        printf("    0x%016lx %s \n", muxCycles, label );
    }
   
    unsigned idx0;
    unsigned idx8=-1;
    unsigned idx20=-1;
    idx0 = Bgpm_GetEventIndex(hEvtSet, PEVT_AXU_FP_DS_ACTIVE, 0);
    if(numEvts > 7 )
    {
    idx8 = Bgpm_GetEventIndex(hEvtSet,PEVT_AXU_FP_EXCEPT,0);
    }
    if(numEvts > 13)
    {
     idx20=Bgpm_GetEventIndex(hEvtSet,PEVT_L1P_BAS_LU_STALL_LIST_RD, 0);
    }


    ushort pedge;
    ushort pInvert;
    uint64_t period;
    Bgpm_OverflowHandler_t handler;
    pedge=-1; 
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(hEvtSet,idx0,&pedge),0); TEST_CHECK_EQUAL(pedge,1);
    pInvert=-1;
    TEST_CHECK_EQUAL(Bgpm_GetEventInvert(hEvtSet,idx0,&pInvert),0); TEST_CHECK_EQUAL(pInvert,1);

    if(OF){
    TEST_CHECK_EQUAL(Bgpm_GetOverflow(hEvtSet,idx0, &period, &handler),0);
    printf("Period:  0x%016lx \n",period);
    }
    if(numEvts > 7)
    {
    pedge=-1;
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(hEvtSet,idx8,&pedge),0); TEST_CHECK_EQUAL(pedge,1);
    pInvert=-1;
    TEST_CHECK_EQUAL(Bgpm_GetEventInvert(hEvtSet,idx8,&pInvert),0); TEST_CHECK_EQUAL(pInvert,1);
    }
    if(numEvts > 13){
    pedge=-1;
    TEST_CHECK_EQUAL(Bgpm_GetEventEdge(hEvtSet,idx20,&pedge),0); TEST_CHECK_EQUAL(pedge,1);
    if(OF){
    TEST_CHECK_EQUAL(Bgpm_GetOverflow(hEvtSet,idx20, &period, &handler),0);
    printf("Period:  0x%ld \n",period);
    }
    }
}
void clearcounts(uint64_t *cnts,int size)
{
    int i;
    for(i=0;i<size;i++){
    cnts[i]=0;
    }
}


void ReadEvents(ushort hNMEvtSet,uint64_t *cnts)
{
    int i;
    unsigned numEvts = Bgpm_NumEvents(hNMEvtSet);

    TEST_CHECK_EQUAL(Bgpm_ReadEventList(hNMEvtSet,cnts,&numEvts),0);
    for(i=0;i<numEvts;i++)
    {
        const char *label = Bgpm_GetEventLabel(hNMEvtSet, i);
        printf("    0x%016lx %20ld %s \n", cnts[i], cnts[i], label );
    }

}

void compare_counts(uint64_t *cnts_SinglePass,uint64_t *cnts_Multiple,int size,int repeat)
{

   int i;
   uint64_t predict_value[size];
   float low_range[size]; 
   float upper_range[size]; 
   for(i=0;i<size;i++)
   {
    if (cnts_SinglePass[i] >0)
    {
      predict_value[i]=(cnts_SinglePass[i])* repeat;

      //low_range[i]=predict_value[i] - (predict_value[i] * 0.01);
      //upper_range[i] = predict_value[i] + (predict_value[i] * 0.01);
      low_range[i]=predict_value[i] - (predict_value[i] * 0.10);      // Checking the range can be 10% high or low
      upper_range[i] = predict_value[i] + (predict_value[i] * 0.10);
      //TEST_CHECK_RANGE(predict_value[i], low_range[i], upper_range[i]) 
      char strg[30]; sprintf(strg, "i=%d", i);
      TEST_CHECK_RANGEDBL_wLABEL(cnts_Multiple[i], low_range[i], upper_range[i], strg) ;
    } 
   }
   #if 0 // This code compares how much percentage events got increased from the prevoius test(test once) 
         // and also indicates wheter it's increase or decrease by (+/-)
    int i;
    uint64_t compared[size];
    printf("\tComparision results:\n"); 
    printf("-------------------------------------------------------------------------------\n");
    for(i=0;i<size;i++)
    {
      compared[i]=cnts_Multiple[i]-cnts_SinglePass[i]; 
      if(cnts_SinglePass[i] > cnts_Multiple[i]){   
      compared[i]=cnts_SinglePass[i]-cnts_Multiple[i];
      printf("|\tEvent %d : %15ld \t Percentage Change (-) : %15f |\n",i,compared[i],((float)compared[i]*100/(float)cnts_Multiple[i]));
      }
      else if(cnts_SinglePass[i] <  cnts_Multiple[i]){
      compared[i]=cnts_Multiple[i]-cnts_SinglePass[i];
      printf("| \tEvent %d : %15ld  \t Percentage Change (+) : %15f |\n",i,compared[i],((float)compared[i]*100/(float)cnts_SinglePass[i]));
      }
      else{
      printf("| \tEvent %d :  \t         0                                                | \n",i); 
      }
    }    
    printf("-------------------------------------------------------------------------------\n");
    #endif
}

void sampletest()
{
   volatile double a=9.87678;
   volatile double b=8.767876;
   int i; 
   for(i=0;i<2000000;i++)
   {
     a=a*5;
     a=a+b;
     b=b*5;
   }
}

void Attributes_test(int MUX,int setoverflow)
{
    if(MUX){
    TEST_CASE_PRINT0( TEST: MULTIPLXED EVENTSET ATTRIBUTES READ TEST AFTER APPENDING ADDITIONAL EVENT);
    }
    else{
    TEST_CASE_PRINT0( TEST: NON-MULTIPLXED EVENTSET ATTRIBUTES READ TEST AFTER APPENDING ADDITIONAL EVENT);
    }
    int loop,numevents;

    for(loop=0;loop<=2;loop++){
        Bgpm_Init(BGPM_MODE_SWDISTRIB);
        int hEvtSet = Bgpm_CreateEventSet();
    
        if(MUX){
        int normalize= BGPM_NORMAL;
        uint64_t period;
        period = 6000;
        TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hEvtSet, period, normalize,BGPMMuxMinEvts),0);
        }
     switch (loop){
      case 0:
             printf("\n\n  CHECKING FOR 6 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL6, sizeof(evtL6)/sizeof(unsigned));
             break;
      case 1:
             printf("\n\n  CHECKING FOR 12  NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL12, sizeof(evtL12)/sizeof(unsigned));
             break;
      case 2:
             printf("\n\n  CHECKING FOR 23 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));
             break;
       }

        numevents=Bgpm_NumEvents(hEvtSet);
        printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));

        // Setting Attributes for events
        SetAttributes(hEvtSet,setoverflow);
        ReadAttributs(hEvtSet);                                           // read attribute information immediatly after setting it.
        TEST_CHECK_EQUAL(Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC),0);   // Add event after setting attributes to events
        ReadAttributs(hEvtSet);                                           // read attributes of events   
        TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);
        TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0);
        sampletest();
        ReadAttributs(hEvtSet);
        Bgpm_Stop(hEvtSet);
        TEST_CHECK_EQUAL(Bgpm_Disable(),0);
     }
}
    
void Reset_Repeat_event_test(int MUX)
{
    TEST_AT()

    if (!MUX){
    TEST_CASE_PRINT0( TEST: NON MULTIPLXED EVENTSET SCENARIO TEST);
    }
    else {
    TEST_CASE_PRINT0( TEST: MULTIPLXED EVENTSET SCENARIO TEST);
    }


    int i,loop,muxevents=5,numevents;
    //////////////////////////////////////
    //   Overflow related variables     //
    //int normalize= BGPM_NORMAL;
    uint64_t period;
    //////////////////////////////////////
    
    int hNMEvtSet;
    int repeat=5;

    for(loop=0;loop<=2;loop++){
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
      hNMEvtSet = Bgpm_CreateEventSet();
      if(MUX){
      int normalize= BGPM_NORMAL;
      uint64_t period;
      period = 6000;
      TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMinEvts),0);
      }

     switch (loop){
      case 0:
             printf("\n\n  CHECKING FOR 6 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL6, sizeof(evtL6)/sizeof(unsigned));  
             break;
      case 1:
             printf("\n\n  CHECKING FOR 12  NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL12, sizeof(evtL12)/sizeof(unsigned));
             break;
      case 2:
             printf("\n\n  CHECKING FOR 23 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));
             break;
       }

      numevents=Bgpm_NumEvents(hNMEvtSet);

      TEST_CHECK_EQUAL(Bgpm_Apply(hNMEvtSet),0);
      #pragma omp barrier
      sampletest(); // warmup test
      #pragma omp barrier
      TEST_CHECK_EQUAL(Bgpm_Start(hNMEvtSet),0);
      sampletest();

      /* Verify Write/reset operations are invalid when event set is already running*/
      //TEST_CHECK_EQUAL(Bgpm_Reset(hNMEvtSet),0);                    // Throws an error and exits(enable for testing)
      //TEST_CHECK_EQUAL(Bgpm_WriteEvent(hNMEvtSet,1,2345),0);        // Throws an error and exits(enable for testing)
     
      #pragma omp barrier
      ReadEvents(hNMEvtSet,cnts_SinglePass);                          // Read Events after creating events through sampletest   
     // BgpmDebugPrintThreadVars(); 
     // Bgpm_PrintCurrentPunitReservations(hNMEvtSet);

      //-------------------------------------------------------------
      TEST_CASE_PRINT0(Check Whether the events getting reset after Bgpm_Stop);
      Bgpm_Stop(hNMEvtSet);      
      #pragma omp barrier
      TEST_CHECK_EQUAL(Bgpm_Reset(hNMEvtSet),0);                      // This Should Reset all counters             
      ReadEvents(hNMEvtSet,cnts_reset);                               //Expect all 0 values for all counters  
      ////////////////////////////////////////////////////
      //Try to write some value in the evnts and observe
      // the values getting incremented from there
      // ///////////////////////////////////////////////
      int value=444444; 
      #pragma omp barrier
      for(i=0;i<numevents;i++)
      {
        TEST_CHECK_EQUAL(Bgpm_WriteEvent(hNMEvtSet,i,value),0);
        value=value+40000;
       }    

      ReadEvents(hNMEvtSet,cnts_reset);    // Look whether events have been writeen with the value
      #pragma omp barrier
      TEST_CHECK_EQUAL(Bgpm_Start(hNMEvtSet),0);
      sampletest();
      ReadEvents(hNMEvtSet,cnts_reset);    // Look for the counter val modified based on the written value
      Bgpm_Stop(hNMEvtSet);
      TEST_CHECK_EQUAL(Bgpm_Reset(hNMEvtSet),0);
      ReadEvents(hNMEvtSet,cnts_reset);    // Again reset teh counters for resepat test    

      //--------------------------------------------------------------
      TEST_CASE_PRINT0(Run the sample test 2 times and observe the results);
      #pragma omp barrier
      sampletest();  // warmup test
      TEST_CHECK_EQUAL(Bgpm_Start(hNMEvtSet),0);
      #pragma omp barrier
      for(i=0;i<repeat;i++) {              //Do the same test (sampletets) for repeat number of times and observe tehe results   
         sampletest();
      } 
      #pragma omp barrier
      ReadEvents(hNMEvtSet,cnts_Multiple);
      #pragma omp barrier
      compare_counts(cnts_SinglePass,cnts_Multiple,numevents,repeat);
      //clear_counts(cnts_SinglePass,numevents);  
      clearcounts(cnts_Multiple,numevents);  
      TEST_CHECK_EQUAL(Bgpm_Disable(),0);  
     }
// Doing for loop again because once we do Bgpm_Apply() we cann't modify any event attributes,Need to Do a fresh start.
   //////////////////////Multiplex events could be 5,11,23////////////////////////////

   for(muxevents=0;muxevents<=2;muxevents++)
   {
    for(loop=0;loop<=2;loop++){
      TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
      hNMEvtSet = Bgpm_CreateEventSet();
      if(MUX){
      int normalize= BGPM_NORMAL;
      uint64_t period;
      period = 6000;
        if(muxevents == 0){
         TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMinEvts),0);
         printf("Mux Events: %d\n",BGPMMuxMinEvts);
        }
        else if(muxevents == 1){
          TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMedEvts),0);
         printf("Mux Events: %d\n",BGPMMuxMedEvts);
        }
        else if(muxevents == 2){
          TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMaxEvts),0);
         printf("Mux Events: %d\n",BGPMMuxMaxEvts);
        }
      }

     switch (loop){
      case 0:
             printf("\n\n  CHECKING FOR 6 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL6, sizeof(evtL6)/sizeof(unsigned));
             break;
      case 1:
             printf("\n\n  CHECKING FOR 12  NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL12, sizeof(evtL12)/sizeof(unsigned));
             break;
      case 2:
             printf("\n\n  CHECKING FOR 23 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));
             break;
       }

     
       numevents=Bgpm_NumEvents(hNMEvtSet);
       //-----------------------------------------------------------
       TEST_CASE_PRINT0(Looking at a smaller PERIOD value--100);
       period=100;
       TEST_CHECK_EQUAL(Bgpm_SetOverflow(hNMEvtSet,5,period),0);
       TEST_CHECK_EQUAL(Bgpm_SetOverflow(hNMEvtSet,2,period),0);
       TEST_CHECK_EQUAL(Bgpm_Apply(hNMEvtSet),0);
      #pragma omp barrier
       TEST_CHECK_EQUAL(Bgpm_Start(hNMEvtSet),0);
       sampletest();
       ReadEvents(hNMEvtSet,cnts_overflow);
       // compare_counts(cnts_SinglePass,cnts_overflow,numevents,repeat);
       // BgpmDebugPrintThreadVars(); 
       Bgpm_Disable(); 
   }
   if(!MUX){
   break; 
   }
 }


// Doing for loop again because once we do Bgpm_Apply() we cann't modify any event attributes,Need to Do a fresh start.
    for(muxevents=0;muxevents<=2;muxevents++)
    {
      for(loop=0;loop<=2;loop++){
         TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
          hNMEvtSet = Bgpm_CreateEventSet();

          if(MUX){
          int normalize= BGPM_NORMAL;
          uint64_t period;
          period = 6000;
          if(muxevents == 0){
          TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMinEvts),0);
          printf("Mux Events: %d\n",BGPMMuxMinEvts);
          }
          else if(muxevents == 1){
          TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMedEvts),0);
          printf("Mux Events: %d\n",BGPMMuxMedEvts);
          }
          else if(muxevents == 2){
          TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hNMEvtSet, period, normalize,BGPMMuxMaxEvts),0);
          printf("Mux Events: %d\n",BGPMMuxMaxEvts);
          }
         } 

         switch (loop){
          case 0:
             printf("\n\n  CHECKING FOR 6 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL6, sizeof(evtL6)/sizeof(unsigned));
             break;
          case 1:
             printf("\n\n  CHECKING FOR 12  NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL12, sizeof(evtL12)/sizeof(unsigned));
             break;
          case 2:
             printf("\n\n  CHECKING FOR 23 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hNMEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));
             break;
          }

        //----------------------------------------------------------
          numevents=Bgpm_NumEvents(hNMEvtSet);
          TEST_CASE_PRINT0(Looking at a large PERIOD value--60000);
          period=60000000;
          TEST_CHECK_EQUAL(Bgpm_SetOverflow(hNMEvtSet,5,period),0);
          TEST_CHECK_EQUAL(Bgpm_SetOverflow(hNMEvtSet,2,period),0);
          TEST_CHECK_EQUAL(Bgpm_Apply(hNMEvtSet),0);
          TEST_AT();
          #pragma omp barrier
          TEST_AT();
          TEST_CHECK_EQUAL(Bgpm_Start(hNMEvtSet),0);
          sampletest();
          //BgpmDebugPrintThreadVars(); 
          //Bgpm_PrintCurrentPunitReservations(hNMEvtSet);
          TEST_CHECK_EQUAL(Bgpm_Disable(hNMEvtSet),0);
         }
    if(!MUX){
    break;
    }
    }// end of for
}

void automaticmux_overflow_test()
{
   int hEvtSet,numevents,i,j,normalize,period; 
   TEST_CASE_PRINT0(AUTOMATIC MUX WITH OVERFLOW-Reset and Re-test);
   numevents=0;

   for(i=0;i<3;i++)
  {  
      TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
      hEvtSet = Bgpm_CreateEventSet();

      normalize= BGPM_NORMAL;
      period = 0;
      TEST_CHECK_EQUAL(Bgpm_SetMultiplex2(hEvtSet, period, normalize,BGPMMuxMaxEvts),0);

      switch (i){
      case 0:
             printf("\n\n  CHECKING FOR 6 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL6, sizeof(evtL6)/sizeof(unsigned));
             break;
      case 1:
             printf("\n\n  CHECKING FOR 12  NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL12, sizeof(evtL12)/sizeof(unsigned));
             break;
      case 2:
             printf("\n\n  CHECKING FOR 23 NUMBER OF EVENTS \n\n");
             Bgpm_AddEventList(hEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));
             break;
      }
       numevents=Bgpm_NumEvents(hEvtSet);
       period=10000;

      for(j=0;j<numevents;j++){                                  // Overflow is active on all events
        TEST_CHECK_EQUAL(Bgpm_SetOverflow(hEvtSet,j,period),0);
      }

      #pragma omp barrier
      TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);
      #pragma omp barrier
      TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0);
      sampletest();
      #pragma omp barrier
      ReadEvents(hEvtSet,cnts_reset);
      TEST_CHECK_EQUAL(Bgpm_Stop(hEvtSet),0);
      TEST_CHECK_EQUAL(Bgpm_Reset(hEvtSet),0); 
      #pragma omp barrier
      ReadEvents(hEvtSet,cnts_reset);
      TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0); 
      sampletest();
      #pragma omp barrier
      ReadEvents(hEvtSet,cnts_reset);
      TEST_CHECK_EQUAL(Bgpm_Disable(hEvtSet),0);
   } 
 }

int main(int argc, char *argv[])
{
   Bgpm_EnableTracePrint();
   int numThreads;
   numThreads = atoi(argv[1]);
   printf("Number of Threads: %d\n",numThreads);

   #pragma omp parallel default(none) num_threads(numThreads)    \
                        shared(stderr,numFailures)
   {
    MUX=BGPM_TRUE;
    OF=BGPM_TRUE;
    Attributes_test(MUX,OF);
   }

   #pragma omp parallel default(none) num_threads(numThreads)    \
                        shared(stderr,numFailures)
   {
    MUX=BGPM_FALSE;
    OF=BGPM_FALSE;
    Attributes_test(MUX,OF);
   }

   #pragma omp parallel default(none) num_threads(numThreads)    \
                        shared(stderr,numFailures)
   {
    Reset_Repeat_event_test(BGPM_FALSE);
   }

   #pragma omp parallel default(none) num_threads(numThreads)    \
                        shared(stderr,numFailures)
   {  
    Reset_Repeat_event_test(BGPM_TRUE);
   }

   #pragma omp parallel default(none) num_threads(numThreads)    \
                        shared(stderr,numFailures)
   {
    automaticmux_overflow_test();
   }
   TEST_RETURN();
}
