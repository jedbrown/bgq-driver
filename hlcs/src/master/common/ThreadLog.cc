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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#include "ThreadLog.h"

#include <utility/include/Log.h>

#include <iomanip>
#include <sstream>

LOG_DECLARE_FILE("master");


// We use pthread_self() because the boost thread id output
// format is not useful when comparing to log and debugger thread ids.
ThreadLog::ThreadLog(
        std::string name
        ):
        _name(name),
        _my_id(pthread_self())
{
    std::ostringstream os;
    os << std::setbase(16);
    os << _my_id;
    LOG_DEBUG_MSG("Starting " << _name << " [0x" << os.str() << "]");
}

ThreadLog::~ThreadLog()
{
    std::ostringstream os;
    os << std::setbase(16);
    os << _my_id;
    LOG_DEBUG_MSG("Stopping " << _name << " [0x" << os.str() << "]");
}
