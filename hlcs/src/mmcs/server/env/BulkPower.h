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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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

#ifndef MMCS_ENV_BULK_POWER_H
#define MMCS_ENV_BULK_POWER_H

#include "Polling.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/io_service.hpp>

namespace MCServerMessageSpec {

class ReadBulkPowerEnvReply;

}

namespace mmcs {
namespace server {
namespace env {

void
processBulks(
        const MCServerMessageSpec::ReadBulkPowerEnvReply* mcBPReply,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& bulkInsert
        );

class BulkPower : public Polling
{
public:
    BulkPower(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server //!< [in]
            );

    static cxxdb::ConnectionPtr prepareInserts(
            cxxdb::UpdateStatementPtr& bulkInsert
            );

private:
    void makeTargetSetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& bulkInsert,
            const Timer::Ptr& timer
        );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& bulkInsert,
            const Timer::Ptr& timer
            );

    void readHandler(
            std::istream& response,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& bulkInsert,
            const Timer::Ptr& timer
            );

    std::string getDescription() const { return "bulk power"; }
};

} } } // namespace mmcs::server::env

#endif
