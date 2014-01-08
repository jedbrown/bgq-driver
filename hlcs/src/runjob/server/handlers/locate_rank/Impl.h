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
#ifndef RUNJOB_SERVER_HANDLERS_LOCATE_RANK_IMPL_H
#define RUNJOB_SERVER_HANDLERS_LOCATE_RANK_IMPL_H

#include "common/commands/LocateRank.h"

#include "server/handlers/locate_rank/fwd.h"

#include "server/CommandHandler.h"

#include "server/fwd.h"

#include <boost/shared_ptr.hpp>

#include <sstream>

namespace runjob {
namespace server {
namespace handlers {
namespace locate_rank {

/*!
 * \brief Handles the locate_rank command.
 *
 * The input to this command is a job ID and rank. The job can either be
 * located in the bgqjob or bgqjob_history tables, but not both. After finding
 * the job this command then finds the block in use. For active jobs, the block
 * will always be in the bgqblock table. For history jobs, the block may still
 * be in bgqblock, or it may have been moved to bgqblock history. This is 
 * determined by using the block's entry into bgqblock_history and the job's
 * start time.
 *
 * Once the block has been found, the algorithm for converting a rank into a 
 * location is fairly simple. The coordinates are calculated based on the 
 * rank using the mapping permutation (ABCDET) and job shape. From the coordinates,
 * the coordinates within the midplane in question can be deduced. The midplane
 * coordinates can be converted to node board and compute card locations
 * using the BGQTopology class provided by the bgqconfig library.
 *
 * \section unit_tests Unit Tests
 *
 * There is a series of unit tests in the runjob/test/server/locate_rank directory
 * that needs to be explicitly made (with make test) since it assumes a certain
 * database schema has been populated.
 *
 * \see Block
 * \see Job
 */
class Impl : public CommandHandler
{
public:
    /*!
     * \brief ctor.
     */
    Impl(
            const boost::shared_ptr<Server>& server          //!< [in]
        );

    /*!
     * \brief dtor.
     */
    ~Impl();

    /*!
     * \brief Handle the command.
     */
    void handle(
            const runjob::commands::Request::Ptr& request,          //!< [in]
            const boost::shared_ptr<CommandConnection>& connection  //!< [in]
            );
    
    /*!
     * \brief Get the user type.
     */
    bgq::utility::portConfig::UserType::Value getUserType() const { return bgq::utility::portConfig::UserType::Normal; }

private:
    void validateMappingFile(
            const boost::shared_ptr<Job>& job
            );

private:
    runjob::commands::request::LocateRank::Ptr _request;
    const runjob::commands::response::LocateRank::Ptr _response;
    boost::shared_ptr<CommandConnection> _connection;
    runjob::commands::error::rc _error;
    std::ostringstream _message;
};

} // locate_rank
} // handlers
} // server
} // runjob

#endif
