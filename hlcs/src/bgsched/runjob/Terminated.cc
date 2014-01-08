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
#include <bgsched/runjob/Terminated.h>

#include "TerminatedImpl.h"

namespace bgsched {
namespace runjob {

Terminated::Terminated(
        const Pimpl& impl
        ) :
    _impl( impl )
{

}

pid_t
Terminated::pid() const
{
    return _impl->_pid;
}

Job::Id
Terminated::job() const
{
    return _impl->_job;
}

int
Terminated::status() const
{
    return _impl->_status;
}

bool
Terminated::kill_timeout() const
{
    return _impl->_killTimeout;
}

const Terminated::Nodes&
Terminated::software_error_nodes() const
{
    return _impl->_nodes;
}

const std::string&
Terminated::message() const
{
    return _impl->_message;
}

} // runjob
} // bgsched
