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

#include "BulkPower.h"

#include "McServerConnection.h"
#include "types.h"
#include "utility.h"

#include <db/include/api/tableapi/gensrc/DBTBulkpowerenvironment.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

void
processBulks(
        const MCServerMessageSpec::ReadBulkPowerEnvReply* mcBPReply,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& bulkInsert
        )
{
    for (
        std::vector<MCServerMessageSpec::BpmsEnv>::const_iterator bp = mcBPReply->_bpms.begin();
        bp != mcBPReply->_bpms.end();
        ++bp
        )
    {
        if (static_cast<int>(bp->_error) == CARD_NOT_PRESENT) continue;
        if (static_cast<int>(bp->_error) == CARD_NOT_UP) continue;
        if (bp->_error) {
            LOG_ERROR_MSG("Error reading environmentals from: " << bp->_location);
            RasEventImpl noContact(0x00061003);
            noContact.setDetail(RasEvent::LOCATION, bp->_location);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
            continue;
        }

        cxxdb::Transaction tx( *connection );
        for (
                std::vector<MCServerMessageSpec::BpmEnv>::const_iterator bpm = bp->_bpms.begin();
                bpm != bp->_bpms.end();
                ++bpm
            )
        {
            bulkInsert->parameters()[ BGQDB::DBTBulkpowerenvironment::LOCATION_COL ].set( bpm->_location );
            bulkInsert->parameters()[ BGQDB::DBTBulkpowerenvironment::INPUTVOLTAGE_COL ].cast( bpm->_inputVoltage );
            bulkInsert->parameters()[ BGQDB::DBTBulkpowerenvironment::INPUTCURRENT_COL ].cast( bpm->_inputCurrent );
            bulkInsert->parameters()[ BGQDB::DBTBulkpowerenvironment::OUTPUTVOLTAGE_COL ].cast( bpm->_outputVoltage51V );
            bulkInsert->parameters()[ BGQDB::DBTBulkpowerenvironment::OUTPUTCURRENT_COL ].cast( bpm->_outputCurrent51V );

            bulkInsert->execute();
        }
        connection->commit();
    }
}

BulkPower::BulkPower(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 300 )
{

}

cxxdb::ConnectionPtr
BulkPower::prepareInserts(
        cxxdb::UpdateStatementPtr& bulkInsert
        )
{
    const cxxdb::ConnectionPtr result = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !result ) {
        return result;
    }

    // prepare insert statements
    BGQDB::ColumnsBitmap columns;
    columns.set();
    columns.reset( BGQDB::DBTBulkpowerenvironment::TIME );
    BGQDB::DBTBulkpowerenvironment bpe( columns );
    bulkInsert = result->prepareUpdate(
            bpe.getInsertStatement(),
            bpe.calcColumnNames()
            );

    return result;
}

void
BulkPower::impl(
        const McServerConnection::Ptr& mc_server
        )
{
    const Timer::Ptr database_timer = this->time()->subFunction("connect to database");
    database_timer->dismiss();

    cxxdb::ConnectionPtr connection;
    cxxdb::UpdateStatementPtr bulkInsert;
    connection = this->prepareInserts( bulkInsert );

    if ( !connection ) {
        LOG_ERROR_MSG("Could not get database connection");
        this->wait();
        return;
    }

    database_timer->dismiss( false );
    database_timer->stop();

    const Timer::Ptr target_set_timer = this->time()->subFunction("make and open target set" );
    target_set_timer->dismiss();

    MCServerMessageSpec::MakeTargetSetRequest request("EnvMonBulk", "EnvMonBulk", true);
    request._expression.push_back("R..-M.-S$");
    const IoDrawers ioDrawers( getIoRacks(connection) );
    BOOST_FOREACH( const std::string& i, ioDrawers ) {
        request._expression.push_back(i);
    }

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &BulkPower::makeTargetSetHandler,
                boost::static_pointer_cast<BulkPower>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                bulkInsert,
                target_set_timer
                )
            );
}

void
BulkPower::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& bulkInsert,
        const Timer::Ptr& timer
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    const MCServerMessageSpec::OpenTargetRequest request( "EnvMonBulk","EnvMonBulk", MCServerMessageSpec::RAAW, true);
    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &BulkPower::openTargetHandler,
                boost::static_pointer_cast<BulkPower>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                bulkInsert,
                timer
                )
            );
}

void
BulkPower::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& bulkInsert,
        const Timer::Ptr& timer
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
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

    MCServerMessageSpec::ReadBulkPowerEnvRequest request;
    request._set = "EnvMonBulk";

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &BulkPower::readHandler,
                boost::static_pointer_cast<BulkPower>( shared_from_this() ),
                _1,
                reply._handle,
                mc_server,
                connection,
                bulkInsert,
                mc_timer
                )
            );
}

void
BulkPower::readHandler(
        std::istream& response,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& bulkInsert,
        const Timer::Ptr& timer
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    MCServerMessageSpec::ReadBulkPowerEnvReply reply;
    reply.read( response );

    timer->stop();

    const Timer::Ptr database_timer = this->time()->subFunction("database insertion");
    database_timer->dismiss();

    try {
        processBulks(&reply, connection, bulkInsert);
        database_timer->dismiss( false );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    this->closeTarget(
            mc_server,
            "EnvMonBulk",
            handle,
            boost::bind(
                &Polling::wait,
                boost::static_pointer_cast<BulkPower>( shared_from_this() )
                )
            );
}

} } } // namespace mmcs::server::env
