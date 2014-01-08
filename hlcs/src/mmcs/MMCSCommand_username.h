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
 * \file MMCSCommand_username.h
 * \brief Set the username.
 */

#ifndef _MMCSCOMMAND_USERNAME_H
#define _MMCSCOMMAND_USERNAME_H

#include "MMCSCommand.h"

/*!
** username <username> - returns the username
*/
class MMCSCommand_username: public MMCSCommand
{
public:
    MMCSCommand_username(const char* name, const char* description, const MMCSCommandAttributes& attributes)
        : MMCSCommand(name,description,attributes) {}
    static  MMCSCommand_username* build();  // factory method
    static  std::string cmdname() { return "username"; }
    void execute(std::deque<std::string> args,
            MMCSCommandReply& reply,
            DBConsoleController* pController,
            BlockControllerTarget* pTarget=NULL
            );
    void help(std::deque<std::string> args,
            MMCSCommandReply& reply);
};

#endif
