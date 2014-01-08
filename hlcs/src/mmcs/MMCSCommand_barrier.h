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

#ifndef _MMCSCOMMAND_BARRIER_H
#define _MMCSCOMMAND_BARRIER_H

#include "MMCSCommand.h"

/*!
//  show_barrier
//  syntax:
//     show_barrier -show nodes with unsatisfied control system barriers
*/
class MMCSCommand_show_barrier: public MMCSCommand
{
public:
    MMCSCommand_show_barrier(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "show_barrier";}
    static  MMCSCommand_show_barrier* build();	// factory method
    static  std::string cmdname() { return "show_barrier"; }
    virtual void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    //    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 1) return false; else return true;}
    virtual void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
