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
#include "cxxsockets/PollingSocketSet.h"

#include "Log.h"

LOG_DECLARE_FILE( "utility.cxxsockets" );

namespace CxxSockets {

PollingSocketSet::PollingSocketSet() :
    PollingFileSet()
{

}

void
PollingSocketSet::RemoveSock(
        const SocketPtr sock
        )
{ 
    PollingFileSet::RemoveFile(sock);
}

void
PollingSocketSet::pAddSock(
        const SocketPtr sock,
        const PollType p
        )
{
    PollingFileSet::pAddFile(sock, p);
}

void
PollingSocketSet::AddSock(
        const SocketPtr sock,
        const PollType p
        )
{
    PthreadMutexHolder mutex; LockSet(mutex);
    pAddSock(sock, p);
}

}
