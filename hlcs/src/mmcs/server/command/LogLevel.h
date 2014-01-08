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
/* (C) Copyright IBM Corp.  2010, 2011                              */
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

#ifndef MMCS_SERVER_COMMAND_LOG_LEVEL_H
#define MMCS_SERVER_COMMAND_LOG_LEVEL_H


#include "common/AbstractCommand.h"


namespace mmcs {
namespace server {
namespace command {


/*!
 * \brief
 */
class LogLevel : public common::AbstractCommand
{
public:
    /*!
     * \brief
     */
    LogLevel(
            const char* name,
            const char* description,
            const Attributes& attributes
            );

    /*!
     * \brief
     */
    void execute(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply,
            common::ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );

    /*!
     * \brief
     */
    void help(
            std::deque<std::string> args,
            mmcs_client::CommandReply& reply
            );

    bool checkArgs(std::deque<std::string>&) { return true; }

public:
    static  LogLevel* build();
    static  std::string cmdname() { return "log_level"; }
};

} } } // namespace mmcs::server::command

#endif
