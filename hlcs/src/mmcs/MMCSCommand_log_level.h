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

#ifndef MMCS_COMMAND_LOG_LEVEL_H
#define MMCS_COMMAND_LOG_LEVEL_H

#include "MMCSCommand.h"

class ConsoleController;

/*!
 * \brief
 */
class MMCSCommand_log_level: public MMCSCommand
{
public:
    /*!
     * \brief
     */
    MMCSCommand_log_level(
            const char* name,
            const char* description,
            const MMCSCommandAttributes& attributes
            );

    /*!
     * \brief
     */
    void execute(
            std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );

    /*!
     * \brief
     */
    void help(
            std::deque<std::string> args,
            MMCSCommandReply& reply
            );

    bool checkArgs(std::deque<std::string>&) { return true; }

public:
    static  MMCSCommand_log_level* build();
    static  std::string cmdname() { return "log_level"; }
};

#endif
