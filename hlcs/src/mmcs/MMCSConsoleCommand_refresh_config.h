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

#ifndef MMCSCONSOLECOMMAND_REFRESH_CONFIG_H
#define MMCSCONSOLECOMMAND_REFRESH_CONFIG_H

#include "MMCSCommand.h"

#include <vector>

class MMCSConsoleCommand_refresh_config : public MMCSCommand
{
public:
    MMCSConsoleCommand_refresh_config(
            const char* name,
            const char* description,
            const MMCSCommandAttributes& attributes
            );

    static  MMCSConsoleCommand_refresh_config* build();

    static  std::string cmdname() {
        return "refresh_config";
    }

    void execute(
            std::deque<std::string> args,
            MMCSCommandReply& reply,
            ConsoleController* controller,
            BlockControllerTarget* target=NULL
            );
    bool checkArgs(std::deque<std::string>& args) { if(args.size() > 2) return false; else return true;}
    void help(
            std::deque<std::string> args,
            MMCSCommandReply& reply
            );

private:
    void refresh(
            const std::string& server,
            const std::string& file,
            MMCSCommandReply& reply,
            ConsoleController* controller
            );

private:
    typedef std::vector<std::string> ServerContainer;
    ServerContainer _servers;
};

#endif
