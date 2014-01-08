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
/* (C) Copyright IBM Corp.  2004, 2011                              */
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
 * \file MMCSCommandProcessor.cc
 */

#include <boost/foreach.hpp>
#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>
#include "lite/MMCSCommand_runjob.h"
#include "lite/MMCSCommand_killjob.h"
#include "lite/MMCSCommand_waitjob.h"

#include "BlockControllerTarget.h"
#include "ConsoleController.h"
#include "MMCSProperties.h"

#include "MMCSCommandProcessor.h"
#include "MMCSCommand_allocate.h"
#include "MMCSCommand_authority.h"
#include "MMCSCommand_block.h"
#include "MMCSCommand_block_status.h"
#include "MMCSCommand_db.h"
#include "MMCSCommand_diags.h"
#include "MMCSCommand_dump.h"
#include "MMCSCommand_envs.h"
#include "MMCSCommand_free.h"
#include "MMCSCommand_help.h"
#include "MMCSCommand_ionodes.h"
#include "MMCSCommand_lite.h"
#include "MMCSCommand_list.h"
#include "MMCSCommand_locate.h"
#include "MMCSCommand_log_level.h"
#include "MMCSCommand_mcserver.h"
#include "MMCSCommand_mmcs_server_cmd.h"
#include "MMCSCommand_mmcs_server_connect.h"
#include "MMCSCommand_read_kernel_status.h"
#include "MMCSCommand_redirect.h"
#include "MMCSCommand_replyformat.h"
#include "MMCSCommand_select_block.h"
#include "MMCSCommand_shell.h"
#include "MMCSCommand_username.h"
#include "MMCSCommand_version.h"
#include "MMCSCommand_reboot.h"
#include "MMCSCommand_inject_ras.h"
#include "MMCSCommand_refresh_config.h"
#include "MMCSCommand_barrier.h"
#include "MMCSServerCommand_xml.h"
#include "MMCSConsoleCommand_xml.h"
#include "MMCSServerCommand_block.h"
#include "MMCSConsoleCommand_redirect.h"
#include "MMCSConsoleCommand_refresh_config.h"
#include "MMCSConsoleCommand_replyformat.h"
#include "MMCSConsoleCommand_username.h"
#include "MMCSExternalCommand.h"
#include "MMCSServerCommand_disconnect.h"
#include "ras.h"

#ifdef WITH_DB
#include <db/include/api/BGQDBlib.h>
#include <hlcs/include/security/Enforcer.h>
#endif

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/Exec.h>

#include <vector>

LOG_DECLARE_FILE( "mmcs" );

using namespace std;

// Static initialization
EnforcerPtr MMCSCommandProcessor::_command_enforcer;

// /brief Build a vector containing all of the available MMCSCommands
static vector<MMCSCommand*>* allCommands()
{
    vector<MMCSCommand*> *commands = new vector<MMCSCommand*>;

#ifdef MMCS_SERVER
#ifdef WITH_DB
#ifdef WITH_SECURITY
       commands->push_back(MMCSCommand_grant_block_authority::build());
       commands->push_back(MMCSCommand_revoke_block_authority::build());
       commands->push_back(MMCSCommand_list_block_authority::build());
#endif // end WITH_SECURITY
       // commands->push_back(MMCSCommand_mmcs_server_cmd::build());
       // commands->push_back(MMCSCommand_mmcs_server_connect::build());
       commands->push_back(MMCSCommand_allocate::build());
       commands->push_back(MMCSCommand_allocate_block::build());
       commands->push_back(MMCSCommand_deselect_block::build());
       commands->push_back(MMCSCommand_free::build());
       commands->push_back(MMCSCommand_free_block::build());
       commands->push_back(MMCSCommand_free_all::build());
       commands->push_back(MMCSCommand_list_blocks::build());
       commands->push_back(MMCSCommand_list_selected_block::build());
       commands->push_back(MMCSCommand_list_users::build());
       commands->push_back(MMCSCommand_reboot_nodes::build());
       commands->push_back(MMCSCommand_redirect::build());
       commands->push_back(MMCSCommand_redirect_block::build());
       commands->push_back(MMCSCommand_select_block::build());
       commands->push_back(MMCSCommand_start_hw_polling::build());
       commands->push_back(MMCSCommand_stop_hw_polling::build());
       commands->push_back(MMCSCommand_list_hw_polling::build());
       commands->push_back(MMCSCommand_username::build());
       commands->push_back(MMCSCommand_wait_boot::build());
       commands->push_back(MMCSServerCommand_add_boot_option::build());
       commands->push_back(MMCSServerCommand_dump_block::build());
       commands->push_back(MMCSServerCommand_copy_block::build());
       commands->push_back(MMCSServerCommand_create_node_config::build());
       commands->push_back(MMCSCommand_delete::build());
       commands->push_back(MMCSServerCommand_gen_io_block::build());
       commands->push_back(MMCSServerCommand_gen_midplane_block::build());
       commands->push_back(MMCSServerCommand_gen_block::build());
       commands->push_back(MMCSServerCommand_gen_blocks::build());
       commands->push_back(MMCSServerCommand_gen_full_block::build());
       commands->push_back(MMCSServerCommand_gen_small_block::build());
       commands->push_back(MMCSServerCommand_disconnect::build());
       commands->push_back(MMCSCommand_get_block_info::build());
       commands->push_back(MMCSCommand_list::build());
       commands->push_back(MMCSCommand_list_midplanes::build());
       commands->push_back(MMCSCommand_list_io::build());
       commands->push_back(MMCSCommand_list_io_links::build());
       commands->push_back(MMCSServerCommand_dump_machine::build());
       commands->push_back(MMCSServerCommand_boot_block::build());
       commands->push_back(MMCSServerCommand_set_block_info::build());
       commands->push_back(MMCSServerCommand_set_boot_options::build());
       commands->push_back(MMCSCommand_sql::build());
       commands->push_back(MMCSCommand_kill_midplane_jobs::build());
       commands->push_back(MMCSCommand_complete_io_service::build());
       commands->push_back(MMCSCommand_refresh_config::build());
#endif // end WITH_DB
       commands->push_back(MMCSCommand_help::build());
       commands->push_back(MMCSCommand_show_barrier::build());
       commands->push_back(MMCSCommand_connect::build());
#ifndef WITH_DB
       // these are mmcs_lite commands
       commands->push_back(MMCSCommand_create_block::build());
       commands->push_back(MMCSCommand_bringup::build());
       commands->push_back(MMCSCommand_dump_ras::build());
       commands->push_back(MMCSCommand_gen_block::build());
       commands->push_back(MMCSCommand_gen_io_block::build());
       commands->push_back(MMCSCommand_killjob::build());
       commands->push_back(MMCSCommand_waitjob::build());
       commands->push_back(MMCSCommand_runjob::build());
       commands->push_back(MMCSCommand_show_envs::build());
       commands->push_back(MMCSCommand_wait_for_terminate::build());
       commands->push_back(MMCSCommand_d::build());
       commands->push_back(MMCSCommand_sleep::build());
       commands->push_back(MMCSCommand_sub_shell::build());
       commands->push_back(MMCSCommand_redirect_input::build());
       commands->push_back(MMCSCommand_delete_block::build());
       commands->push_back(MMCSCommand_boot_block::build());
#endif // end !WITH_DB
#ifdef MMCS_PROHIBITED
       commands->push_back(MMCSCommand_inject_ras::build());
       commands->push_back(MMCSCommand_inject_console::build());
#endif // end MMCS_PROHIBITED
       commands->push_back(MMCSCommand_diag_wait::build());
       commands->push_back(MMCSCommand_disconnect::build());
       commands->push_back(MMCSCommand_dump_block::build());
       commands->push_back(MMCSCommand_dump_machine::build());
       commands->push_back(MMCSCommand_dump_personalities::build());
       commands->push_back(MMCSCommand_get_block_size::build());
       commands->push_back(MMCSCommand_locate::build());
       commands->push_back(MMCSCommand_log_level::build());
       commands->push_back(MMCSCommand_mcserver_clients::build());
       commands->push_back(MMCSCommand_mcserver_status::build());
       commands->push_back(MMCSCommand_quit::build());
       commands->push_back(MMCSCommand_replyformat::build());
       commands->push_back(MMCSCommand_block_status::build());
       commands->push_back(MMCSCommand_sysrq::build());
       commands->push_back(MMCSCommand_version::build());
       commands->push_back(MMCSCommand_write_con::build());
       commands->push_back(MMCSCommand_wc::build());
#endif // end MMCS_SERVER
#ifdef BG_CONSOLE
       commands->push_back(MMCSCommand_comment::build());
       commands->push_back(MMCSConsoleCommand_quit::build());
       commands->push_back(MMCSCommand_sleep::build());
       commands->push_back(MMCSCommand_sub_shell::build());
       commands->push_back(MMCSCommand_redirect_input::build());
       commands->push_back(MMCSConsoleCommand_dump_machine::build());
       commands->push_back(MMCSConsoleCommand_redirect::build());
       commands->push_back(MMCSConsoleCommand_redirect_block::build());
       commands->push_back(MMCSConsoleCommand_refresh_config::build());
       commands->push_back(MMCSConsoleCommand_replyformat::build());
       commands->push_back(MMCSConsoleCommand_username::build());
       commands->push_back(MMCSConsoleCommand_dump_block::build());
       commands->push_back(MMCSCommand_help::build());
       commands->push_back(MMCSCommand_mmcs_server_cmd::build());
       commands->push_back(MMCSCommand_mmcs_server_connect::build());
#endif // end bg_console
    return commands;
}



/*!
** log the command and parameters
** @param string - command name
** @param deque<string> - command parameters
*/
void
MMCSCommandProcessor::logCommand(string cmd, deque<string> args)
{
    if (cmd.size() > 0 || args.size() > 0)
    {
        ostringstream oss;
        if (cmd.size() != 0)
            oss << cmd << " ";
        for (unsigned i = 0; i < args.size(); ++i)
            oss << args[i] << " ";
        LOG_INFO_MSG( oss.str() );
    }
}

/*!
** log the command and parameters
** @param deque<string> - command name and command parameters
*/
void
MMCSCommandProcessor::logCommand(deque<string> cmd_and_args)
{
    if (cmd_and_args.size() > 0)
    {
        ostringstream oss;
        for (unsigned i = 0; i < cmd_and_args.size(); ++i)
            oss << cmd_and_args[i] << " ";

        LOG_INFO_MSG( oss.str() );
    }
}

/*!
** /brief create a command map from specified attributes
** /parm attr MMCSCommandAttributes matching desired commands
** /parm mask MMCSCommandAttributes mask indicating which attributes
**            to check
*/
MMCSCommandMap*
MMCSCommandProcessor::createCommandMap(MMCSCommandAttributes& attr, MMCSCommandAttributes& mask,
                                       MMCSProperties::Map* externalCmds)
{
    LOG_DEBUG_MSG("Creating command map");

    // initialize the command map
    MMCSCommandMap* cmdMap = new MMCSCommandMap;

#ifdef WITH_DB // No security in mmcs_lite
    if(attr.mmcsConsoleCommand() != true) {
        // Security in the server, not the client
        EnforcerPtr ep(new hlcs::security::Enforcer(MMCSProperties::getProperties() ));
        MMCSCommandProcessor::_command_enforcer = ep;
    }
#endif

    // create a list of all MMCS commands
    vector<MMCSCommand*>* cmds = allCommands();

    if(externalCmds != 0) {
        for(MMCSProperties::Map::iterator it = externalCmds->begin(); it != externalCmds->end();
            ++it) {
            MMCSCommandAttributes cmdattr;
            cmdattr.externalCommand(true);
            MMCSExternalCommand* externalcmd = new MMCSExternalCommand((*it).first.c_str(), (*it).second.c_str(), cmdattr);

            if(MMCSProperties::getProperty("USER").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(USER);
            else if(MMCSProperties::getProperty("ADMIN").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(ADMIN);
            else if(MMCSProperties::getProperty("SPECIAL").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(SPECIAL);
            else
                externalcmd->attributes().helpCategory(DEFAULT);
            externalcmd->attributes().externalCommand(true);
            externalcmd->attributes().mmcsConsoleCommand(true);
            cmds->push_back(externalcmd);
        }
    }

    // check each command for the attributes specified by the caller.
    // If they match, add them to the command map
    while (!cmds->empty())
    {
	MMCSCommand* cmd = cmds->back();	// get the last element in the list
	cmds->pop_back();	// remove it from the list

	// check the command attributes against the selection criteria
	if ((mask.requiresBlock() && cmd->attributes().requiresBlock() != attr.requiresBlock()) ||
	    (mask.requiresTarget() && cmd->attributes().requiresTarget() != attr.requiresTarget()) ||
	    (mask.requiresConnection() && cmd->attributes().requiresConnection() != attr.requiresConnection()) ||
	    (mask.internalCommand() && cmd->attributes().internalCommand() != attr.internalCommand()) ||
	    (mask.mmcsConsoleCommand() && cmd->attributes().mmcsConsoleCommand() != attr.mmcsConsoleCommand()) ||
	    (mask.mmcsServerCommand() && cmd->attributes().mmcsServerCommand() != attr.mmcsServerCommand()) ||
	    (mask.mmcsLiteCommand() && cmd->attributes().mmcsLiteCommand() != attr.mmcsLiteCommand()))
	{
            LOG_TRACE_MSG("Deleting cmd " << cmd->name() << " from the command map");
	    delete cmd;		// this command doesn't match the selection criteria
	}
	else
	{
            if (!mask.externalCommand() && cmd->attributes().externalCommand() == true) {
                // If we did not specify external commands and this is one, delete it.  (IE in mmcs_server).
                LOG_TRACE_MSG("Deleting internal cmd " << cmd->name() << " from the command map");
                delete cmd;		// this command doesn't match the selection criteria
            }

            LOG_TRACE_MSG("Adding command " << cmd->name() << " to the command map");
	    (*cmdMap)[cmd->name()] = cmd; // add the command to the command map
	}
    }

    // return the commands matching the attributes specified by the caller
    return cmdMap;
}

/*!
** Break up command line into words.
** @param rStr	          null terminated command line
** @returns deque<string> sequence of words
*/
deque<string>
MMCSCommandProcessor::parseCommand(const std::string &rStr)
{
    deque<string> words;
    string szDelimiters = " \n";

    std::string::size_type lastPos(rStr.find_first_not_of(szDelimiters, 0));
    std::string::size_type pos(rStr.find_first_of(szDelimiters, lastPos));
    while (std::string::npos != pos || std::string::npos != lastPos)
    {
	words.push_back(rStr.substr(lastPos, pos - lastPos));
	lastPos = rStr.find_first_not_of(szDelimiters, pos);
	pos = rStr.find_first_of(szDelimiters, lastPos);
    }
    return words;
}

/*! /brief combine words into a command line
**  /param deque<string> sequence of words
**  /returns string      null terminated command line
*/
string
MMCSCommandProcessor::unparseCommand(const std::deque<string>& cmdStr)
{
    string str;
    if (cmdStr.size() > 0)
    {
	for (unsigned i = 0; i < cmdStr.size(); ++i)
	    str.append(cmdStr[i]).append(" ");
    }
    if (str.size() > 0 && str[str.size()-1] != '\n')
	str.append("\n");
    return str;
}

#ifndef WITH_SECURITY
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::validate_security(deque<string>& cmdStr,
                                        MMCSCommandReply& reply,
                                        ConsoleController* pController,
                                        MMCSCommand** pCmd,
                                        std::string& cmdName,
                                        std::vector<std::string>* validnames) {
    return MMCSCommandProcessorStatus::procstat(0);
}
#else
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::validate_security(deque<string>& cmdStr,
                                        MMCSCommandReply& reply,
                                        ConsoleController* pController,
                                        MMCSCommand** pCmd,
                                        std::string& cmdName,
                                        std::vector<std::string>* validnames) {

    if(_bg_console) // Security happens in the server, the console skips it.
        return MMCSCommandProcessorStatus::procstat(0);

    // We're doing security.  Here are a few ground rules:
    // 1)  External commands' servers do their own security.  We don't check it.
    // 2)  Internal commands have their own security requirements hard coded and
    //     stored in the command objects.
    // 3)  There are certain internal or trivial commands that don't access security
    //     objects. They're marked and we skip them.
    // 4)  bgadmin is a special authority.
    // 5)  We can have multiple authorities for security objects in a single command.
    //     They are stored in pairs.  We need to loop through the pairs and make sure
    //     all objects are valid.
    // 6)  If we have a block object, we're going to use it as a block auth object.
    // 7)  If a block object is ALL we have, then we don't need to worry about sending
    //     a list of object names to the command.
    // 8)  Some commands are able to operate on objects in addition to the selected block
    //     object.  They have a mechanism, getBlockObjects(), to return a list of those
    //     names when given a command string.
    // 9)  If a command returns a list of objects, we DON'T use the selected block
    //     and just validate the results of getBlockObjects() and send it back to them.
    // 10) An object may be validated for one action and invalidated for another.
    //     If a command requires both, the object may NOT be used.
    // 11) Hardware auth is all or nothing.

    BlockHelperPtr pBlock = pController->getBlockHelper();
    if((*pCmd)->attributes().externalCommand() == true)
        // We can return without security checking because external servers do their own.
        return MMCSCommandProcessorStatus::CMD_EXTERNAL;

    // Special internal commands without security objects.
    if((*pCmd)->attributes().getInternalAuth() == true)
        return MMCSCommandProcessorStatus::procstat(0);

    bool blockauth = false;  // Set this if we need authorization to a block

    std::vector<MMCSCommandAttributes::AuthPair>* aps = (*pCmd)->attributes().getAuthPairs();
    std::vector<std::string> bogus_objects;
    if(aps->size() == 0 && (*pCmd)->attributes().getBgAdminAuth() != true ) {
        std::ostringstream msg;
        msg << "Internal error.  Command " <<
            cmdName << " invalid.  Has no security data.";
        LOG_ERROR_MSG(msg.str());
        reply << FAIL << msg.str() << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    } else if((*pCmd)->attributes().getBgAdminAuth()) {
        if (pController->getUserType() == CxxSockets::Administrator) {
            LOG_DEBUG_MSG("bgadmin authority granted");
            // We're good to go.  Don't need to do anything else.
            return MMCSCommandProcessorStatus::CMD_EXECUTED;
        } else {
#ifdef WITH_DB
            RasEventImpl event(MMCSOps_0301);
            event.setDetail("USER", pController->getUser().getUser());
            event.setDetail("COMMAND", (*pCmd)->name());
            RasEventHandlerChain::handle(event);
            reply << FAIL << event.getDetail(RasEvent::MESSAGE) << DONE;
            BGQDB::putRAS(event);
            return MMCSCommandProcessorStatus::CMD_INVALID;
#endif
        }
    } else {  // We have auth pairs
        for(std::vector<MMCSCommandAttributes::AuthPair>::iterator it = aps->begin();
            it != aps->end(); ++it) {
            MMCSCommandAttributes::AuthPair ap = *it;
            std::vector<std::string> objnames;
            if(ap.first == hlcs::security::Object::Block) {
                blockauth = true;
                if(pBlock) {
                    objnames.push_back(pBlock->getBase()->getBlockName());
                }

                // See if the command can parse out some block IDs. It'll only do that
                // if it has to operate on something other than the selected block.
                // Note that the following will nuke anything stuffed in 'objnames'
                // in the if check above.  It's supposed to do that.  See rule #9 above.
                std::vector<std::string> t = (*pCmd)->getBlockObjects(cmdStr, (DBConsoleController*)(pController));
                bool good = false;
                if(!t.empty()  && (*pCmd)->attributes().requiresBlock() == false) {
                    // This means the command has at least one arg.

                    // Copy_block needs to do its own special authorization.
                    // This lets it happen.
                    MMCSCommandProcessorStatus::procstat stat;
                    boost::shared_ptr<hlcs::security::Enforcer> e = MMCSCommandProcessor::_command_enforcer;
                    bgq::utility::UserId i = pController->getUser();
                    if((*pCmd)->doSpecialAuths(t, e, stat, i) == true) {
                        LOG_TRACE_MSG("Special Auths done");
                        for(unsigned i = 0; i < t.size(); ++i) {
                            validnames->push_back(t[i]);
                        }
                        if(stat == MMCSCommandProcessorStatus::CMD_INVALID) {
                            t.clear(); // No usable objects!
                            reply << FAIL << "User has insufficient authority to necessary security objects." << DONE;
                            return stat;
                        }
                    } else {
#ifdef WITH_DB
                        for(std::vector<std::string>::iterator it = t.begin(); it != t.end(); ++it) {
                            BGQDB::BlockInfo binfo;
                            bool foundindb = (BGQDB::getBlockInfo(*it, binfo) == BGQDB::OK);
                            bool createaction = (ap.second == hlcs::security::Action::Create);
                            if((foundindb && !createaction) || (!foundindb && createaction)) {
                                // If it's create, we'll call it good because it won't be in the db any way.
                                LOG_TRACE_MSG("Got a valid block from command " << *it);
                                good = true;
                            } else if(!foundindb && !createaction) {
                                LOG_DEBUG_MSG("Block is not in DB and action is not create.");
                                if(std::find(bogus_objects.begin(), bogus_objects.end(), *it) == 
                                   bogus_objects.end())
                                    bogus_objects.push_back(*it);
                                good = false;
                            } else if(foundindb && createaction) {
                                reply << FAIL << "Cannot create block already created." << DONE;
                                return MMCSCommandProcessorStatus::CMD_INVALID;
                            }
                        }
#endif
                    }
                    if(good || (*pCmd)->attributes().requiresObjNames()) {
                        // At least one name in the arg list is a valid block and we don't
                        // need a selected block so we'll use the arg list.
                        objnames = t;
                    }
                }

                // Found no objnames and we don't require a =selected= block.
                if(objnames.size() == 0 && (*pCmd)->attributes().requiresBlock() == true) {
                    // If there's no block object and we need one, and the command doesn't
                    // even know about any, we darn well better not
                    // even attempt to do security.
                    std::ostringstream msg;
                    msg << "Internal error.  Command security requires block security object but no block object exists.";
                    LOG_ERROR_MSG(msg.str());
                    reply << FAIL << msg.str() << DONE;
                    return MMCSCommandProcessorStatus::CMD_INVALID;
                }

            } else if(ap.first == hlcs::security::Object::Hardware) {
                objnames.clear(); // Empty string.  Hardware is all or nothing.
                try {
                    hlcs::security::Object hw(hlcs::security::Object::Hardware, "all hardware");
                    if(MMCSCommandProcessor::_command_enforcer->
                       validate(hw,
                                ap.second,
                                pController->getUser()) == false) {
                        std::ostringstream emsg;
                        emsg << "User " << pController->getUser().getUser()
                             << " not authorized to hardware.";
                        LOG_ERROR_MSG(emsg.str());
                        reply << FAIL << emsg.str() << DONE;
                        return MMCSCommandProcessorStatus::CMD_INVALID;
                    } else {
                        LOG_DEBUG_MSG("User " << pController->getUser().getUser()
                                      << " authorized to hardware.");
                    }
                } catch (std::runtime_error& d) {
                    reply << FAIL << "Unable to validate object hardware.  " << d.what() << ". "  << (*pCmd)->description() << DONE;
                    return MMCSCommandProcessorStatus::CMD_INVALID;
                } catch (std::invalid_argument& v) {
                    reply << FAIL << "Unable to validate object hardware.  " << v.what() << ". "  << (*pCmd)->description() << DONE;
                    return MMCSCommandProcessorStatus::CMD_INVALID;
                }
            } else {
                std::ostringstream msg;
                msg << "Internal error.  Invalid security object type." << ap.first;
                LOG_ERROR_MSG(msg.str());
                reply << FAIL << msg.str() << DONE;
                return MMCSCommandProcessorStatus::CMD_INVALID;
            }

            LOG_DEBUG_MSG("Command " << cmdName << " wants to operate on "
                          << objnames.size() << " objects.");

            // For each name returned by the command, we have to
            // determine if we can use it.
            std::ostringstream msg;
            msg << "User " << pController->getUser().getUser()
                << " does not have appropriate authority for security object(s)";
            std::vector<std::string> failednames; // fill with names that enforcer disallows
            for(std::vector<std::string>::iterator i = objnames.begin();
                i != objnames.end(); ++i) {
                LOG_TRACE_MSG("validating object name " << *i);
                hlcs::security::Object blockobj(hlcs::security::Object::Block, *i);
                try {
                    if(MMCSCommandProcessor::_command_enforcer->
                       validate(blockobj,
                                ap.second,
                                pController->getUser()) == false) {
                        msg << " " << *i;
                        failednames.push_back(*i);
                        LOG_ERROR_MSG(msg.str());
                    } else {
                        LOG_DEBUG_MSG("User " << pController->getUser().getUser()
                                      << " authorized to object " << *i);
                        if(std::find(validnames->begin(), validnames->end(), *i) == validnames->end())
                            validnames->push_back(*i); // It's good.  Save it.
                    }
                } catch (std::runtime_error& d) {
                    reply << FAIL << "Unable to validate object " << *i << ".  " << d.what() << ". "  << (*pCmd)->description() << DONE;
                    return MMCSCommandProcessorStatus::CMD_INVALID;
                } catch (std::invalid_argument& v) {
                    reply << FAIL << "Unable to validate object " << *i << ".  " << v.what() << ". "  << (*pCmd)->description() << DONE;
                    return MMCSCommandProcessorStatus::CMD_INVALID;
                }
            }

            // If none of the names the command wants to use are
            // available, fail the command.
            if(validnames->size() == 0 && objnames.size() != 0) {
                RasEventImpl event(MMCSOps_0301);
                event.setDetail("USER", pController->getUser().getUser());
                event.setDetail("COMMAND", cmdName);
                RasEventHandlerChain::handle(event);
                BGQDB::putRAS(event);
                reply << FAIL << event.getDetail(RasEvent::MESSAGE) << DONE;
                return MMCSCommandProcessorStatus::CMD_INVALID;
            }

            // Now check to make sure that none of the failed names
            // are in the valid list.  This is a subtle issue.  If
            // a user needs BOTH B:E and B:R authority to perform
            // an action on that block, then that block name will
            // show up in both validnames and the failednames .
            for(std::vector<std::string>::iterator it = failednames.begin();
                it != failednames.end(); ++it) {
                std::vector<std::string>::iterator found = std::find(validnames->begin(), validnames->end(), *it);
                if(found != validnames->end()) {
                    // Uh oh.  There's a bad one in the good list.  Yank it.
                    LOG_TRACE_MSG("Removing " << *found << " from list of usable objects");
                    if(pBlock && *found == pBlock->getBase()->getBlockName()) {
                        pBlock.reset();
                    }
                    validnames->erase(found);
                }
            }
        }
    }

    if(validnames->size() != 0)
        LOG_TRACE_MSG("First validname of " << validnames->size() << " is " << validnames->at(0));

    // Last check.  If we need a block and don't have one, bail.
    if(!pBlock && validnames->size() == 0 && blockauth == true) {
        if((*pCmd)->name() != "list_blocks" && (*pCmd)->name() != "gen_blocks") {
            // list_blocks is special because it can still succeed without all authorities.
            // gen_blocks is special because it requires block create but doesn't take a block name.
            if(bogus_objects.size() == 0)
                reply << FAIL << "User has insufficient authority to a required security object or no objects were specified." << DONE;
            else if(bogus_objects.size() == 1) {  // Some blocks either don't exist or no blocks were specified.
                reply << FAIL << "Block";
                BOOST_FOREACH(std::string& curr_block, bogus_objects) {
                    reply << " " << curr_block;
                }
                reply << " does not exist." << DONE;
            } else if(bogus_objects.size() > 1) {
                reply << FAIL << "Blocks";
                BOOST_FOREACH(std::string& curr_block, bogus_objects) {
                    reply << " " << curr_block;
                }
                reply << " do not exist." << DONE;
            }
            return MMCSCommandProcessorStatus::CMD_INVALID;
        }
    }

    // If we have pBlock and that's the only object,
    // and the command relies entirely on the selected block as its object,
    // then we don't bother with validnames at all.
    if(pBlock && validnames->size() == 1 && // Only one validname
       pBlock->getBase()->getBlockName() == validnames->at(0) &&
       (*pCmd)->attributes().requiresObjNames() != true) {
        LOG_TRACE_MSG("BlockController is our only security object.  Don't bother with name list");
        validnames->clear();
    }

    return MMCSCommandProcessorStatus::procstat(0);
}
#endif

/*!
** validate() - check parameters for MMCS commands
** @param cmdStr      the command string as parsed by parse()
** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
** @param pController the ConsoleController object that the command is to work on
** @param pTarget     (output) the BlockControllerTarget list that the command is to work on
** @param pCmd        (output) the MMCSCommand object to be executed
** @returns           execution status, refer the MMCSCommandProcessor::status
*/
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::validate(deque<string>& cmdStr,
			       MMCSCommandReply& reply,
			       ConsoleController* pController,
			       BlockControllerTarget** pTarget,
			       MMCSCommand** pCmd,
                               std::vector<std::string>* validnames)
{
    BlockHelperPtr pBlock;        // selected BlockController
    *pTarget = NULL;                // initial return value for pTarget
    *pCmd = NULL;                   // initial return value for pCmd
    bool targetSpec_found = false;  // did the command contain a target specification?
    reply.reset();                  // make sure reply doesn't contain good status

    // validate input
    if (cmdStr.size() == 0) {
        // called with no command
        reply << FAIL << "command?" << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    }

    if (pController == NULL) {
        reply << FAIL << "internal error: invalid ConsoleController" << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    }

    // return a pointer to the BlockController selected by the ConsoleController
    pBlock = pController->getBlockHelper();

    // check for target specification
    string targetSpec;
    if (cmdStr.front().c_str()[0] == '{') {
        targetSpec_found = true;
        targetSpec = cmdStr.front();
        cmdStr.pop_front();		// remove target spec from command string

        if (cmdStr.size() == 0) {
            // called with no command
            reply << FAIL << "command?" << DONE;
            return MMCSCommandProcessorStatus::CMD_INVALID;
        }

        if (cmdStr.front().c_str()[0] == '.') {
            // We've also got a cpu spec
            targetSpec += cmdStr.front();
            cmdStr.pop_front();
        }

        if (cmdStr.size() == 0) {
            // called with no command
            reply << FAIL << "command?" << DONE;
            return MMCSCommandProcessorStatus::CMD_INVALID;
        }

        if (targetSpec.at(targetSpec.length() - 1) != '}') {
            reply << FAIL << "invalid target" << DONE;
            return MMCSCommandProcessorStatus::CMD_INVALID;
        }
    } else {
        targetSpec = "{*}";
    }

    // search for the command object for this command
    string cmdName = cmdStr.front();

    MMCSCommandMap::iterator mmcsCommandIter = _mmcsCommands->find(cmdName);

    if (mmcsCommandIter == _mmcsCommands->end()) {
        if (targetSpec_found) {
            cmdStr.push_front(targetSpec);
        }
        return MMCSCommandProcessorStatus::CMD_NOT_FOUND;
    }

    *pCmd = mmcsCommandIter->second; // get the command object

    if((*pCmd)->attributes().externalCommand() == false)
        cmdStr.pop_front();	    // remove command name from command string

    // check that command requirements are satisfied
    if ((*pCmd)->attributes().requiresBlock() && pBlock == NULL) {
        reply << FAIL << "block not selected" << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    } else if ((*pCmd)->attributes().requiresConnection() && (pBlock == NULL || (pBlock)->getBase()->isConnected() == false)) {
        reply << FAIL << "block not connected" << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    }
    // if((*pCmd)->attributes().requiresBlock() && pBlock) {
    //     if(pBlock->getBase()->hardWareAccessBlocked()) {
    //         // Important, non-security related check.  If there's a dead subnet_mc associated
    //         // with this block's hardware, then fail the command.
    //         reply << FAIL << "SubnetMc managing hardware for this block is temporarily unavailable." << DONE;
    //         return MMCSCommandProcessorStatus::CMD_INVALID;
    //     }
    // }
    // build target specification
    if ((*pCmd)->attributes().requiresTarget()) {
        if (pBlock != NULL) {
            *pTarget = new BlockControllerTarget(pBlock->getBase(), targetSpec, reply);
            if (reply.getStatus() < MMCSCommandReply::STATUS_OK) {
                return MMCSCommandProcessorStatus::CMD_INVALID;
            }
        } else {
            reply << FAIL << "block not selected" << DONE;
            return MMCSCommandProcessorStatus::CMD_INVALID;
        }
    }

    if((*pCmd)->checkArgs(cmdStr) == false) {
        reply << FAIL << "args? " << (*pCmd)->get_usage() << DONE;
        return MMCSCommandProcessorStatus::CMD_INVALID;
    }

#ifndef WITH_SECURITY
    return MMCSCommandProcessorStatus::procstat(0);
#else

    return validate_security(cmdStr, reply,
                             pController,
			     pCmd, cmdName, validnames);
#endif
}


/*!
** execute() - Perform specific MMCS command
** @param cmdStr      the command string as parsed by parse()
** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
** @param pController the ConsoleController object that the command is to work on
** @returns           execution status, refer the MMCSCommandProcessor::status
*/
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::execute(deque<string> cmdStr,
			      MMCSCommandReply& reply,
			      ConsoleController* pController)
{
    BlockControllerTarget* pTarget = NULL;
    MMCSCommand* pCmd = NULL;

    // validate input
    std::vector<std::string> validnames;  // list of object names the command can legally use.
    MMCSCommandProcessorStatus::procstat status = validate(cmdStr,reply,pController,&pTarget,&pCmd, &validnames);

    // execute the command
    if (status != MMCSCommandProcessorStatus::CMD_INVALID)
	status = invokeCommand(cmdStr, reply, pController, pTarget, pCmd, status, validnames);

    // clean up
    if (pTarget)
	delete pTarget;

    // log errors
    if (reply.getStatus() == MMCSCommandReply::STATUS_NOT_SET)
	reply << FAIL << "no reply from command" << DONE;
    if (_logFailures && reply.getStatus() < 0) {
	if (pCmd != NULL) {
	    LOG_ERROR_MSG(pCmd->name() << ": " << reply.str(true,0));
	} else {
	    LOG_ERROR_MSG("unknown command: " << reply.str(true,0));
	}
    }

    return status;
}


/*!
** execute() - Perform specific MMCS command
** @param cmdName     the command name
** @param cmdArgs     the command arguments
** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
** @param pController the ConsoleController object that the command is to work on
** @returns           execution status, refer the MMCSCommandProcessor::status
*/
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::execute(string cmdName,
			      deque<string> cmdArgs,
			      MMCSCommandReply& reply,
			      ConsoleController* pController)
{
    cmdArgs.push_front(cmdName);
    return execute(cmdArgs, reply, pController);
}


/*!
** invokeCommand() - Call the command processor for the command
** @param cmdStr      the command string as parsed by parse()
** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
** @param pController the ConsoleController object that the command is to work on
** @param pTarget     the BlockControllerTarget list that the command is to work on
** @param pCmd        the MMCSCommand object to be executed
** @param status      the validation status, refer to MMCSCommandProcessor::status
** @returns           execution status, refer to MMCSCommandProcessor::status
*/
MMCSCommandProcessorStatus::procstat
MMCSCommandProcessor::invokeCommand(deque<string> cmdStr,
				    MMCSCommandReply& reply,
				    ConsoleController* pController,
				    BlockControllerTarget*  pTarget,
				    MMCSCommand*  pCmd,
				    MMCSCommandProcessorStatus::procstat status,
                                    std::vector<std::string>& validnames)
{
    // execute the command
    if (status == 0)
    {
	try
	{
	    status = MMCSCommandProcessorStatus::CMD_EXECUTED;
	    pCmd->execute(cmdStr, reply, pController, pTarget);
	}
	catch (exception &e)
	{
	    reply << ABORT << e.what() << DONE;
	}
    }
    else if (status == MMCSCommandProcessorStatus::CMD_NOT_FOUND)
      reply << FAIL << "command not found" << DONE;
    return status;
}

#ifdef WITH_DB
/*!
** Class: MMCSConsoleCommandProcessor
*/

/*!
** invokeCommand() - Call the command processor for the command
** @param cmdStr      the command string as parsed by parse()
** @param reply       the command output stream. Refer to class MMCSCommandProcessorReply
** @param pController the ConsoleController object that the command is to work on
** @param pTarget     the BlockControllerTarget list that the command is to work on
** @param pCmd        the MMCSCommand object to be executed
** @param status      the validation status, refer to MMCSCommandProcessor::status
** @returns           execution status, refer to MMCSCommandProcessor::status
*/
MMCSCommandProcessorStatus::procstat
MMCSConsoleCommandProcessor::invokeCommand(deque<string> cmdStr,
					   MMCSCommandReply& reply,
					   ConsoleController* pController,
					   BlockControllerTarget*  pTarget,
					   MMCSCommand*  pCmd,
					   MMCSCommandProcessorStatus::procstat status,
                                           std::vector<std::string>& validnames)
{
    // execute the command
    if (status == 0)
    {
	try
	{
	    status = MMCSCommandProcessorStatus::CMD_EXECUTED;
	    pCmd->execute(cmdStr, reply, pController, pTarget);
	}
	catch (exception &e)
	{
	    reply << ABORT << e.what() << DONE;
	}
    }
    else if (status == MMCSCommandProcessorStatus::CMD_EXTERNAL) {
        static_cast<MMCSExternalCommand*>(pCmd)->execute(cmdStr, reply, pController, pTarget);
        //        executeExternal(cmdStr, reply, pController, pTarget);
    }
    else if (status == MMCSCommandProcessorStatus::CMD_NOT_FOUND)
    {
	// if the command is not available locally,
	// try to forward the command to the mmcs server
	if (pController->getConsolePort() != NULL)
	{
	    status = execute("mmcs_server_cmd", cmdStr, reply, pController);
	    if (status == MMCSCommandProcessorStatus::CMD_NOT_FOUND) // prevent infinite loop
	    {
		status = MMCSCommandProcessorStatus::CMD_INVALID;
		reply << FAIL << "Internal failure: mmcs_server_cmd is missing" << DONE;
	    }
	}
        else
          reply << FAIL << "lost connection to mmcs_server;use mmcs_server_connect to reconnect" << DONE;
    }

    return status;
}

MMCSServerCommandProcessor::MMCSServerCommandProcessor(MMCSCommandMap* mmcsCommands) :
    MMCSCommandProcessor(mmcsCommands)
{

}

MMCSCommandProcessorStatus::procstat
MMCSServerCommandProcessor::invokeCommand(deque<string> cmdStr,
					  MMCSCommandReply& reply,
					  ConsoleController* pController,
					  BlockControllerTarget*  pTarget,
					  MMCSCommand*  pCmd,
					  MMCSCommandProcessorStatus::procstat status,
                                          std::vector<std::string>& validnames)
{
    // execute the command
    if (status == 0)
    {
	try
	{
	    status = MMCSCommandProcessorStatus::CMD_EXECUTED;

            if(!validnames.empty())// && pCmd->attributes().requiresBlock() == false)
                pCmd->execute(cmdStr, reply, (DBConsoleController*) pController, pTarget, &validnames);
            else
                pCmd->execute(cmdStr, reply, (DBConsoleController*) pController, pTarget);
	}
	catch (exception &e)
	{
	    reply << ABORT << e.what() << DONE;
	}
    }
    else if (status == MMCSCommandProcessorStatus::CMD_NOT_FOUND)
    {
	reply << FAIL << "command not found" << DONE;
    }

    return status;
}

MMCSCommandProcessorStatus::procstat
MMCSServerCommandProcessor::validate(
            std::deque<std::string>& cmdStr,
            MMCSCommandReply& reply,
            ConsoleController* pController,
            BlockControllerTarget** pTarget,
            MMCSCommand** pCmd,
            std::vector<std::string>* validnames)
{
    return MMCSCommandProcessor::validate(cmdStr, reply, pController, pTarget, pCmd, validnames);
}

void
MMCSConsoleCommandProcessor::executeExternal(deque<string>& cmdStr,
                                             MMCSCommandReply& reply,
                                             ConsoleController* pController,
                                             BlockControllerTarget* pTarget)
{
    // Find the command and call its execute with the args.
}

#endif
