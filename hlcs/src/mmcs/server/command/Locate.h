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
 * \file Locate.h
 * The locate command shows the addressing information for nodes and idos in a block.
 */

#ifndef MMCS_SERVER_COMMAND_LOCATE_H_
#define MMCS_SERVER_COMMAND_LOCATE_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

class Locate : public common::AbstractCommand
{
public:
    /*!
    ** Locate constructor
    ** The constructor is normally invoked only by the build() method
    ** @param name the command name
    ** @param desc a brief, one line description
    ** @param attr describes the attributes and requirements of the command
    */
    Locate(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "[<target>] locate [neighbors [verbose]] [ras_format]";}

    /*!
    ** build() - MMCSCommand factory
    ** This is invoked at MMCS startup when MMCS builds its list of commands
    ** @return an MMCSCommand object for this specific command
    */
    static Locate* build();	// factory method

    /*!
    ** execute() - Perform specific MMCS command
    ** @param args the command arguments
    ** @param reply       the command output stream. Refer to class mmcs_client::CommandReply
    ** @param pController the ConsoleController object that the command is to work on
    */
    void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 common::ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);

    bool checkArgs(std::deque<std::string>& args) { return true; }

    /*!
    ** help() - Print extended command help to the reply stream
    ** @param args the help command arguments
    ** @param reply the command output stream. Refer to class mmcs_client::CommandReply
    */
    void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
