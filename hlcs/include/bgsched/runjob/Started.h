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

/*!
 * \file bgsched/runjob/Started.h
 * \brief definition and implementation of bgsched::runjob::Started class.
 */

#ifndef BGSCHED_RUNJOB_STARTED_H
#define BGSCHED_RUNJOB_STARTED_H

#include <bgsched/Job.h>

#include <boost/shared_ptr.hpp>

#include <string>

#include <sys/types.h>

namespace bgsched {
namespace runjob {

/*!
 * \brief Data used to notify when a job is started.
 */
class Started
{
public:
    /*!
     * \brief Get pid.
     */
    pid_t pid() const;

    /*!
     * \brief Get job ID.
     */
    Job::Id job() const;

    /*!
     * \brief Implementation type.
     */
    class Impl;

    /*!
     * \brief Pointer to implementation type.
     */
    typedef boost::shared_ptr<Impl> Pimpl;

    /*!
     * \brief Ctor.
     */
    explicit Started(
            const Pimpl& impl      //!< [in] Pointer to implementation
            );

private:
    Pimpl _impl;
};

} // runjob
} // bgsched

#endif
