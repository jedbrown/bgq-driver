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

#ifndef MMCS_ENV_NODE_BOARD_H
#define MMCS_ENV_NODE_BOARD_H

#include "Polling.h"
#include "Token.h"
#include "types.h"

#include <db/include/api/cxxdb/fwd.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/strand.hpp>

namespace MCServerMessageSpec {

class ReadNodeCardEnvReply;

}

namespace mmcs {
namespace server {
namespace env {

void
processNB(
        const MCServerMessageSpec::ReadNodeCardEnvReply* mcNCReply,
        const cxxdb::UpdateStatementPtr& nodeBoardInsert,
        const cxxdb::UpdateStatementPtr& nodeInsert,
        const cxxdb::UpdateStatementPtr& linkChipInsert
        );

/*!
 * \brief Handle node board environmentals
 *
 * This class will create one connection to mc_server for each
 * row or column in the system, whichever is greater. For example:
 *
 * - a 15x8 (120 rack) system will use 15 connections
 *
 * - a 3x16 (48 rack) system will use 16 connections
 *
 * This is done to improve environmental data collection performance. A Token
 * object manages the lifetime of each polling instance.
 */
class NodeBoard : public Polling
{
public:
    /*!
     * \brief ctor.
     */
    NodeBoard(
            boost::asio::io_service& io_service //!< [in]
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server  //!< [in]
            );

    static cxxdb::ConnectionPtr prepareInserts(
            cxxdb::UpdateStatementPtr& nodeBoardInsert,
            cxxdb::UpdateStatementPtr& nodeInsert,
            cxxdb::UpdateStatementPtr& linkChipInsert
            );

private:
    std::string getDescription() const { return "node card"; }

    void connectHandler(
            const bgq::utility::Connector::Error::Type error,
            const std::string& message,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Token::Ptr& token
            );

    void makeTargetSet(
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Token::Ptr& token
            );

    void makeTargetSetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const std::string& name,
            const Token::Ptr& token
        );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const std::string& name,
            const Token::Ptr& token
            );

    void readHandler(
            std::istream& response,
            const std::string& name,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Token::Ptr& token
            );

    void closeTargetHandler(
            const boost::shared_ptr<MCServerMessageSpec::ReadNodeCardEnvReply>& reply,
            const std::string& name,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Token::Ptr& token
            );

    void insertData(
            const boost::shared_ptr<MCServerMessageSpec::ReadNodeCardEnvReply>& reply,
            const std::string& name,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const Token::Ptr& token
        );

    void createTimers();

private:
    Racks _racks;                                       //!< compute rack location strings
    boost::asio::io_service::strand _strand;            //!< serialized access to _racks
    boost::asio::io_service::strand _databaseStrand;    //!< serialized access to database connection
    cxxdb::ConnectionPtr _connection;
    cxxdb::UpdateStatementPtr _nodeBoardInsert;
    cxxdb::UpdateStatementPtr _nodeInsert;
    cxxdb::UpdateStatementPtr _linkChipInsert;
    boost::posix_time::ptime _mc_start;
    boost::posix_time::time_duration _insertion_time;
};

} } } // namespace mmcs::server::env

#endif
