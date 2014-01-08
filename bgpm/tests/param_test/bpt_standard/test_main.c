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


#define NUM_THREADS 68
#define INVALID_PARM -1
int Dummy=-1;

void Bgpm_Apply_test(int);
void Bgpm_Attach_test(int);
void Bgpm_Detach_test(int);
void Bgpm_IsController_test(int);
void Bgpm_Start_test(int);
void Bgpm_ResetStart_test(int);
void Bgpm_Stop_test(int);
void Bgpm_Reset_test(int);
void Bgpm_ReadEvent_test(int);
void Bgpm_ReadEventList_test(int);
void Bgpm_WriteEvent_test(int);
//void Bgpm_ReadThreadEvent_test(int);
void Bgpm_NW_ReadLinkEvent_test();
void Bgpm_L2_test();
void Bgpm_CNK_SampleEvent_test();

int main(int argc, char *argv[])
{
     Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_HWDISTRIB),0);

     // set to look at Punit Counters
     int hPuSet = Bgpm_CreateEventSet();
     // define eventlist to be added
     unsigned measList[] = {
     PEVT_IU_IS1_STALL_CYC,
     PEVT_CYCLES,
     PEVT_INST_ALL
     };
     int numMeasEvts = sizeof(measList) / sizeof(int);
     Bgpm_AddEventList(hPuSet, measList, numMeasEvts);

     Bgpm_Attach_test(hPuSet);
     Bgpm_Detach_test(hPuSet);
     Bgpm_Disable(); 

     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);
     hPuSet = Bgpm_CreateEventSet();
     Bgpm_AddEventList(hPuSet, measList, numMeasEvts);

     Bgpm_Apply_test(hPuSet);
     Bgpm_IsController_test(hPuSet);
     Bgpm_ResetStart_test(hPuSet);
     Bgpm_Start_test(hPuSet);
     Bgpm_Reset_test(hPuSet); 
     Bgpm_Stop_test(hPuSet);
     Bgpm_ReadEvent_test(hPuSet);
     Bgpm_ReadEventList_test(hPuSet);    
     Bgpm_WriteEvent_test(hPuSet);
     //Bgpm_ReadThreadEvent_test(hPuSet); // Seperate test case 
     Bgpm_NW_ReadLinkEvent_test();
     Bgpm_L2_test();
     Bgpm_CNK_SampleEvent_test(); 

     TEST_RETURN();
}
void Bgpm_Apply_test(int hPuSet)
{
     TEST_FUNCTION(Bgpm_Apply() PARAMETER TEST);

     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_Apply(Dummy), BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_Apply(hPuSet), 0);
}

void Bgpm_Attach_test(int hPuSet)
{
     TEST_FUNCTION(Bgpm_Attach() PARAMETER TEST);

     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_Attach(Dummy,UPC_NW_ALL_LINKS,0), BGPM_EINV_SET_HANDLE);

     //TEST_FUNCTION_PARM(Invalid parameter);
     //TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet,Dummy,0), BGPM_EINV_PARM); // Cannot test , Dummy parameter sets for all threads.

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_Attach(hPuSet,UPC_NW_ALL_LINKS,0), 0);
}

void Bgpm_Detach_test(int hPuSet)
{
     TEST_FUNCTION(Bgpm_Detach() PARAMETER TEST);
     
     TEST_FUNCTION_PARM(Invalid Event set handle);
     TEST_CHECK_EQUAL(Bgpm_Detach(Dummy),BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_Detach(hPuSet),0);
   
}

void Bgpm_IsController_test(int hPuSet)
{
     TEST_FUNCTION(Bgpm_IsControllerDetach() PARAMETER TEST);
     
     TEST_FUNCTION_PARM(Invalid Event set handle);
     TEST_CHECK_EQUAL(Bgpm_IsController(Dummy),BGPM_EINV_SET_HANDLE);
     
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_IsController(hPuSet),1);
}

void Bgpm_Start_test(int hPuSet)
{
     TEST_FUNCTION(Bgpm_Start() PARAMETER TEST);

     
     TEST_FUNCTION_PARM(Invalid Event set handle);
     TEST_CHECK_EQUAL(Bgpm_Start(Dummy),BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_Start(hPuSet),0);

     TEST_FUNCTION_PARM(Success Case Expecting BGPM already started error);
     TEST_CHECK_EQUAL(Bgpm_Start(hPuSet),0);
}

void Bgpm_ResetStart_test(int hPuSet)
{
    TEST_FUNCTION(Bgpm_ResetStart() PARAMETER TEST);

    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_ResetStart(Dummy),BGPM_EINV_SET_HANDLE);

    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_ResetStart(hPuSet),0);
}

void Bgpm_Stop_test(int hPuSet)
{
    TEST_FUNCTION(Bgpm_Stop() PARAMETER TEST);

    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_Stop(Dummy),BGPM_EINV_SET_HANDLE);

    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_Stop(hPuSet),0);

    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_Stop(hPuSet),BGPM_WALREADY_STOPPED);
}

void Bgpm_Reset_test(int hPuSet)
{
    TEST_FUNCTION(Bgpm_Reset() PARAMETER TEST);

    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_Reset(Dummy),BGPM_EINV_SET_HANDLE);

    TEST_FUNCTION_PARM(EventSet Already running);
    TEST_CHECK_EQUAL(Bgpm_Reset(hPuSet),BGPM_EUPDATE_DISALLOWED);
}

void Bgpm_ReadEvent_test(int hPuSet)
{
    uint64_t is1stall_cyc;
    TEST_FUNCTION(Bgpm_ReadEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case); 
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(hPuSet, 0, &is1stall_cyc),0);
    TEST_FUNCTION_PARM(Invalid Event Set Handle); 
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(Dummy, 0, &is1stall_cyc),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index); 
    TEST_CHECK_EQUAL(Bgpm_ReadEvent(hPuSet, Dummy, &is1stall_cyc),BGPM_EINV_EVT_IDX);
}

void Bgpm_ReadEventList_test(int hPuSet)
{
     uint64_t ctrs[3];
     uint64_t ctr_dummy[2];
     unsigned numCtrs = 3;
     unsigned numCtrs_dummy = 1;
     TEST_FUNCTION(ReadEventList() PARAMETER TEST);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_ReadEventList(hPuSet, ctrs, &numCtrs),0);
     TEST_FUNCTION_PARM(Invalid Event Set Handle);
     TEST_CHECK_EQUAL(Bgpm_ReadEventList(Dummy, ctrs, &numCtrs),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Invalid counter size);
     TEST_CHECK_EQUAL(Bgpm_ReadEventList(hPuSet, ctr_dummy, &numCtrs_dummy),BGPM_EARRAY_TOO_SMALL);
}
void Bgpm_WriteEvent_test(int hPuSet)
{
    TEST_FUNCTION(WriteEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);   
    TEST_CHECK_EQUAL(Bgpm_WriteEvent(hPuSet, 0, 500), 0);
    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_WriteEvent(Dummy, 0, 500),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_WriteEvent(hPuSet,Dummy,500),BGPM_EINV_EVT_IDX);
}


void Bgpm_ReadThreadEvent_test(int hPuSet)
{
    uint64_t count;
    uint64_t appMask   = UPC_APP_HWTID_MASK(Kernel_ProcessorID());
    uint64_t agentMask = UPC_AGENT_HWTID_MASK(Kernel_ProcessorID());
    TEST_FUNCTION(ReadThreadEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hPuSet,0, appMask, agentMask, &count),0); 
    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(Dummy,0,appMask, agentMask, &count),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_ReadThreadEvent(hPuSet,0,Dummy , agentMask, &count),BGPM_EINV_EVT_IDX); 
}

void Bgpm_NW_ReadLinkEvent_test()
{
   
    int hNWSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_SENT);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV);
    Bgpm_AddEvent(hNWSet, PEVT_NW_USER_PP_RECV_FIFO);
    Bgpm_AddEvent(hNWSet, PEVT_NW_CYCLES);

    //UPC_NW_Dump();
    //
     Bgpm_Attach(hNWSet, UPC_NW_ALL_TORUS_LINKS, 0);

    TEST_FUNCTION(ReadThreadEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    uint16_t link = 0;
    uint64_t act = 0;
    uint64_t lmask = UPC_NW_LINK_bit(link);
     
    TEST_CHECK_EQUAL( Bgpm_NW_ReadLinkEvent(hNWSet, 0, (UPC_NW_LinkMasks)lmask, &act),0);
    TEST_FUNCTION_PARM(Invalid Event Handle);
    TEST_CHECK_EQUAL( Bgpm_NW_ReadLinkEvent(Dummy, 0, (UPC_NW_LinkMasks)lmask, &act),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL( Bgpm_NW_ReadLinkEvent(hNWSet, Dummy, (UPC_NW_LinkMasks)lmask, &act),BGPM_EINV_EVT_IDX);
}

void Bgpm_L2_test()
{

    int hEvtSet = Bgpm_CreateEventSet();
    Bgpm_AddEvent(hEvtSet, PEVT_L2_HITS);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_MISSES);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_PREFETCH);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_FETCH_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_LINE);
    Bgpm_AddEvent(hEvtSet, PEVT_L2_STORE_PARTIAL_LINE);

   // TEST_CHECK_EQUAL(Bgpm_NumEvents(hEvtSet), 6);
    Bgpm_Apply(hEvtSet);  // should have no effect for L2
    
    uint64_t pval;
    TEST_FUNCTION(Bgpm_L2_ReadEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_L2_ReadEvent(hEvtSet,0,1,&pval),0); 
    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_L2_ReadEvent(Dummy,0,1,&pval),BGPM_EINV_SET_HANDLE); 
    TEST_FUNCTION_PARM(Invalid slice value);
    TEST_CHECK_EQUAL(Bgpm_L2_ReadEvent(hEvtSet,Dummy,1,&pval),BGPM_EINV_L2_SLICE); 
    TEST_FUNCTION_PARM(Invalid event Index );
    TEST_CHECK_EQUAL(Bgpm_L2_ReadEvent(hEvtSet,0,Dummy,&pval),BGPM_EINV_EVT_IDX); 

    TEST_FUNCTION(Bgpm_L2_WriteEvent() PARAMETER TEST);
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_L2_WriteEvent(hEvtSet,0,1,pval),0);
    TEST_FUNCTION_PARM(Invalid Event set handle);
    TEST_CHECK_EQUAL(Bgpm_L2_WriteEvent(Dummy,0,1,pval),BGPM_EINV_SET_HANDLE);
    TEST_FUNCTION_PARM(Invalid slice value);
    TEST_CHECK_EQUAL(Bgpm_L2_WriteEvent(hEvtSet,Dummy,1,pval),BGPM_EINV_L2_SLICE);
    TEST_FUNCTION_PARM(Invalid event Index );
    TEST_CHECK_EQUAL(Bgpm_L2_WriteEvent(hEvtSet,0,Dummy,pval),BGPM_EINV_EVT_IDX);
}

void Bgpm_CNK_SampleEvent_test()
{
    unsigned cnkList[] = {      // List of performance events to collect
        PEVT_CNKHWT_SYSCALL,
        PEVT_CNKHWT_STANDARD,
        PEVT_CNKHWT_CRITICAL,
        PEVT_CNKHWT_DECREMENTER,
        PEVT_CNKHWT_FIT,
        PEVT_CNKHWT_WATCHDOG,
        PEVT_CNKHWT_UDEC,
        PEVT_CNKHWT_PERFMON,
        PEVT_CNKHWT_UNKDEBUG
    };
    int cnkNumEvts = sizeof(cnkList) / sizeof(unsigned);
    int hCnkEvtSet = Bgpm_CreateEventSet();
    TEST_CHECK_EQUAL(Bgpm_AddEventList(hCnkEvtSet, cnkList, cnkNumEvts), 0);

    uint64_t startSample;
    TEST_FUNCTION(Bgpm_CNK_SampleEvent() PARAMETER TEST); 
    TEST_FUNCTION_PARM(Success Case);
    TEST_CHECK_EQUAL(Bgpm_CNK_SampleEvent(hCnkEvtSet, 0, &startSample),0); 
    TEST_FUNCTION_PARM(Invalid Event Set Handle);
    TEST_CHECK_EQUAL(Bgpm_CNK_SampleEvent(Dummy, 0, &startSample),BGPM_EINV_SET_HANDLE); 
    TEST_FUNCTION_PARM(Invalid Event Index);
    TEST_CHECK_EQUAL(Bgpm_CNK_SampleEvent(hCnkEvtSet, Dummy, &startSample),BGPM_EINV_EVT_IDX); 
}

