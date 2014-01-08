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
 * \file MMCSServerCommand_block.cc
 */

#include "ConsoleController.h"
#include "MMCSServerCommand_block.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"

#include <db/include/api/BGQDBlib.h>
#include <db/include/api/genblock.h>
#include <db/include/api/tableapi/gensrc/bgqtableapi.h>

#include <boost/lexical_cast.hpp>

LOG_DECLARE_FILE( "mmcs" );

MMCSServerCommand_gen_io_block*
MMCSServerCommand_gen_io_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_io_block("gen_io_block", "gen_io_block <blockid> <location> <ionodes>", commandAttributes);
}

void
MMCSServerCommand_gen_io_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_io_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget,
                       std::vector<std::string>* validnames)
{
    int nodes = 0;
    try {
        nodes = boost::lexical_cast<int>(args[2]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << FAIL << "bad node count: " << e.what() << DONE;
        return;
    }
    if ( nodes <= 0 ) {
        reply << FAIL << "invalid node count: " << args[2] << DONE;
        return;
    }

    BGQDB::STATUS result;
    result = BGQDB::genIOBlock(args[0], args[1], nodes, pController->getUser().getUser());

    switch (result) {
    case BGQDB::OK:
    reply << OK << "block created with default images, use set_block_info to change images" << DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << FAIL << "blockid " << args[0] << " already exists" << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "blockid " << args[0] << " too big or contains invalid characters" << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure" << DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << FAIL << "block configuration data not found" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid block configuration" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::genIOBlock : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_gen_io_block::help(deque<string> args,
                    MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate a block for a set of IO nodes."
      << ";For <location> specify an IO node, i.e. Qxx-Ix-Jxx or Rxx-Ix-Jxx, or an IO drawer, i.e. Qxx-Ix or Rxx-Ix."
      << ";The number of nodes must be a multiple of 8 when specifying an IO drawer."
      << ";The number of nodes must 1, 2, or 4 when specifying an IO node."
      << ";For 2 nodes, the node location must be J00, J02, J04, or J06."
      << ";For 4 nodes, the node location must be J00 or J04."
      << DONE;
}

MMCSServerCommand_create_node_config*
MMCSServerCommand_create_node_config::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    commandAttributes.bgadminAuth(true);
    return new MMCSServerCommand_create_node_config("create_node_config", "create_node_config <configname> [<nodeoptions>] <domaininfo>", commandAttributes);
}


void
MMCSServerCommand_create_node_config::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_create_node_config::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget,
                       std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;
    result = BGQDB::createNodeConfig(args);

    switch (result) {
    case BGQDB::OK:
    reply << OK << "node config created" << DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << FAIL << "node config " << args[0] << " already exists" << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "node config name " << args[0] << " invalid or too big" << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure or duplicate name" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided, or images missing" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid node configuration" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::createNodeConfig : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_create_node_config::help(deque<string> args,
                    MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
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
      << DONE;
}

MMCSServerCommand_gen_midplane_block*
MMCSServerCommand_gen_midplane_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_midplane_block("gen_midplane_block", "gen_midplane_block <blockid> <corner> <asize> <bsize> <csize> <dsize> [aPT bPT cPT dPT]", commandAttributes);
}

void
MMCSServerCommand_gen_midplane_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_midplane_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget,
                       std::vector<std::string>* validnames)
{
    if((args.size() != 6) && (args.size() != 10))
    {
    reply << FAIL << "args? " << usage <<  DONE;
    return;
    }

    BGQDB::STATUS result;

    int asize, bsize, csize, dsize;
    try {
        asize = boost::lexical_cast<int>(args[2]);
        bsize = boost::lexical_cast<int>(args[3]);
        csize = boost::lexical_cast<int>(args[4]);
        dsize = boost::lexical_cast<int>(args[5]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << FAIL << e.what() << DONE;
        return;
    }

    if ( asize <= 0 ) {
        reply << FAIL << "invalid a dimension size: " << args[2] << DONE;
        return;
    }
    if ( bsize <= 0 ) {
        reply << FAIL << "invalid b dimension size: " << args[3] << DONE;
        return;
    }
    if ( csize <= 0 ) {
        reply << FAIL << "invalid c dimension size: " << args[4] << DONE;
        return;
    }
    if ( dsize <= 0 ) {
        reply << FAIL << "invalid d dimension size: " << args[5] << DONE;
        return;
    }

    if (args.size() == 6) {
      result = BGQDB::genMidplaneBlock(args[0], args[1], asize, bsize, csize, dsize, pController->getUser().getUser() );
    } else {
      result = BGQDB::genMidplaneBlock(args[0], args[1], asize, bsize, csize, dsize, pController->getUser().getUser(), args[6], args[7], args[8], args[9] );
    }

    switch (result) {
    case BGQDB::OK:
    reply << OK << "block created with default images, use set_block_info to change images" << DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << FAIL << "blockid " << args[0] << " already exists" << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "blockid " << args[0] << " too big or contains invalid characters" << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure" << DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << FAIL << "block configuration data not found" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid block configuration" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::genBPBlock : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_gen_midplane_block::help(deque<string> args,
                    MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate a block for a set of midplanes."
      << ";For <corner> specify midplane by location, i.e. Rxx-Mx."
      << ";The size is provided in terms of number of midplanes in A, B, C and D dimensions."
      << ";The <corner> will be in the 0,0,0,0 position of the midplanes that make up the block."
      << ";Except for the <corner>, the midplanes included in the generated block"
      << ";depend on the A,B,C,D cabling of your machine."
            << ";;aPT,bPT,cPT,dPT are optional arguments to specify the use of passthrough and are"
          << ";expected to be a string of 1's (include) and 0's (passthrough)."
      << DONE;
}

MMCSServerCommand_gen_block*
MMCSServerCommand_gen_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_block("gen_block", "gen_block <blockid> <midplane>", commandAttributes);
}


void
MMCSServerCommand_gen_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_block::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    if(args.size() != 2)
    {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    BGQDB::STATUS result;
    result = BGQDB::genBlock(args[0], args[1], pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
    reply << OK << "block created with default images, use set_block_info to change images" << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "blockid " << args[0] << " too big or contains invalid characters" << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure or duplicate block name" << DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << FAIL << "midplane not found" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid block configuration" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided" << DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << FAIL << "blockid " << args[0] << " already exists" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::genBlock : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_gen_block::help(deque<string> args,
                  MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate block for a midplane.  Specify <midplane> by location, i.e. Rxx-Mx."
      << DONE;
}

MMCSServerCommand_gen_blocks*
MMCSServerCommand_gen_blocks::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_blocks("gen_blocks", "gen_blocks [<blockidprefix>]", commandAttributes);
}

void
MMCSServerCommand_gen_blocks::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_blocks::execute(deque<string> args,
                      MMCSCommandReply& reply,
                      ConsoleController* pController,
                      BlockControllerTarget* pTarget,
                      std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    if(args.size() == 1)
        result = BGQDB::genBlocks(pController->getUser().getUser(), args[0]);
    else
        result = BGQDB::genBlocks(pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
        reply << OK << "blocks created with default images, use set_block_info to change images" << DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << FAIL << "blockidprefix " << args[0] << " too big or contains invalid characters" << DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << FAIL << "unable to connect to database" << DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << FAIL << "database failure or duplicate block name" << DONE;
        break;
    case BGQDB::FAILED:
        reply << FAIL << "invalid block configuration" << DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << FAIL << "invalid arguments provided" << DONE;
        break;
    case BGQDB::DUPLICATE:
        reply << FAIL << "duplicate block name" << DONE;
        break;
    default:
        reply << FAIL << "unexpected return code from BGQDB::genBlocks : " << result << DONE;
        break;
    }
}

void
MMCSServerCommand_gen_blocks::help(deque<string> args,
                   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate a block for each midplane on a machine."
      << ";The <blockidprefix> is optional."
      << ";If omitted, each block name will be the position of the midplane, i.e. Rxx-Mx."
      << DONE;
}

MMCSServerCommand_gen_full_block*
MMCSServerCommand_gen_full_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_full_block("gen_full_block", "gen_full_block <blockid>", commandAttributes);
}

void
MMCSServerCommand_gen_full_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_full_block::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    result = BGQDB::genFullBlock(args[0], pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
    reply << OK << "block created with default images, use set_block_info to change images" << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "invalid block id " << args[0] << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure or duplicate block name" << DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << FAIL << "block " << args[0] << " not found" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid block configuration" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided" << DONE;
    break;
    case BGQDB::DUPLICATE:
    reply << FAIL << "blockid " << args[0] << " already exists" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::genFullBlock : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_gen_full_block::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate a block for the entire machine."
      << DONE;
}

MMCSServerCommand_gen_small_block*
MMCSServerCommand_gen_small_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    return new MMCSServerCommand_gen_small_block("gen_small_block", "gen_small_block <blockid> <midplane> <cnodes> <nodecard>", commandAttributes);
}

void
MMCSServerCommand_gen_small_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_gen_small_block::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    int nodes = 0;
    try {
        nodes = boost::lexical_cast<int>(args[2]);
    } catch (const boost::bad_lexical_cast& e) {
        reply << FAIL << e.what() << DONE;
        return;
    }
    if ( nodes <= 0 ) {
        reply << FAIL << "invalid node count: " << args[2] << DONE;
        return;
    }

    result = BGQDB::genSmallBlock(args[0], args[1], nodes, args[3], pController->getUser().getUser());
    switch (result) {
    case BGQDB::OK:
        reply << OK << "block created with default images, use set_block_info to change images" << DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << FAIL << "invalid block id " << args[0] << DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << FAIL << "unable to connect to database" << DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << FAIL << "database failure or duplicate block name" << DONE;
        break;
    case BGQDB::NOT_FOUND:
        reply << FAIL << "midplane not found" << DONE;
        break;
    case BGQDB::FAILED:
        reply << FAIL << "invalid block configuration" << DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << FAIL << "invalid arguments provided" << DONE;
        break;
    case BGQDB::DUPLICATE:
        reply << FAIL << "blockid " << args[0] << " already exists" << DONE;
        break;
    default:
        reply << FAIL << "unexpected return code from BGQDB::genSmallBlock : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_gen_small_block::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Generate a sub-midplane block."
      << ";Specify <midplane> by location, i.e. Rxx-Mx."
      << ";The <cnodes> is the total number of compute nodes and must be 32, 64, 128, or 256."
         << ";The <nodecard> is the location of the compute nodes for the block, i.e. N00, N01, etc."
       << DONE;
}


MMCSServerCommand_set_block_info*
MMCSServerCommand_set_block_info::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockupdate(hlcs::security::Object::Block, hlcs::security::Action::Update);
    commandAttributes.addAuthPair(blockupdate);
    return new MMCSServerCommand_set_block_info("set_block_info", "set_block_info <blockid> <uloader> <nodecfg> [<bootoptions>]", commandAttributes);
}

void
MMCSServerCommand_set_block_info::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_set_block_info::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;
    BGQDB::BlockInfo bInfo;

    if (args.size() < 3) {
        reply << FAIL << "args? " << usage <<  DONE;
        return;
    }

    if (args[1].size() >= BGQDB::BlockInfo::MicroLoaderSize) {
        reply << FAIL << "microloader image name too long" << DONE;
        return;
    }

    if (args[2].size() > sizeof(bInfo.nodeConfig)) {
        reply << FAIL << "node config name too long" << DONE;
        return;
    }

    strcpy(bInfo.uloaderImg, args[1].c_str());
    strcpy(bInfo.nodeConfig, args[2].c_str());

    if (args.size() > 3) {
        if (args[3].size() > BGQDB::BlockInfo::OptionsSize) {
            reply << FAIL << "boot options too long" << DONE;
            return;
        }
        strcpy(bInfo.bootOptions, args[3].c_str());
    } else {
        strcpy(bInfo.bootOptions, " ");
    }

    if (args.size() == 5) {
        if (args[4].length() >= sizeof(bInfo.options)) {
            reply << FAIL << "options too long" << DONE;
            return;
        }
        strcpy(bInfo.options, args[4].c_str());

    } else if (args.size() < 5) {
        strcpy(bInfo.options, " ");
    } else {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }

    result = BGQDB::setBlockInfo(args[0], bInfo);
    switch (result) {
    case BGQDB::OK:
        reply << OK << DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << FAIL << "invalid block id " << args[0] << DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << FAIL << "invalid argument" << DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << FAIL << "unable to connect to database" << DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << FAIL << "database failure or invalid node config specified" << DONE;
        break;
    case BGQDB::NOT_FOUND:
        reply << FAIL << "block " << args[0] << " not found or not in valid state" << DONE;
        break;
    case BGQDB::FAILED:
        reply << FAIL << "invalid block state" << DONE;
        break;
    default:
        reply << FAIL << "unexpected return code from BGQDB::setBlockBootInfo : " << result << DONE;
        break;
    }
}

void
MMCSServerCommand_set_block_info::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
          << ";Set boot details for a block."
           << DONE;
}

MMCSServerCommand_set_boot_options*
MMCSServerCommand_set_boot_options::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockupdate(hlcs::security::Object::Block, hlcs::security::Action::Update);
    commandAttributes.addAuthPair(blockupdate);
    return new MMCSServerCommand_set_boot_options("set_boot_options", "set_boot_options <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoptions|NULL>", commandAttributes);
}

void
MMCSServerCommand_set_boot_options::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_set_boot_options::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    if (args[1].size() > BGQDB::BlockInfo::OptionsSize) {
        reply << FAIL << "boot options too long" << DONE;
        return;
    }

    result = BGQDB::setBootOptions(args[0],  args[1]);
    switch (result) {
    case BGQDB::OK:
        reply << OK << DONE;
        break;
    case BGQDB::INVALID_ID:
        reply << FAIL << "invalid block id " << args[0] << DONE;
        break;
    case BGQDB::CONNECTION_ERROR:
        reply << FAIL << "unable to connect to database" << DONE;
        break;
    case BGQDB::DB_ERROR:
        reply << FAIL << "database failure" << DONE;
        break;
    case BGQDB::NOT_FOUND:
        reply << FAIL << "block " << args[0] << " not found or not in valid state" << DONE;
        break;
    case BGQDB::INVALID_ARG:
        reply << FAIL << "boot options too long" << DONE;
        break;
    case BGQDB::FAILED:
        reply << FAIL << "invalid block state" << DONE;
        break;
    default:
        reply << FAIL << "unexpected return code from BGQDB::setBootOptions : " << result << DONE;
        break;
    }
}

void
MMCSServerCommand_set_boot_options::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
  reply << OK << description()
    << ";Set boot options for a block, all io blocks, all compute blocks, or all blocks."
    << ";To set multiple boot options, separate by commas, with no spaces."
    << DONE;
}

MMCSServerCommand_add_boot_option*
MMCSServerCommand_add_boot_option::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockupdate(hlcs::security::Object::Block, hlcs::security::Action::Update);
    commandAttributes.addAuthPair(blockupdate);
    return new MMCSServerCommand_add_boot_option("add_boot_option", "add_boot_option <blockid|*ALL|*ALLIO|*ALLCOMPUTE> <bootoption>", commandAttributes);
}

void
MMCSServerCommand_add_boot_option::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_add_boot_option::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    if(args.size() != 2)
      {
    reply << FAIL << "args? " << usage <<  DONE;
    return;
      }

    if(args[1].size() > BGQDB::BlockInfo::OptionsSize)
      {
    reply << FAIL << "boot options too long" << DONE;
    return;
      }

    result = BGQDB::addBootOption(args[0],  args[1]);
    switch (result) {
    case BGQDB::OK:
    reply << OK << DONE;
    break;
    case BGQDB::INVALID_ID:
    reply << FAIL << "invalid block id " << args[0] << DONE;
    break;
    case BGQDB::CONNECTION_ERROR:
    reply << FAIL << "unable to connect to database" << DONE;
    break;
    case BGQDB::DB_ERROR:
    reply << FAIL << "database failure" << DONE;
    break;
    case BGQDB::NOT_FOUND:
    reply << FAIL << "block " << args[0] << " not found or not in valid state" << DONE;
    break;
    case BGQDB::FAILED:
    reply << FAIL << "invalid block state" << DONE;
    break;
    case BGQDB::INVALID_ARG:
    reply << FAIL << "invalid arguments provided" << DONE;
    break;
    default:
    reply << FAIL << "unexpected return code from BGQDB::addBootOption : " << result << DONE;
    break;
    }
}

void
MMCSServerCommand_add_boot_option::help(deque<string> args,
                      MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
  reply << OK << description()
    << ";Add boot option for a block, all io blocks, all compute blocks, or all blocks."
    << ";To add multiple boot options, separate by commas, with no spaces."
    << DONE;
}

MMCSServerCommand_copy_block*
MMCSServerCommand_copy_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsConsoleCommand(false);
    commandAttributes.helpCategory(ADMIN);             // 'help admin'  will include this command's summary
    MMCSCommandAttributes::AuthPair blockcreate(hlcs::security::Object::Block, hlcs::security::Action::Create);
    commandAttributes.addAuthPair(blockcreate);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSServerCommand_copy_block("copy_block", "copy_block <existingblockid> <newblockid>", commandAttributes);
}

std::vector<std::string>
MMCSServerCommand_copy_block::getBlockObjects(std::deque<std::string>& cmdString, DBConsoleController* pController) {
    std::vector<std::string> blocks;
    blocks.push_back(cmdString[0]);
    blocks.push_back(cmdString[1]);
    return blocks;
}

bool
MMCSServerCommand_copy_block::doSpecialAuths(std::vector<std::string>& blocks,
                                             boost::shared_ptr<hlcs::security::Enforcer>& enforcer,
                                             MMCSCommandProcessorStatus::procstat& procstat,
                                             bgq::utility::UserId& user) {

    // Assume success!
    procstat = MMCSCommandProcessorStatus::CMD_EXECUTED;

    try {
        if(blocks.size() != 2)
            procstat = MMCSCommandProcessorStatus::CMD_INVALID;
        hlcs::security::Object existing_blockobj(hlcs::security::Object::Block, blocks[0]);
        if(enforcer->validate(existing_blockobj, hlcs::security::Action::Read, user) == false)
            procstat = MMCSCommandProcessorStatus::CMD_INVALID;
        hlcs::security::Object new_blockobj(hlcs::security::Object::Block, blocks[1]);
        if(enforcer->validate(new_blockobj, hlcs::security::Action::Create, user) == false)
            procstat =  MMCSCommandProcessorStatus::CMD_INVALID;
    } catch ( const std::exception& e ) {
        LOG_WARN_MSG( e.what() );
        procstat =  MMCSCommandProcessorStatus::CMD_INVALID;
    }

    if(procstat == MMCSCommandProcessorStatus::CMD_INVALID) {
        blocks.clear(); 
    }
    // Always return true because we're running here.
    return true;
}

void
MMCSServerCommand_copy_block::execute(deque<string> args,
                       MMCSCommandReply& reply,
                       ConsoleController* pController,
                       BlockControllerTarget* pTarget) {
    std::vector<std::string>* validnames = 0;
    return execute(args, reply, pController, pTarget, validnames);
}

void
MMCSServerCommand_copy_block::execute(deque<string> args,
                     MMCSCommandReply& reply,
                     ConsoleController* pController,
                     BlockControllerTarget* pTarget,
                     std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;

    if(args.size() != 2)
      {
    reply << FAIL << "args? " << usage <<  DONE;
    return; }

    result = BGQDB::copyBlock(args[0],  args[1], pController->getUser().getUser());
    switch (result) {
        case BGQDB::OK:
            reply << OK << DONE;
            break;
        case BGQDB::INVALID_ID:
            reply << FAIL << "invalid block id " << args[1] << " or block already exists" << DONE;
            break;
        case BGQDB::CONNECTION_ERROR:
            reply << FAIL << "unable to connect to database" << DONE;
            break;
        case BGQDB::DB_ERROR:
            reply << FAIL << "database failure or block id " << args[1] << " already exists"<< DONE;
            break;
        case BGQDB::NOT_FOUND:
            reply << FAIL << "block " << args[0] << " not found" << DONE;
            break;
        case BGQDB::INVALID_ARG:
            reply << FAIL << "invalid arguments provided" << DONE;
            break;
        default:
            reply << FAIL << "unexpected return code from BGQDB::copyBlock : " << result << DONE;
            break;
    }
}

void
MMCSServerCommand_copy_block::help(std::deque<std::string> args,
        MMCSCommandReply& reply)
{
    BGQDB::DBTBlock db;
    reply << OK << description()
        << ";Copy an existing block, along with all of its boot information."
        << ";The maximum size of the new block ID is " << sizeof(BGQDB::DBTBlock()._blockid) - 1 << " characters"
        << DONE;
}
