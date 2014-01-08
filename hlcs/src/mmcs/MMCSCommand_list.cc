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
/* (C) Copyright IBM Corp.  2005, 2011                              */
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

/*!
 * \file MMCSCommand_list.cc
 */

#include "MMCSCommand_list.h"
#include "MMCSCommandReply.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/dataconv.h>
#include <db/include/api/cxxdb/cxxdb.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/assign/list_of.hpp>
#include <boost/foreach.hpp>
#include <boost/scoped_ptr.hpp>

LOG_DECLARE_FILE( "mmcs" );

MMCSCommand_list_blocks*
MMCSCommand_list_blocks::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.mmcsServerCommand(true);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_list_blocks("list_blocks", "list_blocks", commandAttributes);
}

void
MMCSCommand_list_blocks::execute(deque<string> args,
                 MMCSCommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget)
{
    std::vector<std::string> validnames;
    execute(args, reply, pController, pTarget, &validnames);
}

void
MMCSCommand_list_blocks::execute(deque<string> args,
                 MMCSCommandReply& reply,
                 DBConsoleController* pController,
                 BlockControllerTarget* pTarget,
                 std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;
    PthreadMutexHolder mutex;

    int mutex_rc = mutex.Lock(&pController->_blockMapMutex);
    assert(mutex_rc == 0);
    reply << OK;
    if (pController->_blockMap.size() != 0)
        {
            for(BlockMap::iterator it = pController->_blockMap.begin(); it != pController->_blockMap.end(); ++it)
                {
                    result = BGQDB::getBlockInfo(it->first, bInfo);
                    if (result != BGQDB::OK)
                        strcpy(bInfo.status, "?");

                    bool valid = true;
                    DBBlockPtr pBlock;
                    std::string blockname;
#ifdef WITH_SECURITY
                    if(std::find(validnames->begin(), validnames->end(), it->first) == validnames->end())
                        valid = false;
                    pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());
                    if(pBlock != 0) {
                        blockname = pBlock->getBase()->getBlockName();
                    }
#endif

                    if(valid || it->first == blockname) {
                        reply << setw(16) << left << it->first << " " << bInfo.status << " "
                              << left << setw(8) << it->second->getBase()->getUserName() << "(" << it->second->getBase()->peekBlockThreads() << ")" ;
                        if (it->second->getBase()->isConnected())
                            reply << "\tconnected";
                        if (it->second->getBase()->peekDisconnecting())
                            reply << "\tdisconnecting - " << it->second->getBase()->disconnectReason();
                        if (it->second->getBase()->getRedirectSock() != 0)
                            reply << "\tredirecting";
                        reply << ";";
                    }
                }
        }
    reply << DONE;
    mutex.Unlock();
    return;
}

std::vector<std::string>
MMCSCommand_list_blocks::getBlockObjects(std::deque<std::string>& cmdString,
                                  DBConsoleController* pController) {
    PthreadMutexHolder holder;
    holder.Lock(&DBConsoleController::getBlockMapMutex());
    std::vector<std::string> retvec;
    for(BlockMap::iterator i = DBConsoleController::getBlockMap().begin();
        i != DBConsoleController::getBlockMap().end(); ++i) {
        retvec.push_back(i->first);
    }
    return retvec;
}

void
MMCSCommand_list_blocks::help(deque<string> args,
        MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
        << ";Lists allocated blocks."
        << ";Output includes user, number of consoles started, and if output is redirected to console."
        << DONE;
}


MMCSCommand_list_users*
MMCSCommand_list_users::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(ADMIN);
    return new MMCSCommand_list_users("list_users", "list_users", commandAttributes);
}


void
MMCSCommand_list_users::execute(deque<string> args,
        MMCSCommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    PthreadMutexHolder mutex;
    mutex.Lock(&pController->_midplaneControllerListMutex);
    reply << OK;
    if (pController->_midplaneControllerList.size() != 0)
    {
        for(MidplaneControllerList::iterator it = pController->_midplaneControllerList.begin(); it != pController->_midplaneControllerList.end(); ++it)
        {
            // print thread id
            MMCSThread* mmcsThread = (*it)->getMMCSThread();
            reply << "Thread: ";
            if (mmcsThread)
            {
                reply << mmcsThread->getThreadId() << " (" << mmcsThread->getThreadName() << ")\t";
            }
            else
            {
                reply << "n/a\t";
            }

            // print user name
            reply << " User: " << setw(8) << left << (*it)->getUser().getUser();

            // print block info
            reply << " Block: " << setw(16) << left;
            //        DBBlockController* dbBlockController = (DBBlockController*)((*it)->_blockController.get());
            DBBlockPtr dbBlockController = boost::dynamic_pointer_cast<DBBlockController>((*it)->_blockController);
            if (dbBlockController != NULL)
            {
                reply << dbBlockController->getBase()->getBlockName();
                if (dbBlockController->getBase()->peekDisconnecting())
                {
                    reply << "\tdisconnecting - " << dbBlockController->getBase()->disconnectReason();
                }
            }
            else
            {
                reply << "n/a";
            }

            // is this DBConsoleController used for redirection?
            if ((*it)->getRedirecting())
            {
                reply << "\tredirecting";
            }

            reply << "\n";
        }
    }
    reply << DONE;
    mutex.Unlock();
    return;
}

void
MMCSCommand_list_users::help(deque<string> args,
        MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
        << ";List mmcs users."
        << ";Output includes thread number, block ID and if output is redirected to console."
        << DONE;
}


MMCSCommand_list*
MMCSCommand_list::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);
    commandAttributes.bgadminAuth(true);
    return new MMCSCommand_list("list", "list <db2table> [<value>]", commandAttributes);
}

void
MMCSCommand_list::execute(deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    if (args.size() < 1)
    {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection())
    {
        reply << FAIL << "Unable to obtain database connection" << DONE;
        return;
    }

    boost::shared_ptr<BGQDB::DBObj> n;
    string wc;

    if (args[0] == "bgqnode")
    {
        n.reset( new BGQDB::DBVNode );
        if (args.size() == 2) wc="where location='"+args[1]+"'";
    }
    else if (args[0] == "bgqionode")
    {
        n.reset( new BGQDB::DBVIonode );
        if (args.size() == 2) wc="where location='"+args[1]+"'";
    }
    else if (args[0] == "bgqnodecard")
    {
        n.reset( new BGQDB::DBVNodecard );
        if (args.size() == 2) wc="where location='"+args[1]+"'";
    }
    else if (args[0] == "bgqmidplane")
    {
        n.reset( new BGQDB::DBVMidplane );
        if (args.size() == 2) wc="where location='"+args[1]+"'";
    }
    else if (args[0] == "bgqiodrawer")
    {
        n.reset( new BGQDB::DBVIodrawer );
        if (args.size() == 2) wc="where location='"+args[1]+"'";
    }
    else if (args[0] == "bgqmachine")
    {
        n.reset( new BGQDB::DBTMachine );
        if (args.size() == 2) wc="where serialnumber='"+args[1]+"'";
    }
    else if (args[0] == "bgqblock")
    {
        n.reset( new BGQDB::DBTBlock );
        if (args.size() == 2) wc="where blockid like '"+args[1]+" %' or blockid like '" + args[1] + "'";
    }
    else if (args[0] == "bgqproducttype")
    {
        n.reset( new BGQDB::DBTProducttype );
        if (args.size() == 2) wc="where productid='"+args[1]+"'";
    }
    else if (args[0] == "bgqeventlog")
    {
        n.reset( new BGQDB::DBTEventlog );
        if (args.size() == 2) wc="where block='"+args[1]+"' order by recid desc optimize for 10 rows" ;
        else
            wc=" order by recid desc optimize for 10 rows";
    }
    else if (args[0] == "bgqjob")
    {
        n.reset( new BGQDB::DBTJob );
        if (args.size() == 2) wc="where id='"+args[1]+"'";
    }
    else if (args[0] == "bgqjob_history")
    {
        n.reset( new BGQDB::DBTJob_history );
        if (args.size() == 2) wc="where id='"+args[1]+"'";
    }
    else if (args[0] == "bgqnodeconfig")
    {
        n.reset( new BGQDB::DBTNodeconfig );
        if (args.size() == 2) wc="where nodeconfig='"+args[1]+"'";
    }
    else
    {
        reply << FAIL
            << "invalid table\n"
            << "valid tables and optional parms are:\n"
            << "  bgqnode <location>\n"
            << "  bgqionode <location>\n"
            << "  bgqnodecard <location>\n"
            << "  bgqmidplane <location>\n"
            << "  bgqiodrawer <location>\n"
            << "  bgqmachine <alias>\n"
            << "  bgqblock <blockid>\n"
            << "  bgqproducttype <productid>\n"
            << "  bgqeventlog <blockid>\n"
            << "  bgqjob <jobid>\n"
            << "  bgqjob_history <jobid>\n"
            << "  bgqnodeconfig <configname>\n"
            << DONE;
        return;
    }

    SQLRETURN rc;
    SET_ALL_COLUMN((*n));
    if (args[0] == "bgqeventlog")
    {
        int rows = 10;
        tx.query(n.get(),wc.c_str()) ;
        rc = tx.fetch(n.get());

        if (rc == SQL_NO_DATA_FOUND)
        {
            reply << FAIL << "no data found "  << DONE;
            return;
        }
        reply << OK << "Most recent " << rows << " RAS events ";

        if (args.size() == 1)
            reply << '\n';
        else
            reply << "for block " << args[1] << '\n';

        while (rc == SQL_SUCCESS && rows>0)
        {
            reply
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_event_time << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_severity  << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_block << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_location << '\n'
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_message << '\n';
            rc = tx.fetch(n.get());
            rows--;
        }
        reply << DONE;
    }
    else
    {
        if (args[0] == "bgqblock")
        {
            bitset<BGQDB::DBTBlock::N_BLOCK_ATTR> cmap;
            cmap.set();
            cmap.reset( boost::static_pointer_cast<BGQDB::DBTBlock>(n)->SECURITYKEY );
            n->_columns = cmap.to_ulong();
        }

        BGQDB::DBTDomainmap dm;
        SET_ALL_COLUMN(dm);
        BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());

        rc = (args.size() == 1) ? tx.query(n.get()) : tx.query(n.get(),wc.c_str()) ;
        rc = tx.fetch(n.get());
        if (rc == SQL_NO_DATA_FOUND)
            reply << FAIL << "No data found" << DONE;
        else
        if (rc != SQL_SUCCESS)
            reply << FAIL << "Invalid argument" << DONE;
        else
            {
                reply << OK;
                while (rc == SQL_SUCCESS)
                    {
                        n->dump(reply);

                        if (args[0] == "bgqnodeconfig") {
                            if (args.size() == 1)
                                wc="where nodeconfig='"+string(boost::static_pointer_cast<BGQDB::DBTNodeconfig>(n)->_nodeconfig)+"'";
                            rc = tx2.query(&dm,wc.c_str());
                            rc = tx2.fetch(&dm);
                            while (rc == SQL_SUCCESS) {
                                dm.dump(reply);
                                rc = tx2.fetch(&dm);
                            }
                            rc = tx2.close(&dm);
                        }

                        rc = tx.fetch(n.get());
                    }
                rc = tx.close(n.get());
                reply << DONE;
            }
    }
}

void
MMCSCommand_list::help(deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Prints the contents of <db2table>."
        << ";Optional <value> specifies contents of a field within that table."
        << ";Following are the only valid table / field combinations that can be used. "
        << ";bgqnode / location "
        << ";bgqionode / location "
        << ";bgqnodecard / location "
        << ";bgqmidplane / location "
        << ";bgqiodrawer / location "
        << ";bgqmachine / serialnumber "
        << ";bgqblock / blockid (wild card with %) "
        << ";bgqproducttype / productid "
        << ";bgqeventlog / block"
        << ";bgqjob / jobid"
        << ";bgqjob_history / jobid"
        << ";bgqnodeconfig / configname"
        << DONE;
}

MMCSCommand_list_midplanes*
MMCSCommand_list_midplanes::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(ADMIN);
    return new MMCSCommand_list_midplanes("list_midplanes", "list_midplanes", commandAttributes);
}

void
MMCSCommand_list_midplanes::execute(deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << FAIL << "Unable to obtain database connection" << DONE;
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
    reply << OK;
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

    reply << DONE;
    SQLCloseCursor(hstmt);
}

void
MMCSCommand_list_midplanes::help(deque<string> args,
        MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Prints midplane information"
        << ";for the machine, including blocks that are"
        << ";booted on each midplane."
        << DONE;
}

MMCSCommand_list_io*
MMCSCommand_list_io::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.bgadminAuth(true);
    commandAttributes.helpCategory(ADMIN);
    return new MMCSCommand_list_io("list_io", "list_io [ioblock]", commandAttributes);
}

void
MMCSCommand_list_io::execute(deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget)
{
    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << FAIL << "Unable to obtain database connection" << DONE;
        return;
    }

    string sqlstr;
    SQLLEN ind[8];
    SQLRETURN sqlrc;
    SQLHANDLE hstmt;
    char ioid[7], iostatus[2], block[33], blockstatus[2], subd[2], nodepos[5], numnodes[12];
    char cn[16];
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
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
    sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, ioid,        7, &ind[0]);
    sqlrc = SQLBindCol(hstmt, 2, SQL_C_CHAR, iostatus,    2, &ind[1]);
    sqlrc = SQLBindCol(hstmt, 3, SQL_C_CHAR, block,      33, &ind[2]);
    sqlrc = SQLBindCol(hstmt, 4, SQL_C_CHAR, blockstatus, 2, &ind[3]);
    sqlrc = SQLBindCol(hstmt, 5, SQL_C_CHAR, subd,        2, &ind[4]);
    sqlrc = SQLBindCol(hstmt, 6, SQL_C_CHAR, nodepos,     5, &ind[5]);
    sqlrc = SQLBindCol(hstmt, 7, SQL_C_CHAR, numnodes,   12, &ind[6]);

    if  (args.size() > 0) {
        sqlrc= SQLBindCol(hstmt, 8, SQL_C_CHAR, cn,   16, &ind[7]);
        reply << OK;
        reply << "ID      STATUS SUBDIVIDED BLOCK                            BLOCKSTATUS NODEPOS NUMNODES COMPUTENODE\n";
    } else {
        reply << OK;
        reply << "ID      STATUS SUBDIVIDED BLOCK                            BLOCKSTATUS NODEPOS NUMNODES\n";
    }

    sqlrc = SQLFetch(hstmt);

    trim_right_spaces(nodepos);
    trim_right_spaces(numnodes);
    for (;sqlrc==SQL_SUCCESS;)   {
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

    reply << DONE;
    SQLCloseCursor(hstmt);
}

void
MMCSCommand_list_io::help(deque<string> args,
                   MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Prints IO drawer information for the machine,"
        << ";including blocks that are booted on each drawer."
        << ";If the name of an IO block is provided, the output will include details for that block."
        << DONE;
}

MMCSCommand_list_io_links*
MMCSCommand_list_io_links::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_list_io_links("list_io_links", "list_io_links <block>", commandAttributes);
}

std::vector<std::string>
MMCSCommand_list_io_links::getBlockObjects(
        std::deque<std::string>& cmdString,
        DBConsoleController* pController
        )
{
    std::vector<std::string> block_to_use;
    block_to_use.push_back(cmdString[0]);
    return block_to_use;
}

void
MMCSCommand_list_io_links::execute(
        deque<string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
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
MMCSCommand_list_io_links::execute(
        std::deque<std::string> args,
        MMCSCommandReply& reply,
        ConsoleController* pController,
        BlockControllerTarget* pTarget,
        std::vector<std::string>* validnames
        )
{
    if ( args.empty() ) {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }
    if ( !validnames ) {
        reply << FAIL << "empty valid names?" << DONE;
        return;
    }

    BGQDB::BlockInfo bInfo;
    const BGQDB::STATUS result = BGQDB::getBlockInfo(validnames->at(0), bInfo);

    if ( result == BGQDB::CONNECTION_ERROR ) {
        reply << FAIL << "could not get database connection." << DONE;
        return;
    } else if ( result == BGQDB::NOT_FOUND ) {
        reply << FAIL << "block " << validnames->at(0) << " not found" << DONE;
        return;
    } else if ( result == BGQDB::DB_ERROR ) {
        reply << FAIL << "DB query failed" << DONE;
        return;
    } else if ( result == BGQDB::OK && bInfo.ionodes > 0 ) {
        string sqlstr;
        SQLLEN ind[3];
        SQLRETURN sqlrc;
        SQLHANDLE hstmt;
        char ion[11], cnstatus[2], cnblock[33];

        sqlstr =  "select cnblock, cnblockstatus, ion from bgqcnioblockmap "
            + string("  where cnblockstatus <>  'F' ")
            + string("  and ioblock = '") + validnames->at(0) + string("' ")
            + string("    order by ion ");
        BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
        sqlrc = tx.execQuery(sqlstr.c_str(), &hstmt);
        sqlrc = SQLBindCol(hstmt, 1, SQL_C_CHAR, cnblock, 33, &ind[0]);
        sqlrc = SQLBindCol(hstmt, 2, SQL_C_CHAR, cnstatus, 2, &ind[1]);
        sqlrc = SQLBindCol(hstmt, 3, SQL_C_CHAR, ion,     11, &ind[2]);


        sqlrc = SQLFetch(hstmt);

        if (sqlrc == SQL_SUCCESS) {
            reply << OK;
            reply << "CNBLOCK                           STATUS  ION\n";
            trim_right_spaces(cnblock);
            trim_right_spaces(ion);
        } else {
            reply << FAIL << "no IO links for block: " << validnames->at(0) << DONE;
        }

        for (;sqlrc==SQL_SUCCESS;)   {
            reply.setf( ios::left);
            reply << setw(33)  << cnblock << " " << setw(7) << cnstatus << " "
                << setw(12) << ion << "\n";

            sqlrc = SQLFetch(hstmt);
        }

        reply << DONE;
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

            reply << OK;
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

            reply << DONE;
        } catch ( const std::exception& e ) {
            reply << FAIL << e.what() << DONE;
            return;
        }
    } else {
        reply << FAIL << "unknown database error." << DONE;
        return;
    }
}

void
MMCSCommand_list_io_links::help(deque<string> args,
                   MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Prints IO link information for a specific block."
        << DONE;
}

MMCSCommand_get_block_info*
MMCSCommand_get_block_info::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.requiresObjNames(true);
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(DEFAULT);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_get_block_info("get_block_info", "get_block_info <blockId>", commandAttributes);
}

std::vector<std::string>
MMCSCommand_get_block_info::getBlockObjects(std::deque<std::string>& cmdString,
                                  DBConsoleController* pController) {
    std::vector<std::string> block_to_use;
    block_to_use.push_back(cmdString[0]);
    return block_to_use;
}

void
MMCSCommand_get_block_info::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::BlockInfo bInfo;

    const BGQDB::STATUS result = BGQDB::getBlockInfo(validnames->at(0), bInfo);


    if(result == BGQDB::OK)
    {
        reply << OK;
        reply << "boot info for block " << args[0] << ":\n";
        reply << "uloader: " << bInfo.uloaderImg << "\n";
        reply << "node config: " << bInfo.nodeConfig << "\n";
        reply << "status: " << bInfo.status << "\n";
        reply << "boot options: " << bInfo.bootOptions << "\n";

        if (bInfo.domains.size() > 0) {
            reply << "\n";
            reply << "node config info for " << bInfo.nodeConfig << ":\n";
            for(unsigned dm = 0 ; dm < bInfo.domains.size() ; ++dm) {
                reply << " domain id: " << bInfo.domains[dm].id << "\n";
                reply << "   images: " << bInfo.domains[dm].imageSet << "\n";
                reply << "   starting core: " << bInfo.domains[dm].startCore << "\n";
                reply << "   ending core: " << bInfo.domains[dm].endCore << "\n";
                reply << "   starting address: " << "0x" << std::hex << bInfo.domains[dm].startAddr << "\n";
                if ( bInfo.domains[dm].endAddr == -1 ) {
                    reply << "   ending address: " << std::dec << bInfo.domains[dm].endAddr << "\n";
                } else {
                    reply << "   ending address: " << "0x" << std::hex << bInfo.domains[dm].endAddr << "\n";
                }
                reply << "   Customization address: " << "0x" << std::hex << bInfo.domains[dm].custAddr << "\n";
                reply << "\n";
            }
        }

        reply << DONE;
        return;
    }
    else if(result == BGQDB::NOT_FOUND)
        reply << FAIL << "block not found: " << args[0] << DONE;
    else if(result == BGQDB::DB_ERROR)
        reply << FAIL << "DB query failed" << DONE;
    else if(result == BGQDB::INVALID_ID)
        reply << FAIL << "invalid id: " << args[0] << DONE;
    else
        reply << FAIL << "unexpected DB error" << DONE; /* returned status should only be one of the above */
}

void
MMCSCommand_get_block_info::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";Prints boot information, boot mode and status for the specified block."
          << DONE;
}

