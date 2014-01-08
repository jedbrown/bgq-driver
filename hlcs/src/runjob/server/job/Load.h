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
#ifndef RUNJOB_SERVER_JOB_LOAD_H
#define RUNJOB_SERVER_JOB_LOAD_H

#include "server/cios/fwd.h"

#include "server/job/IoNode.h"

#include "server/fwd.h"

#include <ramdisk/include/services/JobctlMessages.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Load a job through the control and I/O services (cios) daemons.
 *
 * \see Job
 */
class Load : public boost::enable_shared_from_this<Load>
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Load> Ptr;

public:
    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Job>& job  //!< [in]
            );

private:
    Load(
            const boost::shared_ptr<Job>& job
        );

    void populate();

    void addUserCredentials(
            const boost::shared_ptr<bgcios::jobctl::LoadJobMessage>& message
            );

    void updateClient(
            bool flag
            );

private:
    const boost::shared_ptr<Job> _job;
    const boost::shared_ptr<cios::Message> _message;
};

} // job
} // server
} // runjob

#endif
