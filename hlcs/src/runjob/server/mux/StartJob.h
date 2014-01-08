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
#ifndef RUNJOB_SERVER_MUX_START_JOB_H
#define RUNJOB_SERVER_MUX_START_JOB_H

#include "common/fwd.h"

#include "server/fwd.h"

#include <boost/shared_ptr.hpp>

#include <string>

namespace runjob {
namespace server {
namespace mux {

/*!
 * \brief
 */
class StartJob
{
public:
    /*!
     * \brief pointer type.
     */
    typedef boost::shared_ptr<StartJob> Ptr;

    /*!
     * \brief Factory.
     */
    static void create(
            const boost::shared_ptr<Server>& server,            //!< [in]
            const boost::shared_ptr<runjob::Message>& message,  //!< [in]
            const std::string& hostname                         //!< [in]
            );

private:
    StartJob(
            const boost::shared_ptr<runjob::Message>& message,  //!< [in]
            const std::string& hostname                         //!< [in]
         );

    void findJobHandler(
            const boost::shared_ptr<Job>& job
            );

private:
    const boost::shared_ptr<runjob::Message> _message;
    const std::string _hostname;
};

} // mux
} // server
} // runjob

#endif
