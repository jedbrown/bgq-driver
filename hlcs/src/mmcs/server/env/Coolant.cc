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

#include "Coolant.h"

#include "McServerConnection.h"
#include "types.h"

#include <db/include/api/tableapi/gensrc/DBTCoolantenvironment.h>
#include <db/include/api/cxxdb/cxxdb.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <xml/include/c_api/MCServerMessageSpec.h>

LOG_DECLARE_FILE( "mmcs.server" );

namespace mmcs {
namespace server {
namespace env {

Coolant::Coolant(
            boost::asio::io_service& io_service
        ) :
    Polling( io_service, 300 )
{

}

cxxdb::ConnectionPtr
Coolant::prepareInsert(
        cxxdb::UpdateStatementPtr& insert
        )
{
    const cxxdb::ConnectionPtr result = BGQDB::DBConnectionPool::Instance().getConnection();
    if ( !result ) {
        return result;
    }

    // prepare insert statements
    BGQDB::ColumnsBitmap columns;
    columns.set();
    columns.reset( BGQDB::DBTCoolantenvironment::TIME );
    BGQDB::DBTCoolantenvironment ce( columns );
    insert = result->prepareUpdate(
            ce.getInsertStatement(),
            ce.calcColumnNames()
            );

    return result;
}

void
Coolant::impl(
        const McServerConnection::Ptr& mc_server
        )
{

    const Timer::Ptr database_timer = this->time()->subFunction("connect to database");
    database_timer->dismiss();

    cxxdb::ConnectionPtr connection;
    cxxdb::UpdateStatementPtr insert;
    connection = this->prepareInsert( insert );

    if ( !connection ) {
        LOG_ERROR_MSG("Could not get database connection.");
        this->wait();
        return;
    }

    database_timer->dismiss( false );
    database_timer->stop();

    const Timer::Ptr target_set_timer = this->time()->subFunction("make and open target set" );
    target_set_timer->dismiss();

    MCServerMessageSpec::MakeTargetSetRequest request("EnvMonCoolant","EnvMonCoolant", true);
    request._expression.push_back("R..-L$");

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Coolant::makeTargetSetHandler,
                boost::static_pointer_cast<Coolant>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                insert,
                target_set_timer
                )
            );
}

void
Coolant::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
        const Timer::Ptr& timer
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    const MCServerMessageSpec::OpenTargetRequest request( "EnvMonCoolant","EnvMonCoolant", MCServerMessageSpec::RAAW, true);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Coolant::openTargetHandler,
                boost::static_pointer_cast<Coolant>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                insert,
                timer
                )
            );
}

void
Coolant::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
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

    MCServerMessageSpec::ReadCoolantMonitorEnvRequest request;
    request._set = "EnvMonCoolant";

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &Coolant::readHandler,
                boost::static_pointer_cast<Coolant>( shared_from_this() ),
                _1,
                reply._handle,
                mc_server,
                connection,
                insert,
                mc_timer
                )
            );
}

void
Coolant::readHandler(
        std::istream& response,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
        const Timer::Ptr& timer
        )
{
    LOG_TRACE_MSG( __FUNCTION__ );

    MCServerMessageSpec::ReadCoolantMonitorEnvReply reply;
    reply.read( response );

    timer->stop();

    const Timer::Ptr database_timer = this->time()->subFunction("database insertion");
    database_timer->dismiss();

    cxxdb::Transaction tx( *connection );
    try {
        for (
                std::vector<MCServerMessageSpec::CoolMonEnv>::const_iterator cmon = reply._coolMons.begin();
                cmon != reply._coolMons.end();
                ++cmon
            )
        {
            if (cmon->_error == CARD_NOT_PRESENT) continue;
            if (cmon->_error == CARD_NOT_UP) continue;
            if (cmon->_error == UNEXPECTED_DEVICE) continue;
            if (cmon->_error) {
                LOG_ERROR_MSG("Error reading environmentals from: " << cmon->_lctn);
                RasEventImpl ras(0x00061005);
                ras.setDetail(RasEvent::LOCATION, cmon->_lctn);
                RasEventHandlerChain::handle(ras);
                BGQDB::putRAS(ras);
                continue;
            }

            insert->parameters()[ BGQDB::DBTCoolantenvironment::LOCATION_COL ].set(
                    cmon->_lctn
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::INLETFLOWRATE_COL ].cast(
                    cmon->_supplyFlowRate
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::OUTLETFLOWRATE_COL ].cast(
                    cmon->_returnFlowRate
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::COOLANTPRESSURE_COL ].cast(
                    cmon->_supplyPressure
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::DIFFPRESSURE_COL ].cast(
                    cmon->_diffPressure
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::INLETCOOLANTTEMP_COL ].cast(
                    cmon->_supplyCoolantTemp
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::OUTLETCOOLANTTEMP_COL ].cast(
                    cmon->_returnCoolantTemp
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::DEWPOINTTEMP_COL ].cast(
                    cmon->_ambientDewPoint
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::AMBIENTTEMP_COL ].cast(
                    cmon->_ambientTemp
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::AMBIENTHUMIDITY_COL ].cast(
                    cmon->_ambientHumidity
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::SYSTEMPOWER_COL ].cast(
                    cmon->_systemPower
                    );
            insert->parameters()[ BGQDB::DBTCoolantenvironment::SHUTOFFCAUSE_COL ].cast(
                    cmon->_shutoffCauseStatus
                    );

            insert->execute();
        }
        connection->commit();
        database_timer->dismiss( false );
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
    }

    this->closeTarget(
            mc_server,
            "EnvMonCoolant",
            handle,
            boost::bind(
                &Polling::wait,
                boost::static_pointer_cast<Coolant>( shared_from_this() )
                )
            );
}

} } } // namespace mmcs::server::env
