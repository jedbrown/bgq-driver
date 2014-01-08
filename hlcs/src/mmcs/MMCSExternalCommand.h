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

#ifndef MMCS_EXTERNAL_COMMAND_H
#define MMCS_EXTERNAL_COMMAND_H

#include "MMCSCommand.h"

class MMCSExternalCommand : public MMCSCommand {
public:
    MMCSExternalCommand(const char* name, const char* description, const MMCSCommandAttributes& attributes)
        : MMCSCommand(name,description,attributes) { /*? usage set ?  */}
    static MMCSExternalCommand* build(std::string& name, std::string& description);
    std::string cmdname() { return _name; }
    void execute(std::deque<std::string> args,
			 MMCSCommandReply& reply,
			 ConsoleController* pController,
			 BlockControllerTarget* pTarget=NULL);
    bool checkArgs(std::deque<std::string>& args) { return true;}
    void help(std::deque<std::string> args,
		      MMCSCommandReply& reply);
    static void setPropfile(const std::string& f) { _propfile = f; }
private:
    static void runcmd(MMCSCommandReply& reply, const std::ostringstream& cmd_and_args, const std::string& executable);
    static MMCSExternalCommand* build() { return 0; }
    static std::string _propfile;
};

#endif
