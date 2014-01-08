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

#include "List.h"

#include <db/include/api/BGQDBlib.h>

#include <db/include/api/tableapi/DBConnectionPool.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

List*
List::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);
    commandAttributes.bgadminAuth(true);
    return new List("list", "list <db2table> [<value>]", commandAttributes);
}

void
List::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        common::ConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    if (args.size() < 1) {
        reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
        return;
    }

    BGQDB::TxObject tx(BGQDB::DBConnectionPool::Instance());
    if (!tx.getConnection()) {
        reply << mmcs_client::FAIL << "Unable to obtain database connection" << mmcs_client::DONE;
        return;
    }

    boost::shared_ptr<BGQDB::DBObj> n;
    string wc;

    if (args[0] == "bgqnode") {
        n.reset( new BGQDB::DBVNode );
        if (args.size() == 2)
            wc = "where location='"+args[1]+"'";
    } else if (args[0] == "bgqionode") {
        n.reset( new BGQDB::DBVIonode );
        if (args.size() == 2)
            wc = "where location='"+args[1]+"'";
    } else if (args[0] == "bgqnodecard") {
        n.reset( new BGQDB::DBVNodecard );
        if (args.size() == 2)
            wc = "where location='"+args[1]+"'";
    } else if (args[0] == "bgqmidplane") {
        n.reset( new BGQDB::DBVMidplane );
        if (args.size() == 2)
            wc = "where location='"+args[1]+"'";
    } else if (args[0] == "bgqiodrawer") {
        n.reset( new BGQDB::DBVIodrawer );
        if (args.size() == 2)
            wc = "where location='"+args[1]+"'";
    } else if (args[0] == "bgqmachine") {
        n.reset( new BGQDB::DBTMachine );
        if (args.size() == 2)
            wc = "where serialnumber='"+args[1]+"'";
    } else if (args[0] == "bgqblock") {
        n.reset( new BGQDB::DBTBlock );
        if ( args.size() == 2 && args[1].find_first_of('%') != std::string::npos ) {
            wc = "where blockid LIKE '" + args[1] + "'";
        } else if ( args.size() == 2 ) {
            wc = "where blockid = '" + args[1] + "'";
        }
    } else if (args[0] == "bgqproducttype") {
        n.reset( new BGQDB::DBTProducttype );
        if (args.size() == 2)
            wc = "where productid='"+args[1]+"'";
    } else if (args[0] == "bgqeventlog") {
        n.reset( new BGQDB::DBTEventlog );
        if (args.size() == 2)
            wc = "where block='"+args[1]+"' order by recid desc optimize for 10 rows" ;
        else
            wc = " order by recid desc optimize for 10 rows";
    } else if (args[0] == "bgqjob") {
        n.reset( new BGQDB::DBTJob );
        if (args.size() == 2)
            wc = "where id='"+args[1]+"'";
    } else if (args[0] == "bgqjob_history") {
        n.reset( new BGQDB::DBTJob_history );
        if (args.size() == 2)
            wc = "where id='"+args[1]+"'";
    } else if (args[0] == "bgqnodeconfig") {
        n.reset( new BGQDB::DBTNodeconfig );
        if (args.size() == 2)
            wc = "where nodeconfig='"+args[1]+"'";
    } else  {
        reply << mmcs_client::FAIL
            << "Invalid table\n"
            << "Valid tables and optional parameters are:\n"
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
            << mmcs_client::DONE;
        return;
    }

    SQLRETURN rc;
    SET_ALL_COLUMN((*n));
    if (args[0] == "bgqeventlog") {
        int rows = 10;
        tx.query(n.get(),wc.c_str()) ;
        rc = tx.fetch(n.get());

        if (rc == SQL_NO_DATA_FOUND) {
            reply << mmcs_client::FAIL << "No data found "  << mmcs_client::DONE;
            return;
        }
        reply << mmcs_client::OK << "Most recent " << rows << " RAS events ";

        if (args.size() == 1)
            reply << '\n';
        else
            reply << "for block " << args[1] << '\n';

        while (rc == SQL_SUCCESS && rows>0) {
            reply
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_event_time << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_severity  << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_block << " "
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_location << '\n'
                << boost::static_pointer_cast<BGQDB::DBTEventlog>(n)->_message << '\n';
            rc = tx.fetch(n.get());
            rows--;
        }
        reply << mmcs_client::DONE;
    }  else {
        if (args[0] == "bgqblock") {
            bitset<BGQDB::DBTBlock::N_BLOCK_ATTR> cmap;
            cmap.set();
            cmap.reset( boost::static_pointer_cast<BGQDB::DBTBlock>(n)->SECURITYKEY );
            n->_columns = cmap.to_ulong();
        }

        BGQDB::DBTDomainmap dm;
        SET_ALL_COLUMN(dm);
        BGQDB::TxObject tx2(BGQDB::DBConnectionPool::Instance());

        args.size() == 1 ? tx.query(n.get()) : tx.query(n.get(), wc.c_str());
        rc = tx.fetch(n.get());
        if (rc == SQL_NO_DATA_FOUND) {
            reply << mmcs_client::FAIL << "No data found" << mmcs_client::DONE;
            return;
        }

        if (rc != SQL_SUCCESS) {
            reply << mmcs_client::FAIL << "Invalid argument" << mmcs_client::DONE;
        } else {
            reply << mmcs_client::OK;
            while (rc == SQL_SUCCESS) {
                n->dump(reply);

                if (args[0] == "bgqnodeconfig") {
                    if (args.size() == 1)
                        wc = "where nodeconfig='"+string(boost::static_pointer_cast<BGQDB::DBTNodeconfig>(n)->_nodeconfig)+"'";
                    tx2.query(&dm,wc.c_str());
                    rc = tx2.fetch(&dm);
                    while (rc == SQL_SUCCESS) {
                        dm.dump(reply);
                        rc = tx2.fetch(&dm);
                    }
                    tx2.close(&dm);
                }

                rc = tx.fetch(n.get());
            }
            tx.close(n.get());
            reply << mmcs_client::DONE;
        }
    }
}

void
List::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Prints the contents of <db2table>."
          << ";Optional <value> specifies contents of a field within that table."
          << ";Following are the only valid table / field combinations that can be used. "
          << ";bgqnode / location "
          << ";bgqionode / location "
          << ";bgqnodecard / location "
          << ";bgqmidplane / location "
          << ";bgqiodrawer / location "
          << ";bgqmachine / serialnumber "
          << ";bgqblock / blockid (use \% for wildcard)"
          << ";bgqproducttype / productid "
          << ";bgqeventlog / block"
          << ";bgqjob / jobid"
          << ";bgqjob_history / jobid"
          << ";bgqnodeconfig / configname"
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
