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
 * \file MMCSCommand_block_status.h
 */

#ifndef _MMCSCOMMAND_BLOCK_STATUS_H
#define _MMCSCOMMAND_BLOCK_STATUS_H

#include "MMCSCommand.h"

/*!
 ** block_status
 * [<target>] block_status
 * Lists nodes in allocated block, the type of each node, and whether it is running a program or vacant.
 * (If block has been booted, it will show that a program is running.)
 */
class MMCSCommand_block_status: public MMCSCommand
{
public:
    MMCSCommand_block_status(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] block_status";}
    static  MMCSCommand_block_status* build();    // factory method
    static  std::string cmdname() { return "block_status"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
