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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_SECURITY_CHECK_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_SECURITY_CHECK_H

#include "server/handlers/locate_rank/fwd.h"

#include "server/fwd.h"

#include <boost/shared_ptr.hpp>
#include <boost/weak_ptr.hpp>

#include <sstream>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Validates the user making the request is authorized.
 *
 * locate_rank is a strange command and doesn't fit into normal security guidelines for operating on job or
 * block objects. It can operate on both active and history blocks and jobs. This is somewhat problematic for
 * history jobs because granted authorites are not retained once the block has been deleted. To solve this for
 * locate_rank, it is assumed only the administrator or owner of the job can perform locate_rank on it.
 *
 * This class performs the follwing steps, in order
 *
 * - is user administrator?
 * - else is job currently active? Check for job read authority.
 * - else does user own the history job?
 * - else fail the security check
 *
 */
class SecurityCheck
{
public:
    /*!
     * \brief ctor.
     */
    SecurityCheck(
            const boost::weak_ptr<Server>& server,                  //!< [in]
            const boost::shared_ptr<CommandConnection>& connection  //!< [in]
            );

    /*!
     * \brief
     */
    bool operator()(
            const boost::shared_ptr<Job>& job   //!< [in]
            );

    std::string error() const { return _message.str(); }

private:
    bool impl(
            const boost::shared_ptr<ActiveJob>& job
            );
    
    bool impl(
            const boost::shared_ptr<HistoryJob>& job
            );

private:
    const boost::weak_ptr<Server> _server;
    const boost::shared_ptr<CommandConnection> _connection;
    std::ostringstream _message;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
