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

#ifndef MMCS_SERVER_COMMAND_PROCESSOR_H_
#define MMCS_SERVER_COMMAND_PROCESSOR_H_

#include "../MMCSCommandProcessor.h"

namespace mmcs {
namespace server {

#ifdef WITH_DB

/*!
 * \brief Executes MMCS commands from an MMCS server.
 */
class CommandProcessor : public MMCSCommandProcessor
{
public:
    CommandProcessor(MMCSCommandMap* mmcsCommands);

    /*!
     * \brief
     */
    procstat validate(
            std::deque<std::string>& cmdStr,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            server::BlockControllerTarget** pTarget,
            common::AbstractCommand** pCmd,
            std::vector<std::string>* validnames);

protected:
    /*!
     * \breif Call the command processor for the command.
     * @param cmdStr      the command string as parsed by parse()
     * @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
     * @param pController the common::ConsoleController object that the command is to work on
     * @param pTarget     the server::BlockControllerTarget list that the command is to work on
     * @param pCmd        the MMCSCommand object to be executed
     * @param status      the validation status, refer to MMCSCommandProcessor::status
     * @returns           execution status, refer to MMCSCommandProcessor::status
     */
    procstat invokeCommand(std::deque<std::string> cmdStr,
                         mmcs_client::CommandReply& reply,
                         common::ConsoleController* pController,
                         server::BlockControllerTarget* pTarget,
                         common::AbstractCommand* pCmd,
                         procstat status,
                         std::vector<std::string>& validnames);
};

#endif // WITH_DB


} } // namespace mmcs::server


#endif
