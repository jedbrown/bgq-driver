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


TEST_INIT();
int OF=0;
int MUX=0;     // This parameter specifies whether to enable/disable multiplexing


 unsigned evtL23[] = {
    PEVT_AXU_FP_DS_ACTIVE,         //0
    PEVT_AXU_FP_EXCEPT,            //1
    PEVT_INST_QFPU_FPGRP2,         //2 
    PEVT_WAKE_HIT_10,              //3    
    PEVT_INST_XU_FST,              //4
    PEVT_INST_QFPU_FMUL,           //5
    PEVT_INST_QFPU_QCVT,           //6
    PEVT_XU_TIMEBASE_TICK,         //7  
    PEVT_INST_QFPU_FPGRP1,         //8   
    PEVT_XU_RUN_INSTR,             //9    
    PEVT_INST_XU_FLD,              //10
    PEVT_CYCLES,                   //11      
    PEVT_IU_RSV_ANY_L2_BACK_INV,   //12
    PEVT_INST_QFPU_QRND,           //13
    PEVT_INST_QFPU_QADD,           //14   
    PEVT_WAKE_HIT_11,              //15
    PEVT_INST_QFPU_QCVT,           //16
    PEVT_INST_XU_FLD,              //17  
    PEVT_XU_ICSWX_COMMIT,          //18
    PEVT_INST_QFPU_GRP_MASK,       //19
    PEVT_L1P_BAS_LU_STALL_LIST_RD, //20
    PEVT_IU_IU0_REDIR_CYC,         //21
    PEVT_XU_ICSWX_FAILED,          //22
    PEVT_INST_XU_CTCH              //23
   };
   
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
  printf("Executed Sampletest \n"); 
}

void eventlistLen_test(int MUX,int setoverflow)
{
    TEST_CASE_PRINT0( TEST: NON-MULTIPLXED EVENTSET LENGTH TEST);
    int hEvtSet,numevents;
    TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
    hEvtSet= Bgpm_CreateEventSet();


    printf("\n\n  CHECKING FOR 24 NUMBER OF EVENTS \n\n");
    Bgpm_AddEventList(hEvtSet, evtL23, sizeof(evtL23)/sizeof(unsigned));

    numevents=Bgpm_NumEvents(hEvtSet);
    printf("NumEvents = %d\n", Bgpm_NumEvents(hEvtSet));


    //TEST_CHECK_EQUAL(Bgpm_AddEvent(hEvtSet, PEVT_L1P_BAS_ST_WC),0);   // Add 25th event  te check the error scenario
    TEST_CHECK_EQUAL(Bgpm_Apply(hEvtSet),0);
    TEST_CHECK_EQUAL(Bgpm_Start(hEvtSet),0);
    sampletest();
    Bgpm_Stop(hEvtSet);
    TEST_CHECK_EQUAL(Bgpm_Disable(),0);
}
    

int main(int argc, char *argv[])
{
    Bgpm_EnableTracePrint();

    MUX=BGPM_FALSE;
    OF=BGPM_TRUE;
    eventlistLen_test(MUX,OF);
    TEST_RETURN();     
}

