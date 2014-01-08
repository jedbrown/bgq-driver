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

#include "ListMidplanes.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListMidplanes*
ListMidplanes::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(common::ADMIN);
    return new ListMidplanes("list_midplanes", "list_midplanes", commandAttributes);
}

void
ListMidplanes::execute(deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << mmcs_client::FAIL << "Unable to obtain database connection" << mmcs_client::DONE;
        return;
    }

    string sqlstr;
    SQLLEN ind[7];
    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    char bpid[7], bpstatus[2], block[33], blockstatus[2], subd[2], compnodepos[5], numnodes[12];
    char curbpid[7] = "prev";

    sqlstr =  "select a.bpid ,a.status as BPSTATUS,case blockstatus when 'F' then x.blockid when '' then x.blockid else a.blockid end as BLOCK, "
        + string(" case blockstatus when 'F' then x.status when '' then x.status else blockstatus end as BLOCKSTATUS,case ")
        + string(" when x.blockid is not null then 'Y' else 'N' end as SUBDIVIDED, x.nodecardpos, char(x.numnodes) ")
        + string(" from bgqbpblockstatus  a left join   table  ")
        + string(" (select posinmachine,a.blockid,a.status,min(nodecardpos) as nodecardpos,size as numnodes ")
        + string(" from bgqblock a,bgqsmallblock b,bgqblocksize c where a.status <> 'F' ")
        + string(" and a.blockid = b.blockid and a.blockid = c.blockid group by posinmachine,a.blockid,a.status,size) as x on a.bpid = x.posinmachine ")
        + string(" order by bpid, nodecardpos");

    tx.execQuery(sqlstr.c_str(), &hstmt);
    SQLBindCol(hstmt, 1, SQL_C_CHAR, bpid,        7, &ind[0]);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, bpstatus,    2, &ind[1]);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, block,      33, &ind[2]);
    SQLBindCol(hstmt, 4, SQL_C_CHAR, blockstatus, 2, &ind[3]);
    SQLBindCol(hstmt, 5, SQL_C_CHAR, subd,        2, &ind[4]);
    SQLBindCol(hstmt, 6, SQL_C_CHAR, compnodepos, 5, &ind[5]);
    SQLBindCol(hstmt, 7, SQL_C_CHAR, numnodes,   12, &ind[6]);
    sqlrc = SQLFetch(hstmt);
    reply << mmcs_client::OK;
    reply << "ID      STATUS   SUBDIVIDED BLOCK                            BLOCKSTATUS COMPNODEPOS NUMNODES\n";

    for (;sqlrc==SQL_SUCCESS;)   {
        reply.setf( ios::left);
        if (ind[2]==SQL_NULL_DATA)
            reply << setw(7) << bpid << " " << setw(9) << bpstatus
                << setw(11) << subd <<  "\n";
        else {
            if (strcmp(curbpid,bpid)==0)
                reply << setw(7) << " " <<  " " << setw(9) << " "
                    << setw(11) << " " << setw(33) <<    block
                    << setw(12) << blockstatus
                    << setw(12) << ((ind[5]==SQL_NULL_DATA) ? " " : compnodepos)
                    << setw(0)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes) << "\n";
            else
                reply << setw(7) << bpid << " " << setw(9) << bpstatus
                    << setw(11) << subd << setw(33) <<    block
                    << setw(12) << blockstatus
                    << setw(12) << ((ind[5]==SQL_NULL_DATA) ? " " : compnodepos)
                    << setw(0)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes) << "\n";
        }

        strcpy(curbpid,bpid);
        sqlrc = SQLFetch(hstmt);
    }

    reply << mmcs_client::DONE;
    SQLCloseCursor(hstmt);
}

void
ListMidplanes::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints midplane information"
          << ";for the machine, including blocks that are"
          << ";booted on each midplane."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
