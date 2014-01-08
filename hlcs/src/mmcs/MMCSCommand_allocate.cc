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
 * \file MMCSCommand_allocate.cc
 */

#include "MMCSCommand_allocate.h"
#include "MMCSCommandReply.h"
#include "DBConsoleController.h"
#include "DBBlockController.h"
#include "MMCSProperties.h"
#include "MMCSCommandProcessor.h"

#include <db/include/api/tableapi/TxObject.h>
#include <db/include/api/BGQDBlib.h>

#include <utility/include/Log.h>


LOG_DECLARE_FILE( "mmcs" );


MMCSCommand_allocate*
MMCSCommand_allocate::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);

    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_allocate("allocate", "allocate <blockId>", commandAttributes);
}


void
MMCSCommand_allocate::execute(deque<string> args,
                  MMCSCommandReply& reply,
                  DBConsoleController* pController,
                  BlockControllerTarget* pTarget)
{
    std::vector<std::string> vn;
    vn.push_back(args[0]);
    execute(args, reply, pController, pTarget, &vn);
    return;
}

void
MMCSCommand_allocate::execute(deque<string> args,
                  MMCSCommandReply& reply,
                  DBConsoleController* pController,
                  BlockControllerTarget* pTarget,
                  std::vector<std::string>* validnames)
{
    BGQDB::STATUS result;           // getBlockStatus return code
    BGQDB::BLOCK_STATUS bState;     // getBlockStatus return state

    if (validnames->size() != 1)
    {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }
    const string blockName = validnames->at(0);    // save the block name

    if (((result = BGQDB::getBlockStatus(blockName, bState)) == BGQDB::OK)
            && (bState != BGQDB::FREE && bState != BGQDB::ALLOCATED)) {
        reply << FAIL << "Block is not free" << DONE;
        return;
    }

    if(!DBConsoleController::setAllocating(blockName)) {
        reply << FAIL << "Block is being allocated in another thread" << DONE;
        return;
    }

    //  select the block
    pController->selectBlock(args, reply, false);
    if (reply.getStatus() != 0) {
        if(reply.str() == "args?") {
            reply << FAIL << "args? " << usage << DONE;
        }
        DBConsoleController::doneAllocating(blockName);
        return;
    }

    // allocate the block
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->allocateBlock(args, reply);
    if (reply.getStatus() != 0)
    {
        if(reply.str() == "args?")
            reply << FAIL << "args? " << usage << DONE;
        if (pController->getBlockHelper() != 0)
            pController->deselectBlock();
        DBConsoleController::doneAllocating(blockName);

        return;
    }

    // remove the  the block name from the arguments
    args.pop_front();

    // boot the block
    pBlock->boot_block(args, reply);
    if (reply.getStatus() != 0) {
        result = BGQDB::getBlockStatus(blockName, bState);
        if ( result == BGQDB::OK && bState != BGQDB::TERMINATING ) {
            // Return the block to terminating state
            if ((result = pBlock->setBlockStatus(blockName, BGQDB::TERMINATING)) != BGQDB::OK) {
                LOG_ERROR_MSG("setBlockStatus(" << blockName << ", TERMINATING) failed, result=" << result);
            } else {
                LOG_INFO_MSG("Set block " << blockName << " to TERMINATING");
            }
        } else {
            LOG_ERROR_MSG( "could not get block status: " << result );
        }

        if (pController->getBlockHelper() != 0) {
            pBlock->getBase()->setDisconnecting(true, "boot failed");
            pController->deselectBlock();
            DBConsoleController::doneAllocating(blockName);
        }

        return;
    }

    DBConsoleController::doneAllocating(blockName);

    // wait for the boot to complete
    deque<string> waitBoot_args; // empty argument list
    pBlock->waitBoot(waitBoot_args, reply);
    if(reply.str() == "args?") {
        reply << FAIL << "args? " << usage << DONE;
        return;
    }
}

void
MMCSCommand_allocate::help(deque<string> args,
               MMCSCommandReply& reply)
{
    reply << OK << description()
          << ";For specified <blockId>, performs select_block, allocate_block, boot_block and wait_boot."
          << ";<blockId> identifies a block in the BGQBLOCK table."
          << ";This command can be used to initialize a block in free status."
          << DONE;
}

MMCSCommand_allocate_block*
MMCSCommand_allocate_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(false);            // does not require a BlockController object
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_allocate_block("allocate_block", "allocate_block <blockId>", commandAttributes);
}

void
MMCSCommand_allocate_block::execute(deque<string> args,
                  MMCSCommandReply& reply,
                  DBConsoleController* pController,
                  BlockControllerTarget* pTarget,
                  std::vector<std::string>* validnames)
{
    execute(args, reply, pController, pTarget);
}

void
MMCSCommand_allocate_block::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    DBConsoleController* pController,
                    BlockControllerTarget* pTarget)
{
    BGQDB::STATUS result;    // getBlockStatus return code
    BGQDB::BLOCK_STATUS bState;    // getBlockStatus return state

    if (((result = BGQDB::getBlockStatus(args[0], bState)) == BGQDB::OK)
    && (bState != BGQDB::FREE)) {
      reply << FAIL << "Block is not free" << DONE;
      return;
    }

    if(!DBConsoleController::setAllocating(args[0])) {
      reply << FAIL << "Block is being allocated or freed in another thread" << DONE;
      return;
    }

    //  select the block
    pController->selectBlock(args, reply, false);
    if (reply.getStatus() != 0) {
      DBConsoleController::doneAllocating(args[0]);
      return;
    }

    // allocate the block
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->allocateBlock(args, reply);
    if (reply.getStatus() != 0)
    pController->deselectBlock();
    DBConsoleController::doneAllocating(args[0]);
}

void
MMCSCommand_allocate_block::help(deque<string> args,
                 MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";For specified <blockId>, marks block as allocated but does not boot the block."
      << ";options:"
      << ";  no_connect - don't connect to block hardware"
      << ";  pgood - reset pgood on block hardware"
      << ";  diags - enable block to be created when components are in Service status"
      << ";  no_check - enable block to be created when nodes are in Error status"
      << ";  shared - enable nodecard resources to be shared between blocks. Implies no_connect."
      << ";  svchost_options=<svc_host_configuration_file>"
      << DONE;
}

MMCSServerCommand_boot_block*
MMCSServerCommand_boot_block::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(true);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    MMCSCommandAttributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    commandAttributes.addAuthPair(blockread);
    return new MMCSServerCommand_boot_block("boot_block", "boot_block [update] [<options>]", commandAttributes);
}

void
MMCSServerCommand_boot_block::execute(std::deque<std::string> args,
                      MMCSCommandReply& reply,
                      DBConsoleController* pController,
                      BlockControllerTarget* pTarget)
{
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->boot_block(args, reply);
}


void
MMCSServerCommand_boot_block::help(deque<string> args,
                   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Initialize, load, and start the block's resources"
      << ";update                  - use a combination of the boot_block options and block definition to boot the block"
      << ";options:"
      << ";   uloader   = <path>   - microloader"
      << ";   domain={cores=startcore-endcore memory=startaddr-endaddr images=imagename,imagename,..  id=domainid options=domainoptions custaddr=addr}"
      << ";   boot options         - any other options are passed to the machineController on the boot command"
      << ";   steps=               - specify a comma separated collection of boot steps to perform"
      << ";     steps:"
      << ";       enableNodes"
      << ";       initDevbus"
      << ";       startTraining"
      << ";       enableLinkChips"
      << ";       trainLinkChips"
      << ";       trainTorus"
      << ";       installFirmware"
      << ";       installPersonalities"
      << ";       startCores"
      << ";       verifyMailbox"
      << ";       monitorMailbox"
      << ";       verifyMailboxReady"
      << ";       installKernelImages"
      << ";       configureDomains"
      << ";       launchKernels"
      << ";       verifyKernelReady"
      << ";       noop"
      << ";       cardNoop"
      << ";If no options are specified, the uloader, images, and boot options specified in the block definition are used"
      << ";If any options are specified, and 'update' is not specified, only the uloader, images, and boot options"
      << ";specified on the boot_block request are used"
      << ";"
      << ";If 'update' is specified, then the uloader, images, and boot options from the boot_block command will be combined"
      << ";with the block definition. Any boot options will be added to those from the block definition. Any uloader or images"
      << ";specified on the boot_block command will replace the specification from the block definition, but any uloader or"
      << ";images not specified on the boot_block command will be taken from the block specification."
      << ";"
      << ";If 'steps=' is specified, boot_block will execute the comma separated list of boot steps.  Subsequent calls to boot_block"
      << ";with additional steps defined, will execute those."
      << DONE;
}

MMCSCommand_wait_boot*
MMCSCommand_wait_boot::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(true);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(USER);
    MMCSCommandAttributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(blockread);
    return new MMCSCommand_wait_boot("wait_boot", "wait_boot [<minutes>]", commandAttributes);
}

void
MMCSCommand_wait_boot::execute(std::deque<std::string> args,
                   MMCSCommandReply& reply,
                   DBConsoleController* pController,
                   BlockControllerTarget* pTarget)
{
    DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper());    // get selected block
    pBlock->waitBoot(args, reply, true);
    if(reply.str() == "args?") {
      reply << FAIL << "args? " << usage << DONE;
      return;
    }
}


void
MMCSCommand_wait_boot::help(deque<string> args,
                MMCSCommandReply& reply)
{
    reply << OK << description()
        << ";Wait for the selected block to complete booting."
        << ";<minutes> specifies the maximum time to wait, in minutes. The default is 20 minutes."
        << ";The command does not complete until the block is fully initialized to the point"
        << ";that kernels are ready for job submission or the wait time has expired."
        << DONE;
}
