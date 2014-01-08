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




int main(int argc, char *argv[]) {

    TEST_MODULE(Print Event Info for all events);

    Bgpm_Init(BGPM_MODE_SWDISTRIB);

    unsigned evtId;
    Bgpm_EventInfo_t einfo;
    char evtDetail[1024];

    printf(" evtId,                              label,    longdesc\n");
    for (evtId=1; evtId<=PEVT_LAST_EVENT; evtId++) {
        int evtDetailSize = 1024;
        Bgpm_GetEventIdInfo(evtId, &einfo);
        Bgpm_GetLongDesc(evtId, evtDetail, &evtDetailSize);

        printf("%4d  %40s  %s\n", evtId, einfo.label, evtDetail);
    }

    TEST_RETURN();
}

