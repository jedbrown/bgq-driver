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

#include <stdlib.h>
#include "globals.h"


using namespace bgpm;


Bgpm_Modes EnvVars::GetEnvMode(Bgpm_Modes curMode)
{
    BGPM_TRACE_L1;

    if (curMode == BGPM_MODE_DEFAULT) {
        curMode = BGPM_MODE_SWDISTRIB;

        // env var doesn't exist - keep default.
        char *strgMode = getenv("BGPM_MODE");
        if (strgMode != NULL) {

            BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " envvar BGPM_MODE=\"%s\"\n", IND_STRG, strgMode));

            // mode returns 0 (default or problem in mode) - keep default
            int numMode = atoi(strgMode);
            if ((numMode > 0) && (numMode < BGPM_MAX_MODES)) { //
                curMode = (Bgpm_Modes)numMode;
            }
        }
    }

    return curMode;
}



int EnvVars::PMSignal(int sig)
{
    BGPM_TRACE_L1;

    // env var doesn't exist - keep default.
    char *strg = getenv("BGPM_SIGNAL");
    if (strg != NULL) {

        BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " envvar BGPM_SIGNAL=\"%s\"\n", IND_STRG, strg));

        int envSig = atoi(strg);
        if (envSig > 0) {
            sig = envSig;
        }
    }
    return sig;
}



short EnvVars::AddEventErrFeedback()
{
    BGPM_TRACE_L1;

    char *cval = getenv("BGPM_EVT_FEEDBACK");
    if (cval == NULL) addEventErrFeedback = BGPM_EVT_FEEDBACK_1ST;
    else {

        BGPM_TRACE_DATA_L1(fprintf(stderr, "%s" _AT_ " envvar BGPM_EVT_FEEDBACK=\"%s\"\n", IND_STRG, cval));

        int val = atoi(cval);
        if ((val >= BGPM_EVT_FEEDBACK_NONE) && (val <= BGPM_EVT_FEEDBACK_ALL)) {
            addEventErrFeedback = val;
        }
    }
    return addEventErrFeedback;
}



Bgpm_Context  EnvVars::GetDefContext(Bgpm_Context defCtx)
{
    Bgpm_Context ret = defCtx;
    char *cval = getenv("BGPM_CONTEXT");
    if (cval != NULL) {
        Bgpm_Context ret = (Bgpm_Context)atoi(cval);
        if (ret > BGPM_CTX_BOTH) { ret = defCtx; }
    }
    return ret;
}



int EnvVars::GetSetsPerThread(int defSets)
{
    char *cval = getenv("BGPM_SETS_PER_THREAD");
    if (cval != NULL) {
        int ret = atoi(cval);
        if (ret > 0) { defSets = ret; }
    }
    return defSets;
}



int EnvVars::GetTraceEnableLevel(int defLevel)
{
    char *cval = getenv("BGPM_TRACE");
    if (cval != NULL) {
        int ret = atoi(cval);
        if (ret > 0) { defLevel = ret; }
    }
    return defLevel;
}


int EnvVars::GetTraceDataEnableLevel(int defLevel)
{
    char *cval = getenv("BGPM_TRACEDATA");
    if (cval != NULL) {
        int ret = atoi(cval);
        if (ret > 0) { defLevel = ret; }
    }
    return defLevel;
}
