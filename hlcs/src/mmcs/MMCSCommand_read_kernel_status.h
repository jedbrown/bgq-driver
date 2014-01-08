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
 * \file MMCSCommand_read_kernel_status.h
 */

#ifndef _MMCSCOMMAND_READ_KERNEL_STATUS_H
#define _MMCSCOMMAND_READ_KERNEL_STATUS_H

#include "MMCSCommand.h"

/*!
** read_kernel_status
** [<target>] read_kernel_status
** Reads sram kernel status area
*/
class MMCSCommand_read_kernel_status: public MMCSCommand
{
public:
    MMCSCommand_read_kernel_status(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "[<target>] read_kernel_status";}
    static  MMCSCommand_read_kernel_status* build();    // factory method
    static  std::string cmdname() { return "read_kernel_status"; }
    void execute(
            std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );
    bool checkArgs(std::deque<std::string>& args) { if(args.size() != 0) return false; else return true; }
    void help(
            std::deque<std::string> args,
            MMCSCommandReply& reply
            );
};

#endif
