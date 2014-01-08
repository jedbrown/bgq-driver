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

void AddEvent_test(int);
void AddEventList_test(int,unsigned int* measList);
void EventQueries_test(int);
void DeleteEventList_test(int);

int main(int argc, char *argv[])
{
     Bgpm_ExitOnError(BGPM_FALSE);  // don't fail process on error
     TEST_CHECK_EQUAL(Bgpm_Init(BGPM_MODE_SWDISTRIB),0);

     // set to look at Punit Counters
     int hPuSet = Bgpm_CreateEventSet();
     // define eventlist to be added
     unsigned measList[] = {
     PEVT_IU_IS1_STALL_CYC,
     PEVT_CYCLES,
     PEVT_INST_ALL,
     0, 0   // adding two 0 events so passing an excess list of events has known size and data space (prevent seg fault)
     };

     AddEvent_test(hPuSet);
     AddEventList_test(hPuSet,measList);
     EventQueries_test(hPuSet);
     DeleteEventList_test(hPuSet);
     TEST_RETURN();
}
void AddEvent_test(int hPuSet)
{
     TEST_FUNCTION(AddEvent() PARAMETER TEST);

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_AddEvent(hPuSet, PEVT_L1P_BAS_LD), 0);

     TEST_FUNCTION_PARM(Passing invalid event ID);
     TEST_CHECK_EQUAL(Bgpm_AddEvent(hPuSet, INVALID_PARM), BGPM_EINV_EVENT);

     TEST_FUNCTION_PARM(Passing invalid event set handle);
     TEST_CHECK_EQUAL(Bgpm_AddEvent(Dummy, PEVT_L1P_STRM_WRT_INVAL), BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Passing invalid event set handle and invalide eventID);
     TEST_CHECK_EQUAL(Bgpm_AddEvent(Dummy,INVALID_PARM), BGPM_EINV_SET_HANDLE);
}

void AddEventList_test(int hPuSet,unsigned int* measList)
{
     unsigned int *Dummy_ptr=NULL;
     int numMeasEvts = sizeof(measList) / sizeof(int);

     TEST_FUNCTION(AddEventList() PARAMETER TEST);

     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_AddEventList(Dummy, measList, numMeasEvts), BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Passing invalid event List);
     TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet, Dummy_ptr, numMeasEvts),  BGPM_EINV_PARM);

     // KDD - modification. The passed size is never invalid - it defines the list length.
     //       There is no way to know the list length otherwise.  Thus, when the list contains
     //       zero value events - its an invalid event error instead.
     TEST_FUNCTION_PARM(Improper Event List Size);
     // TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet, measList,5), BGPM_EINV_PARM);
     TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet, measList,5), BGPM_EINV_EVENT);

     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_AddEventList(hPuSet,measList,3),0);

}
void  EventQueries_test(int hPuSet)
{
     int numevents=0;
     char evtDetail[1024];
     int evtDetailSize = 1024;
     numevents= Bgpm_NumEvents(hPuSet);
     TEST_FUNCTION(NumEvents() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_NumEvents(Dummy),0);   // KDD - NumEvents designed to return 0 on error, makes easier to use by caller.
     TEST_FUNCTION_PARM(Num Events-Sucesscase);
     TEST_CHECK_EQUAL(Bgpm_NumEvents(hPuSet),4);

     TEST_FUNCTION(Bgpm_NumTargets() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid event set handle);
     TEST_CHECK_EQUAL(Bgpm_NumTargets(Dummy),0); //NumTargets designed to return 0 on error, makes easier to use by caller. 
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_NumTargets(hPuSet),1);

     TEST_FUNCTION(GetEventLabel() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid event set handle);
     //TEST_CHECK_EQUAL(Bgpm_GetEventLabel(Dummy,1),BGPM_EINV_SET_HANDLE);
     TEST_CHECK_EQUAL(Bgpm_GetEventLabel(Dummy,1),0); // KDD - GetEventLabel is designed to always return NULL on error
     TEST_FUNCTION_PARM(Event Index greater than availible events);
     //TEST_CHECK_EQUAL(Bgpm_GetEventLabel(hPuSet,7),BGPM_EINV_EVT_IDX)
     TEST_CHECK_EQUAL(Bgpm_GetEventLabel(hPuSet,7),0)
     TEST_FUNCTION_PARM(Invalid Event Handle and Index);
     //TEST_CHECK_EQUAL(Bgpm_GetEventLabel(Dummy,7),BGPM_EINV_SET_HANDLE);
     TEST_CHECK_EQUAL(Bgpm_GetEventLabel(Dummy,7),0);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_NONZERO(Bgpm_GetEventLabel(hPuSet,2));  // Returning String Need to verify TEST_CHECK_EQUAL

     TEST_FUNCTION(GetEventId Parameter Test);
     TEST_FUNCTION_PARM(Invalid event set handle);
     TEST_CHECK_EQUAL(Bgpm_GetEventId(Dummy,1),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Event Index greater than availible events);
     TEST_CHECK_EQUAL(Bgpm_GetEventId(hPuSet,7),BGPM_EINV_EVT_IDX)
     TEST_FUNCTION_PARM(Invalid Event Handle and Index);
     TEST_CHECK_EQUAL(Bgpm_GetEventId(Dummy,7),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Success Case);
     //TEST_CHECK_EQUAL(Bgpm_GetEventId(hPuSet,2),0);
     TEST_CHECK_EQUAL(Bgpm_GetEventId(hPuSet,2),PEVT_CYCLES);   // KDD GetEventID returns positive event ID value - not 0 for success.

     TEST_FUNCTION(GetEventIndex() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid Handle);
     TEST_CHECK_EQUAL(Bgpm_GetEventIndex(Dummy,PEVT_CYCLES,0),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(InvalidEventId);
     TEST_CHECK_EQUAL(Bgpm_GetEventIndex(hPuSet, INVALID_PARM,0), BGPM_EINV_EVENT);
     TEST_FUNCTION_PARM(Invalid Start Index);
     TEST_CHECK_EQUAL(Bgpm_GetEventIndex(hPuSet, PEVT_CYCLES,8), BGPM_EINV_EVT_IDX);
     TEST_FUNCTION_PARM(Invalid Handle,EventID and Start Index);
     TEST_CHECK_EQUAL(Bgpm_GetEventIndex(Dummy, INVALID_PARM,8), BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION(GetUnitType() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid Handle);
     TEST_CHECK_EQUAL(Bgpm_GetUnitType(Dummy),BGPM_EINV_SET_HANDLE);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_GetUnitType(hPuSet),1);

     TEST_FUNCTION(GetEventIdInfo() PARAMETER TEST);
     Bgpm_EventInfo_t einfo;
     TEST_FUNCTION_PARM(Invalid Handle);
     TEST_CHECK_EQUAL(Bgpm_GetEventIdInfo(Dummy,&einfo),BGPM_EUNREC_EVTID);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_GetEventIdInfo(1,&einfo),0);


     TEST_FUNCTION(GetEventIdLabel() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid EventID);
     //TEST_CHECK_EQUAL(Bgpm_GetEventIdLabel(8),BGPM_EINV_EVENT);  // KDD 8 is a valid event id
     TEST_CHECK_EQUAL(Bgpm_GetEventIdLabel(5000),0); // A large event num is invalid, but the error return is a NULL value
     TEST_FUNCTION_PARM(Success Case);
     //TEST_CHECK_EQUAL(Bgpm_GetEventIdLabel(PEVT_L1P_BAS_LD),0);
     TEST_CHECK_NONZERO(Bgpm_GetEventIdLabel(PEVT_L1P_BAS_LD));  // KDD Return is a strin pointer.

     TEST_FUNCTION(GetEventIdFromLabel() PARAMETER TEST);
     const char *p=NULL;
     TEST_FUNCTION_PARM(Invalid Event Label);
     TEST_CHECK_EQUAL(Bgpm_GetEventIdFromLabel(p),BGPM_EINV_PARM);

     TEST_FUNCTION(GetLongDesc() PARAMETER TEST);
     TEST_FUNCTION_PARM(Invalid Event ID);
     TEST_CHECK_EQUAL(Bgpm_GetLongDesc(INVALID_PARM,evtDetail,&evtDetailSize),BGPM_EUNREC_EVTID);
     TEST_FUNCTION_PARM(Success Case);
     TEST_CHECK_EQUAL(Bgpm_GetLongDesc(PEVT_L1P_BAS_LD,evtDetail,&evtDetailSize),0);
}

void DeleteEventList_test(int hPuSet)
{
     TEST_FUNCTION(DeleteEventSet() PARAMETER TEST);

     TEST_FUNCTION_PARM(Invalid Event handle);
     TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(Dummy), BGPM_EINV_SET_HANDLE);

     TEST_FUNCTION_PARM(Valid Event Set Handle);
     TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hPuSet),0);

     TEST_FUNCTION_PARM(Event Set Already Freed);
     TEST_CHECK_EQUAL(Bgpm_DeleteEventSet (hPuSet),BGPM_EINV_SET_HANDLE);

}
