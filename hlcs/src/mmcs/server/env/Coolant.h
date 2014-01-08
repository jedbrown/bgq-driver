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

#ifndef MMCS_ENV_COOLANT_H
#define MMCS_ENV_COOLANT_H

#include "Polling.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/io_service.hpp>

namespace mmcs {
namespace server {
namespace env {

class Coolant : public Polling
{
public:
    Coolant(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server
            );

private:
    cxxdb::ConnectionPtr prepareInsert(
            cxxdb::UpdateStatementPtr& coolantInsert
            );

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

    std::string getDescription() const { return "coolant monitor"; }
};

} } } // namespace mmcs::server::env

#endif
