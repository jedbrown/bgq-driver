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

#include "ServiceCard.h"

#include "McServerConnection.h"
#include "types.h"

#include <db/include/api/tableapi/gensrc/DBTServicecardenvironment.h>
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
processSC(
        const MCServerMessageSpec::ReadServiceCardEnvReply* mcSCReply,
        const cxxdb::UpdateStatementPtr& serviceInsert
        )
{
    for (
            std::vector<MCServerMessageSpec::ServiceCardEnv>::const_iterator sc = mcSCReply->_serviceCards.begin();
            sc != mcSCReply->_serviceCards.end();
            ++sc
        )
    {
        if (static_cast<int>(sc->_error) == CARD_NOT_PRESENT) continue;
        if (static_cast<int>(sc->_error) == CARD_NOT_UP) continue;
        if (sc->_error) {
            LOG_ERROR_MSG("Error reading environmentals from: " << sc->_location);
            RasEventImpl noContact(0x00061002);
            noContact.setDetail(RasEvent::LOCATION, sc->_location);
            RasEventHandlerChain::handle(noContact);
            BGQDB::putRAS(noContact);
            continue;
        }

        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::LOCATION_COL ].set( sc->_location );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV12P_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV12PVoltage)
                );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV12R5_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV12R5Voltage)
                );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV15P_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV15PVoltage)
                );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV25P_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV25PVoltage)
                );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV33P_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV33PVoltage)
                );
        serviceInsert->parameters()[ BGQDB::DBTServicecardenvironment::VOLTAGEV50P_COL ].cast(
                boost::lexical_cast<double>(sc->_powerRailV50PVoltage)
                );

        serviceInsert->execute();
    }
}

ServiceCard::ServiceCard(
        boost::asio::io_service& io_service
        ) :
    Polling( io_service, 1800 )
{

}

cxxdb::ConnectionPtr
ServiceCard::prepareInsert(
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
    columns.reset( BGQDB::DBTServicecardenvironment::TIME );
    BGQDB::DBTServicecardenvironment sce( columns );
    bulkInsert = result->prepareUpdate(
            sce.getInsertStatement(),
            sce.calcColumnNames()
            );

    return result;
}

void
ServiceCard::impl(
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

    MCServerMessageSpec::MakeTargetSetRequest request("EnvMonSC", "EnvMonSC", true);
    request._expression.push_back("R..");

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &ServiceCard::makeTargetSetHandler,
                boost::static_pointer_cast<ServiceCard>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                insert,
                target_set_timer
                )
            );
}

void
ServiceCard::makeTargetSetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
        const Timer::Ptr& timer
        )
{
    MCServerMessageSpec::MakeTargetSetReply reply;
    reply.read( response );

    const MCServerMessageSpec::OpenTargetRequest request( "EnvMonSC","EnvMonSC", MCServerMessageSpec::RAAW, true);

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &ServiceCard::openTargetHandler,
                boost::static_pointer_cast<ServiceCard>( shared_from_this() ),
                _1,
                mc_server,
                connection,
                insert,
                timer
                )
            );
}

void
ServiceCard::openTargetHandler(
        std::istream& response,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
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
    LOG_TRACE_MSG("Opened target set with handle " << reply._handle);

    timer->dismiss( false );
    timer->stop();

    const Timer::Ptr mc_timer = this->time()->subFunction("mc");

    MCServerMessageSpec::ReadServiceCardEnvRequest request;
    request._set = "EnvMonSC";

    mc_server->send(
            request.getClassName(),
            request,
            boost::bind(
                &ServiceCard::readHandler,
                boost::static_pointer_cast<ServiceCard>( shared_from_this() ),
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
ServiceCard::readHandler(
        std::istream& response,
        const int handle,
        const McServerConnection::Ptr& mc_server,
        const cxxdb::ConnectionPtr& connection,
        const cxxdb::UpdateStatementPtr& insert,
        const Timer::Ptr& timer
        )
{
    MCServerMessageSpec::ReadServiceCardEnvReply reply;
    reply.read( response );

    timer->stop();

    const Timer::Ptr database_timer = this->time()->subFunction("database insertion");
    database_timer->dismiss();

    cxxdb::Transaction tx( *connection );
    processSC(&reply, insert);
    connection->commit();

    database_timer->dismiss( false );

    this->closeTarget(
            mc_server,
            "EnvMonSC",
            handle,
            boost::bind(
                &Polling::wait,
                boost::static_pointer_cast<ServiceCard>( shared_from_this() )
                )
            );
}

} } } // namespace mmcs::server::env
