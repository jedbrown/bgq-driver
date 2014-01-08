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
#ifndef BGSCHED_RUNJOB_TERMINATED_IMPL_H
#define BGSCHED_RUNJOB_TERMInATED_IMPL_H

#include <bgsched/runjob/Terminated.h>

#include <bgsched/Job.h>
#include <string>

#include <sys/types.h>

namespace bgsched {
namespace runjob {

class Terminated::Impl
{
public:
    Impl();

    pid_t _pid;                     //!< pid of the runjob process.
    Job::Id _job;                   //!< Job ID.
    int _status;                    //!< exit status.
    bool _killTimeout;              //!<
    Nodes _nodes;                   //!<
    std::string _message;           //!<
};

} // runjob
} // bgsched

#endif

