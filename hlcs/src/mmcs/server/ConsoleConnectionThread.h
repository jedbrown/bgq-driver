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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_SERVER_CONSOLE_CONNECTION_THREAD_H_
#define MMCS_SERVER_CONSOLE_CONNECTION_THREAD_H_

#include "common/Thread.h"

namespace mmcs {
namespace server {

/*!
 *  \brief execute commands from MMCS console connections.
 */
class ConsoleConnectionThread : public common::Thread
{
public:
    ConsoleConnectionThread() : Thread() {};
    void* threadStart();
};

} } // namespace mmcs::server


#endif
