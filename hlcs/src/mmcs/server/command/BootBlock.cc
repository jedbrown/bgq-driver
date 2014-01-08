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

#include "BootBlock.h"

#include "../BlockControllerBase.h"
#include "../DBBlockController.h"
#include "../DBConsoleController.h"

#include <utility/include/Log.h>

using namespace std;

namespace mmcs {
namespace server {
namespace command {

BootBlock*
BootBlock::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);            // does require a BlockController object
    commandAttributes.requiresConnection(true);       // does require  mc_server connections
    commandAttributes.requiresTarget(false);          // does not require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.helpCategory(common::USER);
    Attributes::AuthPair blockread(hlcs::security::Object::Block, hlcs::security::Action::Read);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    commandAttributes.addAuthPair(blockread);
    return new BootBlock("boot_block", "boot_block [update] [<options>]", commandAttributes);
}

void
BootBlock::execute(
        std::deque<std::string> args,
        mmcs_client::CommandReply& reply,
        DBConsoleController* pController,
        BlockControllerTarget* pTarget
)
{
    const DBBlockPtr pBlock = boost::dynamic_pointer_cast<DBBlockController>(pController->getBlockHelper()); // get the selected BlockController
    pBlock->boot_block(args, reply);
}

void
BootBlock::help(
        deque<string> args,
        mmcs_client::CommandReply& reply
)
{
    reply << mmcs_client::OK << description()
          << ";Initialize, load, and start the block's resources"
          << ";update                  - use a combination of the boot_block options and block definition to boot the block"
          << ";options:"
          << ";   uloader = <path>     - microloader"
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
          << ";If 'steps=' is specified, boot_block will execute the comma separated list of boot steps. Subsequent calls to boot_block"
          << ";with additional steps defined, will execute those."
          << mmcs_client::DONE;
}

} } } // namespace mmcs::server::command
