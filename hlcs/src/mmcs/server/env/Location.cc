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

#include "Location.h"

#include "BulkPower.h"
#include "IoDrawer.h"
#include "LocationList.h"
#include "McServerConnection.h"
#include "NodeBoard.h"
#include "ServiceCard.h"

#include <db/include/api/tableapi/gensrc/DBTLinkchipenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTNodecardenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTNodeenvironment.h>

#include <utility/include/Log.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Location::Location(
        boost::asio::io_service& io_service,
        const std::string& type,
        const std::string& location,
        const unsigned seconds
        ) :
    Polling( io_service, seconds ),
    _location( location ),
    _type( type )
{
    // LOG_TRACE_MSG( type << " for " << location << " every " << seconds << " seconds" );
}

void
Location::stop()
{
    _enabled = false;

    boost::system::error_code error;
    _timer.cancel( error );
    if ( error ) {
        LOG_WARN_MSG(_location << " could not stop: " << boost::system::system_error(error).what());
    }
}

void
Location::done()
{
    LocationList::instance().remove( _location );
}

void
Location::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    MCServerMessageSpec::MakeTargetSetRequest request(_location, "EnvMonLoc", true);
    request._expression.push_back(_location);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Location::makeTargetSetHandler,
                boost::static_pointer_cast<Location>( shared_from_this() ),
                _1,
                mc_server
                )
            );
}

void
Location::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server
        )
{
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    const MCServerMessageSpec::OpenTargetRequest request(_location, "EnvMonLoc", MCServerMessageSpec::RAAW, true);
    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Location::openTargetHandler,
                boost::static_pointer_cast<Location>( shared_from_this() ),
                _1,
                mc_server
                )
            );
}

void
Location::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server
        )
{
    MCServerMessageSpec::OpenTargetReply reply;
    reply.read( response );

    if (reply._rc) {
        LOG_ERROR_MSG("Unable to open target set: " << reply._rt);
        this->wait();
        return;
    }
    LOG_TRACE_MSG("Opened target " << _location << " with handle " << reply._handle);

    if (_type == "service") {
        MCServerMessageSpec::ReadServiceCardEnvRequest request;
        request._set = _location;

        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Location::readHandler,
                    boost::static_pointer_cast<Location>( shared_from_this() ),
                    _1,
                    reply._handle,
                    mc_server
                    )
                );
    } else if (_type == "node") {
        MCServerMessageSpec::ReadNodeCardEnvRequest request;
        request._set = _location;
        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Location::readHandler,
                    boost::static_pointer_cast<Location>( shared_from_this() ),
                    _1,
                    reply._handle,
                    mc_server
                    )
                );
    } else if (_type == "io") {
        MCServerMessageSpec::ReadIoCardEnvRequest request;
        request._set = _location;
        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Location::readHandler,
                    boost::static_pointer_cast<Location>( shared_from_this() ),
                    _1,
                    reply._handle,
                    mc_server
                    )
                );
    } else if (_type == "bulk") {
        MCServerMessageSpec::ReadBulkPowerEnvRequest request;
        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Location::readHandler,
                    boost::static_pointer_cast<Location>( shared_from_this() ),
                    _1,
                    reply._handle,
                    mc_server
                    )
                );
    }
}

void
Location::readHandler(
        std::istream& response,
        const int handle,
        const McServerConnection::Ptr& mc_server
        )
{
    if (_type == "service") {
        MCServerMessageSpec::ReadServiceCardEnvReply reply;
        reply.read( response );

        cxxdb::ConnectionPtr connection;
        cxxdb::UpdateStatementPtr insert;
        connection = ServiceCard::prepareInsert( insert );
        cxxdb::Transaction tx( *connection );
        processSC(&reply, insert);
        connection->commit();
    } else if (_type == "node") {
        MCServerMessageSpec::ReadNodeCardEnvReply reply;
        reply.read( response );

        cxxdb::ConnectionPtr connection;
        cxxdb::UpdateStatementPtr nodeBoardInsert;
        cxxdb::UpdateStatementPtr computeInsert;
        cxxdb::UpdateStatementPtr linkChipInsert;
        connection = NodeBoard::prepareInserts(
                nodeBoardInsert,
                computeInsert,
                linkChipInsert
                );

        cxxdb::Transaction tx( *connection );
        processNB(&reply, nodeBoardInsert, computeInsert, linkChipInsert);
        connection->commit();
    } else if (_type == "io") {
        MCServerMessageSpec::ReadIoCardEnvReply reply;
        reply.read( response );

        cxxdb::ConnectionPtr connection;
        cxxdb::UpdateStatementPtr fanInsert;
        cxxdb::UpdateStatementPtr ioCardInsert;
        cxxdb::UpdateStatementPtr computeInsert;
        cxxdb::UpdateStatementPtr linkChipInsert;
        connection = IoDrawer::prepareInserts(
                fanInsert,
                ioCardInsert,
                computeInsert,
                linkChipInsert
                );

        cxxdb::Transaction tx( *connection );
        processIO(&reply, fanInsert, ioCardInsert, computeInsert, linkChipInsert);
        connection->commit();
    } else if (_type == "bulk") {
        MCServerMessageSpec::ReadBulkPowerEnvReply reply;
        reply.read( response );

        cxxdb::ConnectionPtr connection;
        cxxdb::UpdateStatementPtr insert;
        connection = BulkPower::prepareInserts(
                insert
                );

        processBulks(&reply, connection, insert);
    }

    this->closeTarget(
            mc_server,
            _location,
            handle,
            boost::bind(
                &Polling::wait,
                boost::static_pointer_cast<Location>( shared_from_this() )
                )
            );
}

} } } // namespace mmcs::server::env
