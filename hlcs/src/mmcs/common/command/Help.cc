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
 * \file Help.cc
 */

#include "Help.h"

#include "../ConsoleController.h"

#include "../../MMCSCommandProcessor.h"

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>

using namespace std;

namespace mmcs {
namespace common {
namespace command {

typedef boost::tokenizer<boost::char_separator<char> > tokenizer;

bool parseCommand(
        const std::string& command, 
        std::string& name, 
        std::string& description, 
        std::string& authlist
        )
{
    // Split each command by name, description and authorization list
    if (command.find("|") == std::string::npos && command.find("$") == std::string::npos ) {
        // This isn't a command.  Return 'true'
        // to indicate we're done.
        return true;
    }

    boost::char_separator<char> bar_sep("","|");
    tokenizer bar_tok(command, bar_sep);
    tokenizer::const_iterator tokit = bar_tok.begin();
    name = *tokit;
    ++tokit;

    std::string therest; // Everything after the first separator
    while (tokit != bar_tok.end()) {
        if (tokit != bar_tok.end()) {
            therest += *tokit;
        }
        ++tokit;
    }

    boost::char_separator<char> dollar_sep("$");
    tokenizer dollar_tok(therest, dollar_sep);
    tokenizer::const_iterator ptokit = dollar_tok.begin();
    description = *ptokit;
    if (description.at(0) == ' ') {
        description.erase(0,1); // get rid of leading blank
    }
    ++ptokit;
    if (ptokit != dollar_tok.end()) {
        authlist = *ptokit;
    }
    description.erase(description.find("|"), 1);
    return false;
}

std::string
stringify_auths(
        AbstractCommand* cmd
        )
{
    std::ostringstream authstringstream;
    std::vector<AbstractCommand::Attributes::AuthPair>* auths = cmd->attributes().getAuthPairs();
    bool first = true;
    for (
            std::vector<AbstractCommand::Attributes::AuthPair>::const_iterator it = auths->begin();
            it != auths->end();
            ++it
        )
    {
        if (!first) {
            authstringstream << ",";
        }
        if (it->first == hlcs::security::Object::Block)
            authstringstream << "Block:";
        else if (it->first == hlcs::security::Object::Job)
            authstringstream << "Job:";
        else if (it->first == hlcs::security::Object::Hardware)
            authstringstream << "Hardware:";

        if (it->second == hlcs::security::Action::Create)
            authstringstream << "Create";
        else if (it->second == hlcs::security::Action::Read)
            authstringstream << "Read";
        else if (it->second == hlcs::security::Action::Update)
            authstringstream << "Update";
        else if (it->second == hlcs::security::Action::Delete)
            authstringstream << "Delete";
        else if (it->second == hlcs::security::Action::Execute)
            authstringstream << "Execute";
        first = false;
    }
    return authstringstream.str();
}

Help*
Help::build()
{
    Attributes commandAttributes;
    commandAttributes.requiresBlock(false);             // does not require a BlockController object
    commandAttributes.requiresConnection(false);        // does not require  mc_server connections
    commandAttributes.requiresTarget(false);            // does not require a server::BlockControllerTarget object
    commandAttributes.mmcsServerCommand(true);
    commandAttributes.bgConsoleCommand(true);
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.internalAuth(true);
    return new Help("help", "help [<command-name>|<category>] [ auth ]", commandAttributes);
}

void
Help::execute(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        ConsoleController* pController,
        server::BlockControllerTarget* //pTarget
    )
{
    MMCSCommandMap* const commandMap = pController->getCommandProcessor()->getCommandMap();
    MMCSCommandProcessor mmcsCommandProcessor(commandMap);
    mmcsCommandProcessor.logFailures(false);
    mmcs_client::CommandReply serverReply;
    map<string, string> helpMap;
    const unsigned cmdname_width = 23;

   if (args.size() > 2) {
       reply << mmcs_client::FAIL << "args? " << _usage << mmcs_client::DONE;
       return;
   }

   bool auth = false;
   if (std::find(args.begin(), args.end(), "auth") != args.end()) {
       auth = true;
   }

   // give command summary if no command was specified
   if ((args.size() == 0) ||
       (((args.size() == 1 || args.size() == 2) &&
         (args[0] == "user" || args[0] == "admin"||
          args[0] == "target" || args[0] == "special" || args[0] == "default" ||
          args[0] == "all")))) {


       if (args.size() >= 1 && args[0] == "target") {
           // print the legend
           reply << mmcs_client::OK;
           reply << "\n"
                 << "Some commands accept an optional <target>, which may be used to specify the hardware\n"
                 << "on which to apply the operation:\n"
                 << "\n"
                 << setw(cmdname_width) << left << "{*}              " << "| all nodes (default)\n"
                 << setw(cmdname_width) << left << "{<N>}            " << "| single node\n"
                 << setw(cmdname_width) << left << "{<N>,<N>,...}    " << "| list\n"
                 << setw(cmdname_width) << left << "{<N>-<N>}        " << "| subrange\n"
                 << setw(cmdname_width) << left << "{<N>-<N>,<N>,...}" << "| combinations\n";
       } else {
           // send a help command to the mmcs server
           deque<string> cmdStr;
           HELP_CAT hc = DEFAULT;
           cmdStr.push_back("help");
           if (args.size() == 0) {
               cmdStr.push_back("default");
           } else if(args.size() >= 1) {
               BOOST_FOREACH(std::string& arg, args) {
                   cmdStr.push_back(arg);
               }
               if (args[0] == "user") hc = USER;
               else if (args[0] == "admin") hc = ADMIN;
               else if (args[0] == "special") hc = SPECIAL;
               else hc = DEFAULT;
           }

           // Do we want them all?
           bool all = false;
           if ((args.size() == 1) && (args[0] == "all")) {
               all = true;
           }

           mmcsCommandProcessor.execute("mmcs_server_cmd",cmdStr, serverReply, pController);

           std::string serverHelp("");
           // parse the response from the mmcs server
           if (serverReply.getStatus() == 0) {
               // This only happens on the client side.
               // add all the server command names and descriptions to the helpMap
               serverHelp = serverReply.str(); // help response from server
               // Break up the semicolon separated list of commands
               boost::char_separator<char> sem_sep(";");
               tokenizer sem_tok(serverHelp, sem_sep);
               BOOST_FOREACH(std::string current_command, sem_tok) {
                   std::string name, description, authlist;
                   bool done = parseCommand(current_command, name, description, authlist);
                   if (!done) {
                       if ( name.find("help") == std::string::npos)  {
                           // remove whitespace from name
                           name.erase(name.find(" "));
                           if (auth == false)
                               helpMap[name] = description;
                           else
                               helpMap[name] = authlist;
                       }
                   } else
                       break;
               }
           }

           map<string, string> clientMap;
           // add the local commands to the helpMap
           for (
                   MMCSCommandMap::const_iterator pos = commandMap->begin();
                   pos != commandMap->end();
                   ++pos
               )
           {
               AbstractCommand* cmd = pos->second;
               if (cmd->attributes().helpCategory() == hc || all) {
                   // Get the authorizations for the command and stringify them.
                   std::string authstring;
                   if (cmd->attributes().getBgAdminAuth() == true)
                       authstring = "administrative";
                   else if(cmd->attributes().getAuthPairs()->size() > 0) {
                       authstring = stringify_auths(cmd);
                   }
                   else if(cmd->attributes().specialAuthString().length() > 0){
                       authstring = cmd->attributes().specialAuthString();
                   } else
                       authstring = "none";
                   std::string descauth = cmd->description() + "$" + authstring;

                   if (serverReply.getStatus() == 0 ||
                           serverReply.str().length() == 25) { // 25 chars in bad command reply.
                       // We're the console
                       clientMap[cmd->name()] = descauth;
                   } else {
                       helpMap[cmd->name()] = descauth;
                   }
               }
           }

           // Whip through the client's commands, parse them out, and overwrite
           // what's already in the map.
           for (
                   map<string,string>::const_iterator pos = clientMap.begin();
                   pos != clientMap.end();
                   ++pos
               )
           {
               std::string name, description, authlist;
               name = pos->first;
               std::string command = name + "| " + pos->second;
               bool done = parseCommand(command, name, description, authlist);
               if (!done) {
                   if (auth == false)
                       helpMap[name] = description;
                   else
                       helpMap[name] = authlist;
               } else
                   break;
           }

           // print a command summary
           reply << mmcs_client::OK;
           for (
                   map<string,string>::const_iterator pos = helpMap.begin();
                   pos != helpMap.end();
                   ++pos
               )
           {
               reply << setw(cmdname_width) << left << pos->first << "| " << pos->second << '\n';
           }
       }

       reply << "\n"
             << "Command summaries are shown for categories of commands.\n"
             << "The categories are user, admin, target, and special.\n"
             << "There is also a default set of commands that is listed when no command name or category is provided.\n"
             << "The authorities required for the objects on which the command operates may also be printed if \"auth\"\n"
             << "is specified.  The object types are Block, Job, and Hardware.  The authority types are [C]reate,\n"
             << "[R]read, [U]pdate, [D]elete, and [E]xecute.  There are also special administrative authorities.\n"
             << "\n"
             << "For a list of all commands: help all";

       reply << mmcs_client::DONE;
       return;
   }

   // give extended help for specified command
   if (args[0] == "auth") {
       reply << mmcs_client::FAIL << "\"auth\" must be used with a category specified.\nType \"help help\" for more information." << mmcs_client::DONE;
       return;
   }

   MMCSCommandMap::const_iterator pos = commandMap->find(args[0]);
   if (pos != commandMap->end()) { // found the command in our commandMap
       // execute the command's help method
       AbstractCommand* cmd = pos->second;
       mmcs_client::CommandReply cmdReply;
       cmd->help(args, cmdReply);
       std::string authstring;
       if (cmd->attributes().getBgAdminAuth() == true) {
           authstring = "administrative";
       } else if(cmd->attributes().getAuthPairs()->size() > 0) {
           authstring = stringify_auths(cmd);
       } else if(cmd->attributes().specialAuthString().length() > 0) {
           authstring = cmd->attributes().specialAuthString();
       } else {
           authstring = "none";
       }

       if (cmd->attributes().externalCommand() == false) {
           reply << mmcs_client::OK << cmdReply.str() << "\nAuthorizations required are: "
           << authstring << mmcs_client::DONE;
       } else {
           reply << mmcs_client::OK << cmdReply.str() << mmcs_client::DONE;
       }
   } else {	// didn't find the command in our command map
       // send the help command to the server for execution
       deque<string> cmdStr;
       cmdStr.push_back("help");
       cmdStr.push_back(args[0]);
       mmcsCommandProcessor.execute("mmcs_server_cmd",cmdStr, serverReply, pController);
       if (serverReply.getStatus() == 0)
           reply << mmcs_client::OK << serverReply.str() << mmcs_client::DONE;
       else
           reply << mmcs_client::FAIL << "No help available for command or category " << args[0] << mmcs_client::DONE;
       return;
   }
}

void
Help::help(deque<string> ,//args,
          mmcs_client::CommandReply& reply)
{
    reply << mmcs_client::OK << description()
          << ";Print extended help for a specific command, or command summaries for a category of commands."
          << ";Categories are user, admin, target, and special."
          << ";There is also a default set of commands that is listed when no command name or category is provided."
          << ";The authorities required for the objects on which the command operates may also be printed if \"auth\""
          << ";is specified. The object types are Block, Job, and Hardware.  The authority types are [C]reate,"
          << ";[R]read, [U]pdate, [D]elete, and [E]xecute. There are also special administrative authorities."
          << mmcs_client::DONE;
}


} } } // namespace mmcs::common::command
