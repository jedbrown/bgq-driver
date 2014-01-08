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


#include "CreateNodeConfig.h"

#include <db/include/api/BGQDBlib.h>


using namespace std;


namespace mmcs {
namespace server {
namespace command {


CreateNodeConfig*
CreateNodeConfig::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(common::ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new CreateNodeConfig("create_node_config", "create_node_config <configname> [<nodeoptions>] <domaininfo>", commandAttributes);
}


void
CreateNodeConfig::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
CreateNodeConfig::execute(deque<string> args,
                       mmcs_client::CommandReply& reply,
                       common::ConsoleController* pController,
                       BlockControllerTarget* pTarget,
                       std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;
    result = BGQDB::createNodeConfig(args);

    switch (result) {
    case BGQDB::OK:
    reply << mmcs_client::OK << "node config created" << mmcs_client::DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << mmcs_client::FAIL << "node config " << args[0] << " already exists" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << mmcs_client::FAIL << "node config name " << args[0] << " invalid or too big" << mmcs_client::DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << mmcs_client::FAIL << "unable to connect to database" << mmcs_client::DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << mmcs_client::FAIL << "database failure or duplicate name" << mmcs_client::DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << mmcs_client::FAIL << "invalid arguments provided, or images missing" << mmcs_client::DONE;
    break;
    case BGQDB::FAILED:
    reply << mmcs_client::FAIL << "invalid node configuration" << mmcs_client::DONE;
    break;
    default:
    reply << mmcs_client::FAIL << "unexpected return code from BGQDB::createNodeConfig : " << result << mmcs_client::DONE;
    break;
    }
}

void
CreateNodeConfig::help(deque<string> args,
                    mmcs_client::CommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << mmcs_client::OK << description()
          << ";Create a node configuration for booting a block."
          << ";Optionally specify <nodeoptions> in the format:  nodeoptions={option option}"
          << ";For <domaininfo> specify one or more domain specifications, where each is of the format:"
          << ";  domain={cores=startcore-endcore memory=startaddr-endaddr images=imagename,imagename,..  id=domainid custaddr=addr options=option option} "
          << ";Multiple domains can be specified, but they cannot overlap cores or memory."
          << ";Images are required."
          << ";Memory addresses must be specified in hexadecimal format without 0x prefix, the ending address can be -1"
          << ";  to use all available memory."
          << ";Cores can range from 0 to 16."
          << ";Cores and memory can be omitted for a single-domain node config, and will default to all cores and all memory."
          << ";Memory can also be omitted for a multi-domain node config, and will default to 1GB per core."
          << ";Optionally specify custaddr to provide an address for the node-specific customizations, which come from the database."
      << mmcs_client::DONE;
}


} } } // namespace mmcs::server::command
