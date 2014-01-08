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
#ifndef RUNJOB_SERVER_JOB_START_H
#define RUNJOB_SERVER_JOB_START_H

#include "server/cios/fwd.h"

#include "server/fwd.h"

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

#include <cstdint>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Start a job through the Common I/O Services (cios) daemons.
 *
 * The job starts on the jobctl (control) daemons only after all stdio (output) 
 * daemons have reported a successful ack so the necessary data structures 
 * can be setup on the I/O node for handling output.
 *
 * \see Job
 */
class Start : public boost::enable_shared_from_this<Start>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Start> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job,  //!< [in]
            uint8_t service                     //!< [in]
            );

private:
    Start(
            const boost::shared_ptr<Job>& job,
            uint8_t service
        );

private:
    const boost::shared_ptr<Job> _job;
    const boost::shared_ptr<cios::Message> _message;
};

} // job
} // server
} // runjob

#endif
