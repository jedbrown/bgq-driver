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
#ifndef BGSCHED_RUNJOB_STARTED_IMPL_H
#define BGSCHED_RUNJOB_STARTED_IMPL_H

#include <bgsched/runjob/Started.h>

#include <bgsched/Job.h>

namespace bgsched {
namespace runjob {

class Started::Impl
{
public:
    Impl();

    void pid( pid_t pid ) { _pid = pid; }
    void job( Job::Id job ) { _job = job; }

    pid_t pid() const { return _pid; }
    Job::Id job() const { return _job; }

private:
    pid_t _pid;
    Job::Id _job;
};

} // runjob
} // bgsched

#endif

