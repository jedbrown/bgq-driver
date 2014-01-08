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

#ifndef _BGPM_ENVVARS_H_  // Prevent multiple inclusion
#define _BGPM_ENVVARS_H_

#include <stdlib.h>

namespace bgpm {


typedef enum {
    BGPM_EVT_FEEDBACK_NONE = 0,
    BGPM_EVT_FEEDBACK_1ST,
    BGPM_EVT_FEEDBACK_PROCLEADER,
    BGPM_EVT_FEEDBACK_ALL,
} Bgpm_EventFeedback_Mode_t;



//! EnvVars
/*!
  Get Environment Vars Class

  One instance per process
*/
class EnvVars
{
public:
    EnvVars() {};
    ~EnvVars() {};

    Bgpm_Modes    GetEnvMode(Bgpm_Modes curMode);
    int           PMSignal(int sig);
    Bgpm_Context  GetDefContext(Bgpm_Context defCtx);
    int           GetSetsPerThread(int defSets);
    bool          PrintOnError() { return (getenv("BGPM_NO_PRINT_ON_ERR") == NULL); }
    bool          ExitOnError()  { return (getenv("BGPM_NO_EXIT_ON_ERR") == NULL); }
    bool          DumpOnError()  { return (getenv("BGPM_DUMP_ON_ERR") != NULL); }
    const char *  DescFile()     { return (getenv("BGPM_EVTDESC_FILE")); }
    short         AddEventErrFeedback();
    bool          GetTraceEnableAtStart() { return (getenv("BGPM_TRACESTART") != NULL); }
    int           GetTraceEnableLevel(int defLevel);  // not really used
    int           GetTraceDataEnableLevel(int defLevel);  // not really used


private:

    // hide these
    EnvVars(const EnvVars &);
    EnvVars & operator=(const EnvVars &);
};



}

#endif
