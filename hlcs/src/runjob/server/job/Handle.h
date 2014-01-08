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
#ifndef RUNJOB_SERVER_JOB_HANDLE_H
#define RUNJOB_SERVER_JOB_HANDLE_H

#include "common/Uci.h"

#include "server/cios/fwd.h"

#include "server/mux/fwd.h"

#include "server/fwd.h"

#include <ramdisk/include/services/MessageHeader.h>

#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>

#include <string>

namespace runjob {
namespace server {
namespace job {

/*!
 * \brief Handle a message from an I/O connection.
 */
class Handle
{
public:
    /*!
     * \brief Pointer type.
     */
    typedef boost::shared_ptr<Handle> Ptr;

    /*!
     * \brief
     */
    typedef boost::function<void()> Callback;

public:
    /*!
     * \brief ctor.
     */
    Handle(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

private:
    friend class cios::Connection;
    
    void control(
            const Uci& location,
            const boost::shared_ptr<cios::Message>& message
            );
 
    void data(
            const Uci& location,
            const boost::shared_ptr<cios::Message>& message,
            const Callback& callback
            );

    void transition(
            const bgcios::MessageHeader* header
            );

private:
    const boost::shared_ptr<Job> _job;
};

} // job
} // server
} // runjob

#endif
