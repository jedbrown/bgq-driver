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

//#define TEST_CASE_VERBOSE
#include "../../test_utils.h"

TEST_INIT();



void CheckUnit(int evtId, unsigned *evtList, unsigned evtListSize)
{
    uint64_t value;

     // Unassigned event set.
    int hEvtSet = Bgpm_CreateEventSet();

    Bgpm_AddEvent(hEvtSet, evtId);
    Bgpm_AddEvent(hEvtSet, evtId+1);
    Bgpm_AddEvent(hEvtSet, evtId+2);
    Bgpm_AddEvent(hEvtSet, evtId+3);

    //default event set values
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)0);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)0);

    //default event values
    unsigned i;
    for (i=0; i<4; i++) {
        TEST_CHECK_EQUAL(Bgpm_GetEventUser1(hEvtSet, i, &value), 0);
        TEST_CHECK_EQUAL(value, 0);
        TEST_CHECK_EQUAL(Bgpm_GetEventUser2(hEvtSet, i, &value), 0);
        TEST_CHECK_EQUAL(value, 0);
    }

    // invalid event index
    TEST_CHECK_EQUAL(Bgpm_GetEventUser1(hEvtSet, 5, &value), BGPM_EINV_EVT_IDX);
    TEST_CHECK_EQUAL(Bgpm_GetEventUser2(hEvtSet, 5, &value), BGPM_EINV_EVT_IDX);
    TEST_CHECK_EQUAL(Bgpm_SetEventUser1(hEvtSet, 5, 1), BGPM_EINV_EVT_IDX);
    TEST_CHECK_EQUAL(Bgpm_SetEventUser2(hEvtSet, 5, 1), BGPM_EINV_EVT_IDX);


    // assign values - last one wins.
    Bgpm_SetEventSetUser1(hEvtSet, 100);
    Bgpm_SetEventSetUser2(hEvtSet, 200);
    Bgpm_SetEventSetUser1(hEvtSet, 300);
    Bgpm_SetEventSetUser2(hEvtSet, 400);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet, &value), 0);
    TEST_CHECK_EQUAL(value, 300);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet, &value), 0);
    TEST_CHECK_EQUAL(value, 400);

    // Assigned event values
    for (i=0; i<4; i++) {
        Bgpm_SetEventUser1(hEvtSet, i, 100+i);
        Bgpm_SetEventUser2(hEvtSet, i, 200+i);
    }
    for (i=0; i<4; i++) {
        TEST_CHECK_EQUAL(Bgpm_GetEventUser1(hEvtSet, i, &value), 0);
        TEST_CHECK_EQUAL(value, 100+i);
        TEST_CHECK_EQUAL(Bgpm_GetEventUser2(hEvtSet, i, &value), 0);
        TEST_CHECK_EQUAL(value, 200+i);
    }

    // Create another event set -
    TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet), 0);
    int hEvtSet2 = Bgpm_CreateEventSet();

    // add Event Set values before set type determined - should be propogated to new set type.
    TEST_CHECK_EQUAL(Bgpm_SetEventSetUser1(hEvtSet2, 1), 0);
    TEST_CHECK_EQUAL(Bgpm_SetEventSetUser2(hEvtSet2, 2), 0);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet2, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)1);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet2, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)2);

    TEST_CHECK_EQUAL(Bgpm_AddEvent(hEvtSet, evtId), 0);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet2, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)1);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet2, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)2);

    // Try user 1, 2 for AddEventList
    TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet2), 0);
    int hEvtSet3 = Bgpm_CreateEventSet();

    // add Event Set values before set type determined - should be propogated to new set type.
    TEST_CHECK_EQUAL(Bgpm_SetEventSetUser1(hEvtSet3, 3), 0);
    TEST_CHECK_EQUAL(Bgpm_SetEventSetUser2(hEvtSet3, 4), 0);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet3, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)3);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet3, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)4);

    //Bgpm_DumpEvtSet(hEvtSet3, 0);

    TEST_CHECK_EQUAL(Bgpm_AddEventList(hEvtSet3, evtList, evtListSize), 0);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet3, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)3);
    TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet3, &value), 0);
    TEST_CHECK_EQUAL(value, (unsigned)4);

    TEST_CHECK_EQUAL(Bgpm_DeleteEventSet(hEvtSet3), 0);

}






int main(int argc, char *argv[]) {

    TEST_MODULE(Bgpm User Attributes);
    fprintf(stderr, "This test will produce some expected error messages\n");

    Bgpm_Init(BGPM_MODE_SWDISTRIB);
    Bgpm_ExitOnError(BGPM_FALSE);

    TEST_CASE( unassigned sets );
    {
        uint64_t value;

        // no event set.
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(0, &value), BGPM_EINV_SET_HANDLE);
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(0, &value), BGPM_EINV_SET_HANDLE);


        // Unassigned event set.
        int hEvtSet = Bgpm_CreateEventSet();
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet, &value), 0);
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet, &value), 0);
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser1(hEvtSet, NULL), BGPM_EINV_PARM);
        TEST_CHECK_EQUAL(Bgpm_GetEventSetUser2(hEvtSet, NULL), BGPM_EINV_PARM);
    }



    TEST_CASE( punit_events );
    {
        unsigned evtList[2] = { PEVT_CYCLES, PEVT_CYCLES };
        CheckUnit(PEVT_CYCLES, evtList, 2);
    }

    TEST_CASE( l2_events );
    {
        unsigned evtList[2] = { PEVT_L2_HITS, PEVT_L2_HITS };
        CheckUnit(PEVT_L2_HITS, evtList, 2);
    }

    TEST_CASE( IO_events );
    {
        unsigned evtList[2] = { PEVT_MU_PKT_INJ, PEVT_MU_PKT_INJ };
        CheckUnit(PEVT_MU_PKT_INJ, evtList, 2);
    }

    TEST_CASE( NW_events );
    {
        unsigned evtList[2] = { PEVT_NW_USER_PP_SENT, PEVT_NW_USER_PP_SENT+1 };
        CheckUnit(PEVT_NW_USER_PP_SENT, evtList, 2);
    }

#if 0
    TEST_CASE( CNK_events );
    {
        int evtList[2] = { PEVT_NW_USER_PP_SENT, PEVT_NW_USER_PP_SENT };
        CheckUnit(PEVT_NW_USER_PP_SENT, evtList, 2);
    }

    TEST_CASE( Env_events );
    {
        int evtList[2] = { PEVT_NW_USER_PP_SENT, PEVT_NW_USER_PP_SENT };
        CheckUnit(PEVT_NW_USER_PP_SENT, evtList, 2);
    }

    TEST_CASE( MC_events );
    {
        int evtList[2] = { PEVT_NW_USER_PP_SENT, PEVT_NW_USER_PP_SENT };
        CheckUnit(PEVT_NW_USER_PP_SENT, evtList, 2);
    }
#endif

    TEST_RETURN();
}

