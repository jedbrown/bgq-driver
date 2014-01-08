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

#ifndef MMCS_ENV_IO_DRAWER_H
#define MMCS_ENV_IO_DRAWER_H

#include "Polling.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/io_service.hpp>

namespace MCServerMessageSpec {

class ReadIoCardEnvReply;

}

namespace mmcs {
namespace server {
namespace env {

void
processIO(
        const MCServerMessageSpec::ReadIoCardEnvReply* mcIOReply,
        const cxxdb::UpdateStatementPtr& fanInsert,
        const cxxdb::UpdateStatementPtr& ioCardInsert,
        const cxxdb::UpdateStatementPtr& nodeInsert,
        const cxxdb::UpdateStatementPtr& linkChipInsert
        );

class IoDrawer : public Polling
{
public:
    IoDrawer(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server
            );

    static cxxdb::ConnectionPtr prepareInserts(
            cxxdb::UpdateStatementPtr& fanInsert,
            cxxdb::UpdateStatementPtr& ioCardInsert,
            cxxdb::UpdateStatementPtr& nodeInsert,
            cxxdb::UpdateStatementPtr& linkChipInsert
            );

private:
    void makeTargetSetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Timer::Ptr& timer
        );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Timer::Ptr& timer
            );

    void readHandler(
            std::istream& response,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Timer::Ptr& timer
            );

    void closeTargetHandler(
            const boost::shared_ptr<MCServerMessageSpec::ReadIoCardEnvReply>& reply
            );
    
    std::string getDescription() const { return "IO card"; }

private:
    cxxdb::ConnectionPtr _connection;
    cxxdb::UpdateStatementPtr _fanInsert;
    cxxdb::UpdateStatementPtr _ioCardInsert;
    cxxdb::UpdateStatementPtr _nodeInsert;
    cxxdb::UpdateStatementPtr _linkChipInsert;
};

} } } // namespace mmcs::server::env

#endif
