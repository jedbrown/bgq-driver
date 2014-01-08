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

#include "ListIo.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

ListIo*
ListIo::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(common::ADMIN);
    return new ListIo("list_io", "list_io [ioblock]", commandAttributes);
}

void
ListIo::execute(
        deque<string> args,
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
    SQLLEN ind[8];
    SQLHANDLE hstmt;
    char ioid[7] = {0};
    char iostatus[2] = {0};
    char block[33] = {0};
    char blockstatus[2] = {0};
    char subd[2] = {0};
    char nodepos[5] = {0};
    char numnodes[12] = {0};
    char cn[16] = {0};
    char curioid[7] = "prev";
    char curblock[33] = "prev";

    if (args.size() == 1) {
        sqlstr =  "select a.location ,a.status as IOSTATUS,x.blockid,x.status, "
            + string("    case when numnodes < 8 then 'Y' else 'N' end as SUBD, nodepos,  ")
            + string("    char(case when numnodes < 8 then numnodes else null end),  y.source ")
            + string("     from bgqiodrawer   a         left join   table      ")
            + string("     (select substr(location,1,6) as drawer,a.blockid,a.status,min(substr(location,8,3)) as nodepos,numionodes as numnodes  ")
            + string("       from bgqblock a,bgqioblockmap b   ")
            + string("      where a.blockid='") + args[0] + string("' ")
            + string("    and a.blockid = b.blockid  group by substr(location,1,6),a.blockid,a.status,numionodes)   ")
            + string("      as x on a.location = x.drawer  left join table   ")
            + string("     (select substr(destination,1,6) as drawer ,source  from bgqcniolink a, bgqioblockmap b   ")
            + string("      where (ion = location  or (length(rtrim(location))=6 and location = substr(ion,1,6)))   ")
            + string("      and  blockid = '") + args[0] + string("' )  as y on a.location = y.drawer  ")
            + string("      order by location, nodepos , source  ");
    } else if (args.size() == 0) {
        sqlstr =  "select a.drawer ,a.status as IOSTATUS,case blockstatus when 'F' then x.blockid when '' then x.blockid else a.blockid end as BLOCK, "
            + string("    case blockstatus when 'F' then x.status when '' then x.status else blockstatus end as BLOCKSTATUS,case  ")
            + string("    when x.blockid is not null then 'Y' else 'N' end as SUBDIVIDED, x.nodepos, char(x.numnodes)  ")
            + string("    from bgqioblockstatus  a left join   table   ")
            + string("   (select substr(location,1,6) as drawer,a.blockid,a.status,min(substr(location,8,3)) as nodepos,numionodes as numnodes  ")
            + string("   from bgqblock a,bgqioblockmap b ")
            + string("  where a.status <>  'F' ")
            + string("  and a.blockid = b.blockid and length(trim(location)) > 6 group by substr(location,1,6),a.blockid,a.status,numionodes) ")
            + string("   as x on a.drawer = x.drawer ")
            + string("    order by drawer, nodepos ; ");
    } else {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    tx.execQuery(sqlstr.c_str(), &hstmt);
    SQLBindCol(hstmt, 1, SQL_C_CHAR, ioid,        7, &ind[0]);
    SQLBindCol(hstmt, 2, SQL_C_CHAR, iostatus,    2, &ind[1]);
    SQLBindCol(hstmt, 3, SQL_C_CHAR, block,      33, &ind[2]);
    SQLBindCol(hstmt, 4, SQL_C_CHAR, blockstatus, 2, &ind[3]);
    SQLBindCol(hstmt, 5, SQL_C_CHAR, subd,        2, &ind[4]);
    SQLBindCol(hstmt, 6, SQL_C_CHAR, nodepos,     5, &ind[5]);
    SQLBindCol(hstmt, 7, SQL_C_CHAR, numnodes,   12, &ind[6]);

    if  (args.size() > 0) {
        SQLBindCol(hstmt, 8, SQL_C_CHAR, cn,   16, &ind[7]);
        reply << mmcs_client::OK;
        reply << "ID      STATUS SUBDIVIDED BLOCK                            BLOCKSTATUS NODEPOS NUMNODES COMPUTENODE\n";
    } else {
        reply << mmcs_client::OK;
        reply << "ID      STATUS SUBDIVIDED BLOCK                            BLOCKSTATUS NODEPOS NUMNODES\n";
    }

    SQLRETURN sqlrc = SQLFetch(hstmt);

    BGQDB::trim_right_spaces(nodepos);
    BGQDB::trim_right_spaces(numnodes);
    while ( sqlrc==SQL_SUCCESS ) {
        reply.setf( ios::left);
        if (ind[2]==SQL_NULL_DATA)
            reply << setw(7)  << ioid << " " << setw(7) << iostatus
                << setw(11) << subd <<  "\n";
        else {
            if (args.size() > 0) {
                if (strcmp(curblock,block)==0 && strcmp(curioid,ioid)==0) {
                    reply << setw(88) << " " << setw(15) << cn << "\n";
                } else
                    if (strcmp(curioid,ioid)==0)  {
                        reply << setw(7)  << " " <<  " " << setw(7) << " "
                            << setw(11) << " " << setw(33) <<    block
                            << setw(12) << blockstatus
                            << setw(8)  << ((ind[5]==SQL_NULL_DATA) ? " " : nodepos)
                            << setw(9)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes)
                            << setw(15)  << cn << "\n";
                    } else {
                        reply << setw(7)  << ioid << " " << setw(7) << iostatus
                            << setw(11) << subd << setw(33) <<    block
                            << setw(12) << blockstatus
                            << setw(8)  << ((ind[5]==SQL_NULL_DATA) ? " " : nodepos)
                            << setw(9)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes)
                            << setw(15)  << cn << "\n";
                    }
            } else {
                if (strcmp(curioid,ioid)==0) {
                    reply << setw(7)  << " " <<  " " << setw(7) << " "
                        << setw(11) << " " << setw(33) <<    block
                        << setw(12) << blockstatus
                        << setw(8)  << ((ind[5]==SQL_NULL_DATA) ? " " : nodepos)
                        << setw(9)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes) << "\n";
                } else {
                    reply << setw(7)  << ioid << " " << setw(7) << iostatus
                        << setw(11) << subd << setw(33) <<    block
                        << setw(12) << blockstatus
                        << setw(8)  << ((ind[5]==SQL_NULL_DATA) ? " " : nodepos)
                        << setw(9)  << ((ind[6]==SQL_NULL_DATA) ? " " : numnodes) << "\n";
                }
            }
        }

        strcpy(curioid,ioid);
        strcpy(curblock,block);
        sqlrc = SQLFetch(hstmt);
    }

    reply << mmcs_client::DONE;
    SQLCloseCursor(hstmt);
}

void
ListIo::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints I/O drawer information for the machine,"
          << ";including blocks that are booted on each drawer."
          << ";If the name of an I/O block is provided, the output will include details for that block."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
