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

#ifndef MMCS_ENV_SERVICE_CARD_H
#define MMCS_ENV_SERVICE_CARD_H

#include "Polling.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/io_service.hpp>

namespace MCServerMessageSpec {

class ReadServiceCardEnvReply;

}

namespace mmcs {
namespace server {
namespace env {

void
processSC(
        const MCServerMessageSpec::ReadServiceCardEnvReply* mcSCReply,
        const cxxdb::UpdateStatementPtr& serviceInsert
        );

class ServiceCard: public Polling
{
public:
    ServiceCard(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server  //!< [in]
            );

    static cxxdb::ConnectionPtr prepareInsert(
            cxxdb::UpdateStatementPtr& serviceInsert
            );

private:
    void makeTargetSetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& insert,
            const Timer::Ptr& timer
            );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& insert,
            const Timer::Ptr& timer
            );

    void readHandler(
            std::istream& response,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& insert,
            const Timer::Ptr& timer
            );

    std::string getDescription() const { return "service card"; }
};

} } } // namespace mmcs::server::env

#endif
