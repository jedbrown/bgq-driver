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

#include "IoDrawer.h"

#include "McServerConnection.h"
#include "types.h"
#include "utility.h"

#include <db/include/api/tableapi/gensrc/DBTFanenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTIocardenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTLinkchipenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTNodeenvironment.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/make_shared.hpp>
#include <boost/scoped_ptr.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

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
        )
{
    for (
            std::vector<MCServerMessageSpec::IoCardEnv>::const_iterator io = mcIOReply->_ioCards.begin();
            io != mcIOReply->_ioCards.end();
            ++io
    )
    {
        if (io->_error == CARD_NOT_PRESENT) continue;
        if (io->_error == CARD_NOT_UP) continue;
        if (io->_error) {
            LOG_ERROR_MSG("Error reading environmentals from: " << io->_lctn);
            RasEventImpl noContact(0x00061004);
            noContact.setDetail(RasEvent::LOCATION, io->_lctn);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
            continue;
        }

        // LOG_TRACE_MSG("Processing " << io->_lctn );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::LOCATION_COL ].set( io->_lctn );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV08_COL ].cast( io->_powerRailV08Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV14_COL ].cast( io->_powerRailV14Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV25_COL ].cast( io->_powerRailV25Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV33_COL ].cast( io->_powerRailV33Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV120_COL ].cast( io->_powerRailV120Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV15_COL ].cast( io->_powerRailV15Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV15_COL ].cast( io->_powerRailV15Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV09_COL ].cast( io->_powerRailV09Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV10_COL ].cast( io->_powerRailV10Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV120P_COL ].cast( io->_powerRailV120PVoltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV33P_COL ].cast( io->_powerRailV33PVoltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV12_COL ].cast( io->_powerRailV12Voltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV18_COL ].cast( io->_powerRailV18Voltage );

        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV12P_COL ].cast( io->_powerRailV12PVoltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV15P_COL ].cast( io->_powerRailV15PVoltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV18P_COL ].cast( io->_powerRailV18PVoltage );
        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::VOLTAGEV25P_COL ].cast( io->_powerRailV25PVoltage );

        ioCardInsert->parameters()[ BGQDB::DBTIocardenvironment::TEMPMONITOR_COL ].cast( (io->_onboardTemp) );

        ioCardInsert->execute();

        fanInsert->parameters()[ BGQDB::DBTFanenvironment::LOCATION_COL ].set( io->_lctn );
        for (
                std::vector<MCServerMessageSpec::RpmsFan>::const_iterator fan = io->_fanRpms.begin();
                fan != io->_fanRpms.end();
                ++fan
            )
        {
            fanInsert->parameters()[ BGQDB::DBTFanenvironment::FANNUMBER_COL ].cast( std::distance(io->_fanRpms.begin(), fan) );
            fanInsert->parameters()[ BGQDB::DBTFanenvironment::RPMS_COL ].cast( fan->_rpmsFans );
            fanInsert->execute();
        }

        for (
                std::vector<MCServerMessageSpec::ComputeEnv>::const_iterator compute = io->_computes.begin();
                compute != io->_computes.end();
                ++compute
            )
        {
            if ( compute->_error ) {
                LOG_ERROR_MSG("Error reading environmentals from: " << compute->_lctn);
            } else {
                nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::LOCATION_COL ].set( compute->_lctn );

                // use TVSense temp if its valid, use I2C temp otherwise
                if (compute->_tempTvSense == 0 || compute->_tempTvSense >= 254)
                    nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::ASICTEMP_COL ].cast(compute->_tempI2c);
                else
                    nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::ASICTEMP_COL ].cast(compute->_tempTvSense);

                nodeInsert->execute();
            }
        }

        for (
                std::vector<MCServerMessageSpec::LinkChipEnv>::const_iterator link = io->_blinks.begin();
                link != io->_blinks.end();
                ++link
            )
        {
            if ( link->_error ) {
                LOG_ERROR_MSG("Error reading environmentals from: " << link->_lctn);
            } else {
                linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::LOCATION_COL ].set( link->_lctn );

                // use TVSense temp if its valid, use I2C temp otherwise
                if (link->_tempTvSense == 0 || link->_tempTvSense >= 253)
                    linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::ASICTEMP_COL ].cast( link->_tempI2c );
                else
                    linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::ASICTEMP_COL ].cast( link->_tempTvSense );

                linkChipInsert->execute();
            }
        }
    }  // for loop that goes thru all IO cards
}

IoDrawer::IoDrawer(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 300 ),
    _connection( ),
    _fanInsert( ),
    _ioCardInsert( ),
    _nodeInsert( ),
    _linkChipInsert( )
{

}

cxxdb::ConnectionPtr
IoDrawer::prepareInserts(
        cxxdb::UpdateStatementPtr& fanInsert,
        cxxdb::UpdateStatementPtr& ioCardInsert,
        cxxdb::UpdateStatementPtr& nodeInsert,
        cxxdb::UpdateStatementPtr& linkChipInsert
        )
{
    const cxxdb::ConnectionPtr result = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !result ) {
        return result;
    }

    // prepare insert statements
    BGQDB::ColumnsBitmap columns;
    columns.set();
    columns.reset( BGQDB::DBTFanenvironment::TIME );
    BGQDB::DBTFanenvironment fe( columns );
    fanInsert = result->prepareUpdate(
            fe.getInsertStatement(),
            fe.calcColumnNames()
            );

    columns.set();
    columns.reset( BGQDB::DBTIocardenvironment::TIME );
    BGQDB::DBTIocardenvironment ioe( columns );
    ioCardInsert = result->prepareUpdate(
            ioe.getInsertStatement(),
            ioe.calcColumnNames()
            );

    columns.set();
    columns.reset( BGQDB::DBTNodeenvironment::TIME );
    BGQDB::DBTNodeenvironment ne( columns );
    nodeInsert = result->prepareUpdate(
            ne.getInsertStatement(),
            ne.calcColumnNames()
            );

    columns.set();
    columns.reset( BGQDB::DBTLinkchipenvironment::TIME );
    BGQDB::DBTLinkchipenvironment lce( columns );
    linkChipInsert = result->prepareUpdate(
            lce.getInsertStatement(),
            lce.calcColumnNames()
            );

    return result;
}

void
IoDrawer::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    const Timer::Ptr database_timer = this->time()->subFunction("connect to database");
    database_timer->dismiss();

    _connection = this->prepareInserts( _fanInsert, _ioCardInsert, _nodeInsert, _linkChipInsert );

    if ( !_connection ) {
        LOG_ERROR_MSG("Could not get database connection." );
        this->wait();
        return;
    }

    database_timer->dismiss( false );
    database_timer->stop();

    const Timer::Ptr target_set_timer = this->time()->subFunction("make and open target set" );
    target_set_timer->dismiss();

    MCServerMessageSpec::MakeTargetSetRequest request("EnvMonIO", "EnvMonIO", true);
    const IoDrawers ioDrawers( getIoDrawers(_connection) );
    BOOST_FOREACH( const std::string& i, ioDrawers ) {
        request._expression.push_back(i);
    }

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &IoDrawer::makeTargetSetHandler,
                boost::static_pointer_cast<IoDrawer>( shared_from_this() ),
                _1,
                mc_server,
                target_set_timer
                )
            );
}

void
IoDrawer::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const Timer::Ptr& timer
        )
{
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    LOG_TRACE_MSG( "Made target set EnvMonIO" );
    const MCServerMessageSpec::OpenTargetRequest request("EnvMonIO", "EnvMonIO", MCServerMessageSpec::RAAW, true);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &IoDrawer::openTargetHandler,
                boost::static_pointer_cast<IoDrawer>( shared_from_this() ),
                _1,
                mc_server,
                timer
                )
            );
}

void
IoDrawer::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const Timer::Ptr& timer
        )
{
    MCServerMessageSpec::OpenTargetReply reply;
    reply.read( response );

    if (reply._rc) {
        LOG_ERROR_MSG("Unable to open target set: " << reply._rt);
        this->wait();
        return;
    }
    LOG_TRACE_MSG("Opened target set with handle " << reply._handle );

    timer->dismiss( false );
    timer->stop();

    const Timer::Ptr mc_timer = this->time()->subFunction("mc");

    MCServerMessageSpec::ReadIoCardEnvRequest request;
    request._set = "EnvMonIO";

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &IoDrawer::readHandler,
                boost::static_pointer_cast<IoDrawer>( shared_from_this() ),
                _1,
                reply._handle,
                mc_server,
                mc_timer
                )
            );
}

void
IoDrawer::readHandler(
        std::istream& response,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        const Timer::Ptr& timer
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ );
    const boost::shared_ptr<MCServerMessageSpec::ReadIoCardEnvReply> reply(
            boost::make_shared<MCServerMessageSpec::ReadIoCardEnvReply>()
            );
    reply->read( response );
    
    timer->stop();

    this->closeTarget(
            mc_server,
            "EnvMonIO",
            handle,
            boost::bind(
                &IoDrawer::closeTargetHandler,
                boost::static_pointer_cast<IoDrawer>( shared_from_this() ),
                reply
                )
            );
}

void
IoDrawer::closeTargetHandler(
        const boost::shared_ptr<MCServerMessageSpec::ReadIoCardEnvReply>& reply
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ );
    const Timer::Ptr database_timer = this->time()->subFunction("database insertion");
    database_timer->dismiss();

    {
        try {
            cxxdb::Transaction tx( *_connection );
            processIO(reply.get(), _fanInsert, _ioCardInsert, _nodeInsert, _linkChipInsert);
            _connection->commit();
            database_timer->dismiss( false );
        } catch ( const std::exception& e ) {
            LOG_WARN_MSG( e.what() );
        }
    }

    _fanInsert.reset();
    _ioCardInsert.reset();
    _nodeInsert.reset();
    _linkChipInsert.reset();
    _connection.reset();

    this->wait();
}

} } } // namespace mmcs::server::env

