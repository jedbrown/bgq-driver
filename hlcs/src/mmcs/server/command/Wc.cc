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

#include "Wc.h"

namespace mmcs {
namespace server {
namespace command {

Wc*
Wc::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.requiresConnection(false);       // does require  mc_server connections
    commandAttributes.requiresTarget(true);            // does require a BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(common::ADMIN);
    Attributes::AuthPair blockexecute(hlcs::security::Object::Block, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(blockexecute);
    return new Wc("wc", "[<target>] write_con <console-command>", commandAttributes);
}

} } } // namespace mmcs::server::command
