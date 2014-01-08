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
/* (C) Copyright IBM Corp.  2009, 2011                              */
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
 * \file MMCSCommand_refresh_config.h
 */

#ifndef MMCS_SERVER_COMMAND_REFRESH_CONFIG_H_
#define MMCS_SERVER_COMMAND_REFRESH_CONFIG_H_

#include "common/AbstractCommand.h"

namespace mmcs {
namespace server {
namespace command {

/*!
** refresh_config
** Refresh_Config Reread config file to adopt new values without restarting server.
*/
class RefreshConfig : public common::AbstractCommand
{
public:
    RefreshConfig(const char* name, const char* description, const Attributes& attributes)
      : AbstractCommand(name,description,attributes) { _usage = "refresh_config [file]";}
    static RefreshConfig* build();	// factory method
    void execute(std::deque<std::string> args,
			 mmcs_client::CommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if (args.size() > 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      mmcs_client::CommandReply& reply);
};

} } } // namespace mmcs::server::command

#endif
