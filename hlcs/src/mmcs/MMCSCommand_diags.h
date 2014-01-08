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
 * \file MMCSCommand_diags.h
 */

#ifndef _MMCSCOMMAND_DIAGS_H
#define _MMCSCOMMAND_DIAGS_H

#include "MMCSCommand.h"
#include <utility/include/Exec.h>

/*!
** diag_wait
** diag_wait <seconds>
** Wait for diagnostic test to terminate or time out
*/
class MMCSCommand_diag_wait: public MMCSCommand
{
public:
    MMCSCommand_diag_wait(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "diag_wait [<seconds> [waitall]]"; }
    static  MMCSCommand_diag_wait* build();    // factory method
    static  std::string cmdname() { return "diag_wait"; }
    void execute(std::deque<std::string> args,
             MMCSCommandReply& reply,
             ConsoleController* pController,
             BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 2) return false; else return true; }
    void help(std::deque<std::string> args,
              MMCSCommandReply& reply);
};

#endif
