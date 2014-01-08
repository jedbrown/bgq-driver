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

#ifndef MMCS_ENV_PERF_DATA_H
#define MMCS_ENV_PERF_DATA_H

#include "Polling.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/asio/io_service.hpp>


namespace mmcs {
namespace server {
namespace env {


class PerfData : public Polling
{
public:
    PerfData(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server  //!< [in]
            );

private:
    std::string getDescription() const { return "performance data"; }

    void readHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const cxxdb::ConnectionPtr& connection,
            const cxxdb::UpdateStatementPtr& insert
            );

private:
    cxxdb::ConnectionPtr prepareInserts(
            cxxdb::UpdateStatementPtr& perfInsert
            );
};

} } } // namespace mmcs::server::env

#endif
