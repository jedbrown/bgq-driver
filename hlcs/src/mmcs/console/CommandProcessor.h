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

#ifndef MMCS_CONSOLE_COMMAND_PROCESSOR_H_
#define MMCS_CONSOLE_COMMAND_PROCESSOR_H_

#include "../MMCSCommandProcessor.h"

namespace mmcs {
namespace console {

/*!
 * \brief Executes MMCS commands from bg_console.
 */
class CommandProcessor : public MMCSCommandProcessor
{
public:
    CommandProcessor(MMCSCommandMap* mmcsCommands);

protected:
    procstat invokeCommand(
            std::deque<std::string> cmdStr,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget* pTarget,
            common::AbstractCommand* pCmd,
            procstat status,
            std::vector<std::string>& validnames
            );

    //! \brief Execute an external command
    void executeExternal(
            std::deque<std::string>& cmdStr,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget* pTarget
            );
};

} } // namespace mmcs::console

#endif
