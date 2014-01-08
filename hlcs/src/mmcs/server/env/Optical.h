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

#ifndef MMCS_ENV_OPTICAL_H
#define MMCS_ENV_OPTICAL_H

#include "Polling.h"
#include "Token.h"

#include <db/include/api/cxxdb/fwd.h>
#include <xml/include/library/XML.h>

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/asio/io_service.hpp>

namespace mmcs {
namespace server {
namespace env {

/*!
 * \brief Handle optical module environmentals
 *
 * \copydetails mmcs::server::env::NodeBoard
 */
class Optical : public Polling
{
public:
    Optical(
            boost::asio::io_service& io_service
            );

    void impl(
            const boost::shared_ptr<McServerConnection>& mc_server  //!< [in]
            );

    static cxxdb::ConnectionPtr prepareInserts(
            cxxdb::UpdateStatementPtr& opticalInsert,
            cxxdb::UpdateStatementPtr& opticalDataInsert,
            cxxdb::UpdateStatementPtr& opticalChannelDataInsert
            );

private:
    std::string getDescription() const { return "optical module"; }

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
            bool io,
            const Token::Ptr& token
        );

    void openTargetHandler(
            std::istream& response,
            const boost::shared_ptr<McServerConnection>& mc_server,
            const std::string& name,
            bool io,
            const Token::Ptr& token
            );

    void readHandler(
            std::istream& response,
            const std::string& name,
            const int handle,
            const boost::shared_ptr<McServerConnection>& mc_server,
            bool io,
            const Token::Ptr& token
            );

    void closeTargetHandler(
            const boost::shared_ptr<XML::Serializable>& reply,
            const std::string& name,
            const boost::shared_ptr<McServerConnection>& mc_server,
            bool io,
            const Token::Ptr& token
            );

    void processNodeCard(
            const MCServerMessageSpec::ReadNodeCardEnvReply& reply
            );

    void processIo(
            const MCServerMessageSpec::ReadIoCardEnvReply& reply
            );

    void insertData(
            const boost::shared_ptr<XML::Serializable>& reply,
            const std::string& name,
            const boost::shared_ptr<McServerConnection>& mc_server,
            bool io,
            const Token::Ptr& token
            );

    void createTimers();

private:
    Racks _racks;
    IoDrawers _drawers;
    boost::asio::io_service::strand _strand;            //!< serialized access to _racks, and _drawers
    boost::asio::io_service::strand _databaseStrand;    //!< serialized access to database connection
    cxxdb::ConnectionPtr _connection;
    cxxdb::UpdateStatementPtr _opticalInsert;
    cxxdb::UpdateStatementPtr _opticalDataInsert;
    cxxdb::UpdateStatementPtr _opticalChannelDataInsert;
    boost::posix_time::ptime _mc_start;
    boost::posix_time::time_duration _insertion_time;
};

} } } // namespace mmcs::server::env

#endif
