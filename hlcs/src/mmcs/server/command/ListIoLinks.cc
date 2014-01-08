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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#include "ListIoLinks.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/assign.hpp>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListIoLinks*
ListIoLinks::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new ListIoLinks("list_io_links", "list_io_links <block>", commandAttributes);
}

std::vector<std::string>
ListIoLinks::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
)
{
    std::vector<std::string> block_to_use;
    block_to_use.push_back(cmdString[0]);
    return block_to_use;
}

void
ListIoLinks::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    std::vector<std::string> validnames;
    if ( !args.empty() ) {
        validnames.push_back( args[0] );
    }
    return this->execute(args, reply, pController, pTarget, &validnames);
}

void
ListIoLinks::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
)
{
    if ( args.empty() ) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }
    if ( !validnames ) {
        reply << mmcs_client::FAIL << "Empty valid names?" << mmcs_client::DONE;
        return;
    }

    BGQDB::BlockInfo bInfo;
    const BGQDB::STATUS result = BGQDB::getBlockInfo(validnames->at(0), bInfo);

    if ( result == BGQDB::CONNECTION_ERROR ) {
        reply << mmcs_client::FAIL << "Could not get database connection." << mmcs_client::DONE;
        return;
    } else if ( result == BGQDB::NOT_FOUND ) {
        reply << mmcs_client::FAIL << "Block " << validnames->at(0) << " not found" << mmcs_client::DONE;
        return;
    } else if ( result == BGQDB::DB_ERROR ) {
        reply << mmcs_client::FAIL << "Database query failed" << mmcs_client::DONE;
        return;
    } else if ( result == BGQDB::OK && bInfo.ionodes > 0 ) {
        string sqlstr;
        SQLLEN ind[3];
        SQLHANDLE hstmt;
        char ion[11], cnstatus[2], cnblock[33];

        sqlstr =  "select cnblock, cnblockstatus, ion from bgqcnioblockmap "
            + string("  where cnblockstatus <>  'F' ")
            + string("  and ioblock = '") + validnames->at(0) + string("' ")
            + string("    order by ion ");
        BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
        tx.execQuery(sqlstr.c_str(), &hstmt);
        SQLBindCol(hstmt, 1, SQL_C_CHAR, cnblock, 33, &ind[0]);
        SQLBindCol(hstmt, 2, SQL_C_CHAR, cnstatus, 2, &ind[1]);
        SQLBindCol(hstmt, 3, SQL_C_CHAR, ion,     11, &ind[2]);

        SQLRETURN sqlrc = SQLFetch(hstmt);

        if (sqlrc == SQL_SUCCESS) {
            reply << mmcs_client::OK;
            reply << "CNBLOCK                           STATUS  ION\n";
            BGQDB::trim_right_spaces(cnblock);
            BGQDB::trim_right_spaces(ion);
        } else {
            reply << mmcs_client::FAIL << "No I/O links for block: " << validnames->at(0) << mmcs_client::DONE;
        }

        for (;sqlrc==SQL_SUCCESS;) {
            reply.setf( ios::left);
            reply << setw(33)  << cnblock << " " << setw(7) << cnstatus << " "
                << setw(12) << ion << "\n";

            sqlrc = SQLFetch(hstmt);
        }

        reply << mmcs_client::DONE;
        SQLCloseCursor(hstmt);
    } else if ( result == BGQDB::OK && bInfo.ionodes == 0 ) {
        try {
            const cxxdb::ConnectionPtr connection(
                    BGQDB::DBConnectionPool::instance().getConnection()
                    );
            const cxxdb::QueryStatementPtr statement(
                    connection->prepareQuery(
                        "SELECT " + BGQDB::DBVCniolink::SOURCE_COL + ", " + BGQDB::DBVCniolink::ION_COL +
                        ", " + BGQDB::DBVCniolink::STATUS_COL + ", " + BGQDB::DBVCniolink::IONSTATUS_COL +
                        " FROM " + BGQDB::DBVCniolink().getTableName() +
                        " WHERE " + BGQDB::DBVCniolink::ION_COL + " in (" +
                        " SELECT " + BGQDB::DBVCnioblockmap::ION_COL + " FROM " + BGQDB::DBVCnioblockmap().getTableName() +
                        " WHERE " + BGQDB::DBVCnioblockmap::CNBLOCK_COL + "=?)" +
                        " AND (" +
                        " substr(" + BGQDB::DBVCniolink::SOURCE_COL + ",1,6) in (" +
                        " SELECT " + BGQDB::DBTBpblockmap::BPID_COL + " FROM " + BGQDB::DBTBpblockmap().getTableName() +
                        " WHERE " + BGQDB::DBTBpblockmap::BLOCKID_COL + "=?) OR " +
                        " substr(" + BGQDB::DBVCniolink::SOURCE_COL + ",1,10) in (" +
                        " SELECT " + BGQDB::DBTSmallblock::POSINMACHINE_COL + " || '-' || " + BGQDB::DBTSmallblock::NODECARDPOS_COL +
                        " FROM " + BGQDB::DBTSmallblock().getTableName() +
                        " WHERE " + BGQDB::DBTSmallblock::BLOCKID_COL + "=?))" +
                        " ORDER by " + BGQDB::DBVCniolink::ION_COL,
                        boost::assign::list_of( "block" )( "block" )( "block" )
                        )
                    );
            statement->parameters()[ "block" ].set( validnames->at(0) );
            const cxxdb::ResultSetPtr result( statement->execute() );

            reply << mmcs_client::OK;
            reply << "COMPUTE NODE\t";
            reply << "I/O NODE\t";
            reply << "STATUS\t";
            reply << "LINK STATUS\n";

            while ( result->fetch() ) {
                reply << result->columns()[ BGQDB::DBVCniolink::SOURCE_COL ].getString() << "\t";
                reply << result->columns()[ BGQDB::DBVCniolink::ION_COL ].getString() << "\t";
                reply << result->columns()[ BGQDB::DBVCniolink::IONSTATUS_COL ].getString() << "\t";
                reply << result->columns()[ BGQDB::DBVCniolink::STATUS_COL ].getString() << "\n";
            }

            reply << mmcs_client::DONE;
        } catch ( const std::exception& e ) {
            reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
            return;
        }
    } else {
        reply << mmcs_client::FAIL << "Unknown database error." << mmcs_client::DONE;
        return;
    }
}

void
ListIoLinks::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints I/O link information for a specific block."
          << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
