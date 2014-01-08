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
 * \file MMCSCommand_read_kernel_status.cc
 * \brief MMCS commands for diagnostics.
 * Various commands for reading/writing SRAM, TDRs, and kernel status
 */

#include "MMCSCommand_read_kernel_status.h"
#include "MMCSCommandReply.h"
#include "BlockControllerNodeInfo.h"
#include "ConsoleController.h"
#include "BlockControllerTarget.h"

using namespace std;

/*!
** read_kernel_status
** [<target>] read_kernel_status
** Reads sram kernel status area
*/
MMCSCommand_read_kernel_status*
MMCSCommand_read_kernel_status::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);            // requires a BlockController object
    commandAttributes.requiresConnection(true);       // requires  mc_server connections
    commandAttributes.requiresTarget(true);           // requires a BlockControllerTarget object
    commandAttributes.internalCommand(true);           // this is an internal use command
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(ADMIN);
    MMCSCommandAttributes::AuthPair hardwareread(hlcs::security::Object::Hardware, hlcs::security::Action::Read);
    commandAttributes.addAuthPair(hardwareread);
    return new MMCSCommand_read_kernel_status("read_kernel_status", "[<target>] read_kernel_status", commandAttributes);
}

void
MMCSCommand_read_kernel_status::execute(deque<string> args,
                    MMCSCommandReply& reply,
                    ConsoleController* pController,
                    BlockControllerTarget* pTarget)
{

  //    BlockController* pBlock = pTarget->getBlockController();    // get selected block
    if (pTarget->getNodes().size() == 0)
        reply << FAIL << "no targets selected" << DONE;
    else
        reply << FAIL << "not implemented" << DONE;
}

void
MMCSCommand_read_kernel_status::help(deque<string> args,
                     MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
      << ";Reads sram kernel status area"
      << DONE;
}

