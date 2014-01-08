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

#ifndef _MMCSCOMMAND_REFRESH_CONFIG_H
#define _MMCSCOMMAND_REFRESH_CONFIG_H

#include "MMCSCommand.h"

/*!
** refresh_config
** Refresh_Config Reread config file to adopt new values without restarting server.
*/
class MMCSCommand_refresh_config: public MMCSCommand
{
public:
    MMCSCommand_refresh_config(const char* name, const char* description, const MMCSCommandAttributes& attributes)
      : MMCSCommand(name,description,attributes) { usage = "refresh_config [file]";}
    static  MMCSCommand_refresh_config* build();	// factory method
    static  std::string cmdname() { return "refresh_config"; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 DBConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 1) return false; else return true; }
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
};

#endif
