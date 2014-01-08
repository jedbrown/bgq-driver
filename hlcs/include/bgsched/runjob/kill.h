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

/*
 */

/*!
 * \file bgsched/runjob/kill.h
 * \brief kill function definition.
 *
 * \ingroup V1R1M1
 *
 */

#ifndef BGSCHED_RUNJOB_KILL_H
#define BGSCHED_RUNJOB_KILL_H

#include <bgsched/Job.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief Deliver a signal to a job.
 *
 * \throws bgsched::InputException with values:
 * - bgsched::InputErrors::JobNotFound
 *
 * \throws bgsched::runjob::ConnectionException 
 * 
 * \throws bgsched::InternalException
 *
 * \returns 0 if the signal was delivered
 * \returns -1 otherwise
 *
 * \note This method was added in V1R1M1.
 * \ingroup V1R1M1
 * 
 */
int kill(
        Job::Id job,    //!< [in] job ID
        int signal      //!< [in] signal to deliver
        );

} // runjob
} // bgsched

#endif
