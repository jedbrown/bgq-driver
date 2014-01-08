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

#ifndef MMCS_SERVER_COMMAND_SHOW_BARRIER_H_
#define MMCS_SERVER_COMMAND_SHOW_BARRIER_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
//  show_barrier
//  syntax:
//     show_barrier -show nodes with unsatisfied control system barriers
*/
class ShowBarrier: public common::AbstractCommand
{
public:
    ShowBarrier(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "show_barrier";}
    static ShowBarrier* build();	// factory method
    virtual void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 common::ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    //    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 1) return false; else return true;}
    virtual void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
