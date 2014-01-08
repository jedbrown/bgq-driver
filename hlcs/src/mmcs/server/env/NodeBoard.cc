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

#include "NodeBoard.h"

#include "McServerConnection.h"
#include "Token.h"
#include "types.h"
#include "utility.h"

#include "common/Properties.h"

#include <db/include/api/tableapi/gensrc/DBTLinkchipenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTNodeenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTNodecardenvironment.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/make_shared.hpp>

#include <algorithm>
#include <string>

using mmcs::common::Properties;

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

void
processNB(
        const MCServerMessageSpec::ReadNodeCardEnvReply* mcNCReply,
        const cxxdb::UpdateStatementPtr& nodeCardInsert,
        const cxxdb::UpdateStatementPtr& nodeInsert,
        const cxxdb::UpdateStatementPtr& linkChipInsert
        )
{
    unsigned MAXNODETEMP = 90; // Celcius
    if (!Properties::getProperty("MAXNODETEMP").empty() ) {
        MAXNODETEMP = boost::lexical_cast<int>(Properties::getProperty("MAXNODETEMP"));
    }

    for (
            std::vector<MCServerMessageSpec::NodeCardEnv>::const_iterator nodecard = mcNCReply->_nodeCards.begin();
            nodecard != mcNCReply->_nodeCards.end();
            ++nodecard
        )
    {
        if (nodecard->_error == CARD_NOT_PRESENT) {
            continue;
        }

        if (nodecard->_error == CARD_NOT_UP) {
            continue;
        }

        if (nodecard->_error) {
            LOG_ERROR_MSG("Error reading environmentals from: " << nodecard->_lctn);
            RasEventImpl noContact(0x00061001);
            noContact.setDetail(RasEvent::LOCATION, nodecard->_lctn);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
            continue;
        }

        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::LOCATION_COL ].set( nodecard->_lctn );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV08_COL ].cast( nodecard->_powerRailV08Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV14_COL ].cast( nodecard->_powerRailV14Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV25_COL ].cast( nodecard->_powerRailV25Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV33_COL ].cast( nodecard->_powerRailV33Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV120P_COL ].cast( nodecard->_powerRailV120PVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV15_COL ].cast( nodecard->_powerRailV15Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV09_COL ].cast( nodecard->_powerRailV09Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV10_COL ].cast( nodecard->_powerRailV10Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV33P_COL ].cast( nodecard->_powerRailV33PVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV12A_COL ].cast( nodecard->_powerRailV12AVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV12B_COL ].cast( nodecard->_powerRailV12BVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV18_COL ].cast( nodecard->_powerRailV18Voltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV25P_COL ].cast( nodecard->_powerRailV25PVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV12P_COL ].cast( nodecard->_powerRailV12PVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::VOLTAGEV18P_COL ].cast( nodecard->_powerRailV18PVoltage );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::TEMPMONITOR0_COL ].cast( nodecard->_onboardTemp0 );
        nodeCardInsert->parameters()[ BGQDB::DBTNodecardenvironment::TEMPMONITOR1_COL ].cast( nodecard->_onboardTemp1 );

        nodeCardInsert->execute();

        for (
                std::vector<MCServerMessageSpec::ComputeEnv>::const_iterator compute = nodecard->_computes.begin();
                compute != nodecard->_computes.end();
                ++compute
            )
        {
            if (compute->_error) {
                LOG_ERROR_MSG("Error reading environmentals from: " << compute->_lctn);
                continue;
            }

            nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::LOCATION_COL ].set( compute->_lctn );

            // use TVSense temp if its valid, use I2C temp otherwise
            if ( compute->_tempTvSense == 0 || compute->_tempTvSense >= 254 ) {
                nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::ASICTEMP_COL ].cast( compute->_tempI2c );
                if ( compute->_tempI2c > MAXNODETEMP ) {
                    RasEventImpl ras(0x00061015);
                    ras.setDetail(RasEvent::LOCATION, compute->_lctn);
                    ras.setDetail("ACTUAL", boost::lexical_cast<std::string>(compute->_tempI2c));
                    ras.setDetail("EXP", boost::lexical_cast<std::string>(MAXNODETEMP));
                    RasEventHandlerChain::handle(ras);
                    BGQDB::putRAS(ras);
                }
            } else {
                nodeInsert->parameters()[ BGQDB::DBTNodeenvironment::ASICTEMP_COL ].cast( compute->_tempTvSense );
                if ( compute->_tempTvSense > MAXNODETEMP ) {
                    RasEventImpl ras(0x00061015);
                    ras.setDetail(RasEvent::LOCATION, compute->_lctn);
                    ras.setDetail("ACTUAL", boost::lexical_cast<std::string>(compute->_tempTvSense));
                    ras.setDetail("EXP", boost::lexical_cast<std::string>(MAXNODETEMP));
                    RasEventHandlerChain::handle(ras);
                    BGQDB::putRAS(ras);
                }
            }
            nodeInsert->execute();
        }

        for (
                std::vector<MCServerMessageSpec::LinkChipEnv>::const_iterator link = nodecard->_blinks.begin();
                link != nodecard->_blinks.end();
                ++link
            )
        {
            if (link->_error) {
                LOG_ERROR_MSG("Error reading environmentals from: " << link->_lctn);
                continue;
            }
            linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::LOCATION_COL ].set( link->_lctn );

            // use TVSense temp if its valid, use I2C temp otherwise
            if (link->_tempTvSense == 0 || link->_tempTvSense >= 253)
                linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::ASICTEMP_COL ].cast( link->_tempI2c );
            else
                linkChipInsert->parameters()[ BGQDB::DBTLinkchipenvironment::ASICTEMP_COL ].cast( link->_tempTvSense );

            linkChipInsert->execute();
        }
    }
}

NodeBoard::NodeBoard(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 900 ),
    _racks( ),
    _strand( io_service ),
    _databaseStrand( io_service ),
    _connection( ),
    _nodeBoardInsert( ),
    _nodeInsert( ),
    _linkChipInsert( ),
    _mc_start( ),
    _insertion_time( )
{

}

cxxdb::ConnectionPtr
NodeBoard::prepareInserts(
        cxxdb::UpdateStatementPtr& nodeBoardInsert,
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
    columns.reset( BGQDB::DBTNodecardenvironment::TIME );
    BGQDB::DBTNodecardenvironment nce( columns );
    nodeBoardInsert = result->prepareUpdate(
            nce.getInsertStatement(),
            nce.calcColumnNames()
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
NodeBoard::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    const Timer::Ptr database_timer = this->time()->subFunction("connect to database");
    database_timer->dismiss();

    _connection = this->prepareInserts( _nodeBoardInsert, _nodeInsert, _linkChipInsert );

    if ( !_connection ) {
        LOG_ERROR_MSG("Could not get database connection." );
        this->wait();
        return;
    }
    database_timer->dismiss( false );
    database_timer->stop();

    // We'll pass this token around to the completion handlers for collecting and
    // inserting all the environmental values. When it goes out of scope, it means we are done.
    const Token::Ptr token(
            Token::create(
                this->getDescription(),
                boost::bind(
                    &NodeBoard::createTimers,
                    boost::static_pointer_cast<NodeBoard>( shared_from_this() )
                    )
                )
            );

    const Timer::Ptr rack_timer = this->time()->subFunction("count compute racks");

    _racks = getRacks(_connection);
    const unsigned connections = calculateConnectionSize( _racks );

    rack_timer->stop();

    LOG_DEBUG_MSG("Creating " << connections << " connections to mc_server.");

    _insertion_time = boost::posix_time::time_duration();
    _mc_start = boost::posix_time::microsec_clock::local_time();

    for ( unsigned i = 0; i < connections; ++i ) {
        const McServerConnection::Ptr mc(
                McServerConnection::create(
                    _io_service
                    )
                );
        mc->start(
                this->getDescription(),
                _strand.wrap(
                    boost::bind(
                        &NodeBoard::connectHandler,
                        boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                        _1,
                        _2,
                        mc,
                        token
                        )
                    )
                );
    }

    _strand.post(
            boost::bind(
                &NodeBoard::makeTargetSet,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                mc_server,
                token
                )
            );
}

void
NodeBoard::connectHandler(
        const bgq::utility::Connector::Error::Type error,
        const std::string& message,
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    if ( error ) {
        LOG_ERROR_MSG("Could not connect: " << message );
        return;
    }

    this->makeTargetSet( mc_server, token );
}

void
NodeBoard::makeTargetSet(
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    // assume this handler is protected by _strand
    LOG_DEBUG_MSG( _racks.size() << " racks remaining" );
    if ( _racks.empty() ) return;

    // use rack location as part of name, ex: EnvMonNCxx where xx is rack row,column
    const std::string name( "EnvMonNC" + _racks.begin()->substr(1,2) );
    MCServerMessageSpec::MakeTargetSetRequest request(name, "EnvMonNC", true);
    request._expression.push_back( _racks.begin()->substr(1,2) + "-M.-N..$" );
    _racks.erase( _racks.begin() );

    LOG_TRACE_MSG( "Making target set " << request._set );
    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &NodeBoard::makeTargetSetHandler,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                _1,
                mc_server,
                name,
                token
                )
            );
}

void
NodeBoard::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const std::string& name,
        const Token::Ptr& token
        )
{
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    LOG_TRACE_MSG( "Made target set " << name );
    const MCServerMessageSpec::OpenTargetRequest request(name, "EnvMonNC", MCServerMessageSpec::RAAW, true);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &NodeBoard::openTargetHandler,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                _1,
                mc_server,
                name,
                token
                )
            );
}

void
NodeBoard::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const std::string& name,
        const Token::Ptr& token
        )
{
    MCServerMessageSpec::OpenTargetReply reply;
    reply.read( response );

    if (reply._rc) {
        LOG_ERROR_MSG("Unable to open target set: " << reply._rt);
        _strand.post(
                boost::bind(
                    &NodeBoard::makeTargetSet,
                    boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                    mc_server,
                    token
                    )
                );

        return;
    }
    LOG_TRACE_MSG("Opened " << name << " target set with handle " << reply._handle );

    MCServerMessageSpec::ReadNodeCardEnvRequest request;
    request._set = name;
    LOG_DEBUG_MSG( request.getClassName() << " begin " << name );

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &NodeBoard::readHandler,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                _1,
                name,
                reply._handle,
                mc_server,
                token
                )
            );
}

void
NodeBoard::readHandler(
        std::istream& response,
        const std::string& name,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ << " end " << name );

    const boost::shared_ptr<MCServerMessageSpec::ReadNodeCardEnvReply> reply(
            boost::make_shared<MCServerMessageSpec::ReadNodeCardEnvReply>()
            );
    reply->read( response );

    this->closeTarget(
            mc_server,
            name,
            handle,
            boost::bind(
                &NodeBoard::closeTargetHandler,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                reply,
                name,
                mc_server,
                token
                )
            );
}

void
NodeBoard::closeTargetHandler(
        const boost::shared_ptr<MCServerMessageSpec::ReadNodeCardEnvReply>& reply,
        const std::string& name,
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ << " for " << name );

    _databaseStrand.post(
            boost::bind(
                &NodeBoard::insertData,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                reply,
                name,
                mc_server,
                token
                )
            );

    _strand.post(
            boost::bind(
                &NodeBoard::makeTargetSet,
                boost::static_pointer_cast<NodeBoard>( shared_from_this() ),
                mc_server,
                token
                )
            );
}

void
NodeBoard::insertData(
        const boost::shared_ptr<MCServerMessageSpec::ReadNodeCardEnvReply>& reply,
        const std::string& name,
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    // assume this handler is protected by the database strand
    const boost::posix_time::ptime start( boost::posix_time::microsec_clock::local_time() );
    LOG_DEBUG_MSG( __FUNCTION__ << " begin " << name );

    cxxdb::Transaction tx( *_connection );
    processNB(reply.get(), _nodeBoardInsert, _nodeInsert, _linkChipInsert);
    _connection->commit();
    _insertion_time += boost::posix_time::microsec_clock::local_time() - start;

    LOG_DEBUG_MSG( __FUNCTION__ << " end " << name );
}

void
NodeBoard::createTimers()
{
    // Need to subtract the insertion time from the mc time
    const boost::posix_time::time_duration mc_time(
            boost::posix_time::microsec_clock::local_time() - _mc_start - _insertion_time
            );

    // Need to create data points here, we can't use timers since the durations can overlap
    const std::string qualifier(
            boost::lexical_cast<std::string>(
                boost::posix_time::time_duration(
                    _start - boost::posix_time::ptime(boost::gregorian::date(1970,1,1))
                    ).total_seconds()
                )
            );

    bgq::utility::performance::DataPoint mc( this->getDescription(), "EnvMon", mc_time );
    mc.setMode( bgq::utility::performance::Mode::Value::Basic );
    mc.setQualifier( qualifier );
    mc.setSubFunction( "mc" );
    _counters->add( mc );

    bgq::utility::performance::DataPoint database( this->getDescription(), "EnvMon", _insertion_time );
    database.setMode( bgq::utility::performance::Mode::Value::Basic );
    database.setQualifier( qualifier );
    database.setSubFunction( "database insertion" );
    _counters->add( database );

    _linkChipInsert.reset();
    _nodeInsert.reset();
    _nodeBoardInsert.reset();
    _connection.reset();

    this->wait();
}

} } } // namespace mmcs::server::env
