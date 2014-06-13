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

#include "Optical.h"

#include "McServerConnection.h"
#include "Token.h"
#include "types.h"
#include "utility.h"

#include "common/Properties.h"

#include <db/include/api/tableapi/gensrc/DBTOpticalenvironment.h>
#include <db/include/api/tableapi/gensrc/DBTOpticalchanneldata.h>
#include <db/include/api/tableapi/gensrc/DBTOpticaldata.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

#include <boost/make_shared.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Optical::Optical(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 3600 ),
    _racks( ),
    _drawers( ),
    _strand( io_service ),
    _databaseStrand( io_service ),
    _connection( ),
    _opticalInsert( ),
    _opticalDataInsert( ),
    _opticalChannelDataInsert( ),
    _mc_start( ),
    _insertion_time( )
{

}

cxxdb::ConnectionPtr
Optical::prepareInserts(
        cxxdb::UpdateStatementPtr& opticalInsert,
        cxxdb::UpdateStatementPtr& opticalDataInsert,
        cxxdb::UpdateStatementPtr& opticalChannelDataInsert
        )
{
    const cxxdb::ConnectionPtr result( BGQDB::DBConnectionPool::Instance().getConnection() );
    if ( !result ) {
        return result;
    }

    // prepare insert statements
    BGQDB::ColumnsBitmap columns;
    columns.set();
    columns.reset( BGQDB::DBTOpticalenvironment::TIME );
    BGQDB::DBTOpticalenvironment opte( columns );
    opticalInsert = result->prepareUpdate(
            opte.getInsertStatement(),
            opte.calcColumnNames()
            );

    columns.set();
    columns.reset( BGQDB::DBTOpticaldata::TIME );
    BGQDB::DBTOpticaldata optdata( columns );
    opticalDataInsert = result->prepareUpdate(
            optdata.getInsertStatement(),
            optdata.calcColumnNames()
            );

    columns.set();
    columns.reset( BGQDB::DBTOpticalchanneldata::TIME );
    BGQDB::DBTOpticaldata optchdata( columns );
    opticalChannelDataInsert = result->prepareUpdate(
            optchdata.getInsertStatement(),
            optchdata.calcColumnNames()
            );

    return result;
}

void
Optical::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    const Timer::Ptr database_timer = this->time()->subFunction("connect to database");
    database_timer->dismiss();

    _connection = this->prepareInserts( _opticalInsert, _opticalDataInsert, _opticalChannelDataInsert );

    if ( !_connection ) {
        LOG_ERROR_MSG("Could not get database connection.");
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
                    &Optical::createTimers,
                    boost::static_pointer_cast<Optical>( shared_from_this() )
                    )
                )
            );

    const Timer::Ptr rack_timer = this->time()->subFunction("count racks");

    _racks = getRacks(_connection);
    _drawers = getIoDrawers(_connection);
    const unsigned connections = calculateConnectionSize( _racks );

    rack_timer->stop();

    LOG_DEBUG_MSG("Creating " << connections << " connections to mc_server." );

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
                        &Optical::connectHandler,
                        boost::static_pointer_cast<Optical>( shared_from_this() ),
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
                &Optical::makeTargetSet,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                mc_server,
                token
                )
            );
}

void
Optical::connectHandler(
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
Optical::makeTargetSet(
        const McServerConnection::Ptr& mc_server,
        const Token::Ptr& token
        )
{
    // assume this handler is protected by _strand
    LOG_DEBUG_MSG( _racks.size() << " racks remaining and " << _drawers.size() << " drawers remaining" );
    if ( _racks.empty() && _drawers.empty() ) {
        return;
    }

    MCServerMessageSpec::MakeTargetSetRequest request;
    request._user = "EnvMonOM";
    request._temporary = true;

    bool io = false;

    if ( !_racks.empty() ) {
        // name is the name plus the rack location, ex: EnvMonOMxx where xx is rack row,column
        request._set = "EnvMonOM" + _racks.begin()->substr(1, 2);
        request._expression.push_back( _racks.begin()->substr(1,2) + "-M.-N..$" );
        _racks.erase( _racks.begin() );
    } else {
        request._set = "EnvMonOM";
        BOOST_FOREACH( const std::string& i, _drawers ) {
            request._expression.push_back(i);
        }
        _drawers.clear();
        io = true;
    }

    LOG_TRACE_MSG( "Making target set " << request._set );
    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Optical::makeTargetSetHandler,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                _1,
                mc_server,
                request._set,
                io,
                token
                )
            );
}

void
Optical::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const std::string& name,
        bool io,
        const Token::Ptr& token
        )
{
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    LOG_TRACE_MSG( "Made target set " << name );
    const MCServerMessageSpec::OpenTargetRequest request(name, "EnvMonOM", MCServerMessageSpec::RAAW, true);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Optical::openTargetHandler,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                _1,
                mc_server,
                name,
                io,
                token
                )
            );
}

void
Optical::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const std::string& name,
        bool io,
        const Token::Ptr& token
        )
{
    MCServerMessageSpec::OpenTargetReply reply;
    reply.read( response );

    if (reply._rc) {
        LOG_ERROR_MSG("Unable to open target set: " << reply._rt);
        _strand.post(
                boost::bind(
                    &Optical::makeTargetSet,
                    boost::static_pointer_cast<Optical>( shared_from_this() ),
                    mc_server,
                    token
                    )
                );
        return;
    }
    LOG_TRACE_MSG("Opened " << name << " target set with handle " << reply._handle );

    if ( io ) {
        MCServerMessageSpec::ReadIoCardEnvRequest request;
        request._set = name;
        LOG_DEBUG_MSG( request.getClassName() << " begin " << name );

        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Optical::readHandler,
                    boost::static_pointer_cast<Optical>( shared_from_this() ),
                    _1,
                    name,
                    reply._handle,
                    mc_server,
                    io,
                    token
                    )
                );
    } else {
        MCServerMessageSpec::ReadNodeCardEnvRequest request;
        request._set = name;
        LOG_DEBUG_MSG( request.getClassName() << " begin " << name );

        mc_server->send(
                request.getClassName(),
                request,
                boost::bind(
                    &Optical::readHandler,
                    boost::static_pointer_cast<Optical>( shared_from_this() ),
                    _1,
                    name,
                    reply._handle,
                    mc_server,
                    io,
                    token
                    )
                );
    }
}

void
Optical::readHandler(
        std::istream& response,
        const std::string& name,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        bool io,
        const Token::Ptr& token
        )
{
    LOG_DEBUG_MSG( __FUNCTION__ << " end " << name );

    boost::shared_ptr<XML::Serializable> reply;
    if ( io ) {
        reply = boost::make_shared<MCServerMessageSpec::ReadIoCardEnvReply>();
        reply->read( response );
    } else {
        reply = boost::make_shared<MCServerMessageSpec::ReadNodeCardEnvReply>();
        reply->read( response );
    }
    this->closeTarget(
            mc_server,
            name,
            handle,
            boost::bind(
                &Optical::closeTargetHandler,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                reply,
                name,
                mc_server,
                io,
                token
                )
            );
}

void
Optical::closeTargetHandler(
            const boost::shared_ptr<XML::Serializable>& reply,
            const std::string& name,
            const boost::shared_ptr<McServerConnection>& mc_server,
            bool io,
            const Token::Ptr& token
            )
{
    _databaseStrand.post(
            boost::bind(
                &Optical::insertData,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                reply,
                name,
                mc_server,
                io,
                token
                )
            );

    _strand.post(
            boost::bind(
                &Optical::makeTargetSet,
                boost::static_pointer_cast<Optical>( shared_from_this() ),
                mc_server,
                token
                )
            );
}

void
Optical::processNodeCard(
        const MCServerMessageSpec::ReadNodeCardEnvReply& reply
        )
{
    const bool addOpticalData = !common::Properties::getProperty("OPTICALDATA").empty();

    for (
            std::vector<MCServerMessageSpec::NodeCardEnv>::const_iterator nodecard = reply._nodeCards.begin();
            nodecard != reply._nodeCards.end();
            ++nodecard
        )
    {
        if (nodecard->_error == CARD_NOT_PRESENT) continue;
        if (nodecard->_error == CARD_NOT_UP) continue;
        if (nodecard->_error) {
            LOG_ERROR_MSG("Error reading environmentals from: " << nodecard->_lctn);
            RasEventImpl noContact(0x00061001);
            noContact.setDetail(RasEvent::LOCATION, nodecard->_lctn);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
            continue;
        }

        // commit transaction once per node board
        cxxdb::Transaction tx( *_connection );

        for (
                std::vector<MCServerMessageSpec::OpticEnv>::const_iterator optic = nodecard->_optics.begin();
                optic != nodecard->_optics.end();
                ++optic
            )
        {
            if (optic->_error) {
                LOG_ERROR_MSG("Error reading environmentals from: " << optic->_lctn);
                continue;
            }

            _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::LOCATION_COL ].set( optic->_lctn );
            for (unsigned channel = 0; channel < optic->_OpticalEnvChannel.size() ; ++channel) {
                _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::CHANNEL_COL ].cast( channel );
                _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::POWER_COL ].cast(
                        optic->_OpticalEnvChannel[channel]._opticalPower
                        );
                _opticalInsert->execute();
            }

            if (addOpticalData) {
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::LOCATION_COL ].set(
                        optic->_lctn
                        );

                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ERROR_COL ].cast( optic->_error );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::STATUS_COL ].cast( optic->_status );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::LOS_COL ].cast( optic->_los );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::LOSMASK_COL ].cast( optic->_losMask );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::FAULTS_COL ].cast( optic->_faults );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::FAULTSMASK_COL ].cast( optic->_faultsMask );

                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSTEMP_COL ].cast( optic->_alarmsTemp );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSTEMPMASK_COL ].cast( optic->_alarmsTempMask );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSVOLTAGE_COL ].cast( optic->_alarmsVoltage );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSVOLTAGEMASK_COL ].cast( optic->_alarmsVoltageMask );

                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSBIASCURRENT_COL ].cast( optic->_alarmsBiasCurrent );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSBIASCURRENTMASK_COL ].cast( optic->_alarmsBiasCurrentMask );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSPOWER_COL ].cast( optic->_alarmsPower );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::ALARMSPOWERMASK_COL ].cast( optic->_alarmsPowerMask );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::TEMP_COL ].cast( optic->_temp );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::VOLTAGEV33_COL ].cast( optic->_voltageV33 );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::VOLTAGEV25_COL ].cast( optic->_voltageV25 );

                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::EOTHOURS_COL ].cast( optic->_eot );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::FIRMWARELEVEL_COL ].cast( optic->_firmwareLevel );
                _opticalDataInsert->parameters()[ BGQDB::DBTOpticaldata::SERIALNUMBER_COL ].set( optic->_serialNumber );

                _opticalDataInsert->execute();

                _opticalChannelDataInsert->parameters()[ BGQDB::DBTOpticalchanneldata::LOCATION_COL ].set( optic->_lctn );
                for (unsigned channel = 0; channel < optic->_OpticalEnvChannel.size(); ++channel) {
                    _opticalChannelDataInsert->parameters()[ BGQDB::DBTOpticalchanneldata::CHANNEL_COL ].cast( channel );
                    _opticalChannelDataInsert->parameters()[ BGQDB::DBTOpticalchanneldata::BIASCURRENT_COL ].cast( optic->_OpticalEnvChannel[channel]._biasCurrent );
                    _opticalChannelDataInsert->parameters()[ BGQDB::DBTOpticalchanneldata::POWER_COL ].cast( optic->_OpticalEnvChannel[channel]._opticalPower );
                    _opticalChannelDataInsert->execute();
                }
            }
        }

        _connection->commit();
    } // for loop that goes through each node card
}

void
Optical::processIo(
        const MCServerMessageSpec::ReadIoCardEnvReply& reply
        )
{
    for (
            std::vector<MCServerMessageSpec::IoCardEnv>::const_iterator io = reply._ioCards.begin();
            io != reply._ioCards.end();
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

        // commit transaction once per I/O drawer
        cxxdb::Transaction tx( *_connection );

        for (
                std::vector<MCServerMessageSpec::OpticEnv>::const_iterator optic = io->_optics.begin();
                optic != io->_optics.end();
                ++optic
            )
        {
            if (optic->_error) {
                LOG_ERROR_MSG("Error reading environmentals from: " << optic->_lctn);
                continue;
            }

            _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::LOCATION_COL ].set( optic->_lctn );
            for (unsigned channel = 0; channel < optic->_OpticalEnvChannel.size(); ++channel) {
                _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::CHANNEL_COL ].cast( channel );
                _opticalInsert->parameters()[ BGQDB::DBTOpticalenvironment::POWER_COL ].cast( optic->_OpticalEnvChannel[channel]._opticalPower );
                _opticalInsert->execute();
            }
        }

        _connection->commit();
    }
}

void
Optical::insertData(
        const boost::shared_ptr<XML::Serializable>& reply,
        const std::string& name,
        const McServerConnection::Ptr& mc_server,
        bool io,
        const Token::Ptr& token
        )
{
    // Assume this handler is protected by the database strand
    const boost::posix_time::ptime start( boost::posix_time::microsec_clock::local_time() );
    LOG_TRACE_MSG( __FUNCTION__ << " begin " << name );
    BOOST_ASSERT( reply );

    try {
        if ( io ) {
            this->processIo(
                    *boost::dynamic_pointer_cast<MCServerMessageSpec::ReadIoCardEnvReply>( reply )
                    );
        } else {
            this->processNodeCard(
                    *boost::dynamic_pointer_cast<MCServerMessageSpec::ReadNodeCardEnvReply>( reply )
                    );
        }
        _insertion_time += boost::posix_time::microsec_clock::local_time() - start;
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    LOG_TRACE_MSG( __FUNCTION__ << " end " << name );
}

void
Optical::createTimers()
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

    _opticalInsert.reset();
    _opticalDataInsert.reset();
    _opticalChannelDataInsert.reset();
    _connection.reset();

    this->wait();
}

} } } // namespace mmcs::server::env
