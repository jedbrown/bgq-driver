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

#include "MMCSCommandProcessor.h"

#ifndef BG_CONSOLE
#include "server/BlockControllerBase.h"
#include "server/BlockControllerTarget.h"
#include "server/BlockHelper.h"
#include "server/ras.h"
#endif

#include "common/ConsoleController.h"

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#ifdef MMCS_SERVER
#ifdef WITH_DB
#include "server/command/GrantBlockAuthority.h"
#include "server/command/RevokeBlockAuthority.h"
#include "server/command/ListBlockAuthority.h"
#include "server/command/Allocate.h"
#include "server/command/AllocateBlock.h"
#include "server/command/DeselectBlock.h"
#include "server/command/Free.h"
#include "server/command/FreeBlock.h"
#include "server/command/FreeAll.h"
#include "server/command/ListBlocks.h"
#include "server/command/ListSelectedBlock.h"
#include "server/command/ListUsers.h"
#include "server/command/RebootNodes.h"
#include "server/command/Redirect.h"
#include "server/command/RedirectBlock.h"
#include "server/command/SelectBlock.h"
#include "server/command/StartHwPolling.h"
#include "server/command/StopHwPolling.h"
#include "server/command/ListHwPolling.h"
#include "server/command/WaitBoot.h"
#include "server/command/AddBootOption.h"
#include "server/command/DumpBlock.h"
#include "server/command/CopyBlock.h"
#include "server/command/CreateNodeConfig.h"
#include "server/command/Delete.h"
#include "server/command/GenIoBlock.h"
#include "server/command/GenMidplaneBlock.h"
#include "server/command/GenBlock.h"
#include "server/command/GenBlocks.h"
#include "server/command/GenFullBlock.h"
#include "server/command/GenSmallBlock.h"
#include "server/command/GetBlockInfo.h"
#include "server/command/List.h"
#include "server/command/ListMidplanes.h"
#include "server/command/ListIo.h"
#include "server/command/ListIoLinks.h"
#include "server/command/DumpMachine.h"
#include "server/command/BootBlock.h"
#include "server/command/SetBlockInfo.h"
#include "server/command/SetBootOptions.h"
#include "server/command/Sql.h"
#include "server/command/KillMidplaneJobs.h"
#include "server/command/CompleteIoService.h"
#include "server/command/RefreshConfig.h"
#include "server/command/Status.h"
#endif // end WITH_DB
#include "common/command/Help.h"
#include "server/command/ShowBarrier.h"
#include "server/command/Connect.h"
#ifndef WITH_DB
       // these are mmcs_lite commands
#include "lite/command/CreateBlock.h"
#include "lite/MMCSCommand_killjob.h"
#include "lite/MMCSCommand_lite.h"
#include "lite/MMCSCommand_runjob.h"
#include "lite/MMCSCommand_waitjob.h"
#include "console/command/Sleep.h"
#include "console/command/SubShell.h"
#include "console/command/RedirectInput.h"
#include "lite/command/DumpBlock.h"
#include "lite/command/DumpMachine.h"
#include "lite/command/Quit.h"
#endif // end !WITH_DB
#include "server/command/DiagWait.h"
#include "server/command/Disconnect.h"
#include "server/command/DumpPersonalities.h"
#include "server/command/GetBlockSize.h"
#include "server/command/Locate.h"
#include "server/command/LogLevel.h"
#include "server/command/McserverClients.h"
#include "server/command/McserverStatus.h"
#include "server/command/BlockStatus.h"
#include "server/command/Sysrq.h"
#include "server/command/Version.h"
#include "server/command/WriteCon.h"
#include "server/command/Wc.h"
#endif // end MMCS_SERVER
#ifdef BG_CONSOLE
#include "console/command/Comment.h"
#include "console/command/Quit.h"
#include "console/command/Sleep.h"
#include "console/command/SubShell.h"
#include "console/command/RedirectInput.h"
#include "console/command/DumpMachine.h"
#include "console/command/Redirect.h"
#include "console/command/RedirectBlock.h"
#include "console/command/RefreshConfig.h"
#include "console/command/Username.h"
#include "console/command/DumpBlock.h"
#include "common/command/Help.h"
#include "console/command/MmcsServerCmd.h"
#include "console/command/MmcsServerConnect.h"
#endif // end bg_console

#ifdef BG_CONSOLE
#include "console/ExternalCommand.h"
#endif


#ifdef MMCS_SERVER
#ifdef WITH_DB
static const std::string LOGGER_NAME("ibm.mmcs.server.MMCSCommandProcessor");
#else
static const std::string LOGGER_NAME("ibm.mmcs.lite.MMCSCommandProcessor");
#endif
#else
static const std::string LOGGER_NAME("ibm.mmcs.console.MMCSCommandProcessor");
#endif

static log4cxx::LoggerPtr log_logger_(log4cxx::Logger::getLogger( LOGGER_NAME ));

using namespace std;

using mmcs::common::AbstractCommand;
using mmcs::common::ConsoleController;
using mmcs::common::Properties;

#ifdef BG_CONSOLE
using mmcs::console::ExternalCommand;
#endif


namespace {


vector<AbstractCommand*>* allCommands()
{
    vector<AbstractCommand*> *commands = new vector<AbstractCommand*>;

#ifdef MMCS_SERVER
#ifdef WITH_DB
       commands->push_back(mmcs::server::command::GrantBlockAuthority::build());
       commands->push_back(mmcs::server::command::RevokeBlockAuthority::build());
       commands->push_back(mmcs::server::command::ListBlockAuthority::build());
       commands->push_back(mmcs::server::command::Allocate::build());
       commands->push_back(mmcs::server::command::AllocateBlock::build());
       commands->push_back(mmcs::server::command::DeselectBlock::build());
       commands->push_back(mmcs::server::command::Free::build());
       commands->push_back(mmcs::server::command::FreeBlock::build());
       commands->push_back(mmcs::server::command::FreeAll::build());
       commands->push_back(mmcs::server::command::ListBlocks::build());
       commands->push_back(mmcs::server::command::ListSelectedBlock::build());
       commands->push_back(mmcs::server::command::ListUsers::build());
       commands->push_back(mmcs::server::command::RebootNodes::build());
       commands->push_back(mmcs::server::command::Redirect::build());
       commands->push_back(mmcs::server::command::RedirectBlock::build());
       commands->push_back(mmcs::server::command::SelectBlock::build());
       commands->push_back(mmcs::server::command::StartHwPolling::build());
       commands->push_back(mmcs::server::command::StopHwPolling::build());
       commands->push_back(mmcs::server::command::ListHwPolling::build());
       commands->push_back(mmcs::server::command::WaitBoot::build());
       commands->push_back(mmcs::server::command::AddBootOption::build());
       commands->push_back(mmcs::server::command::DumpBlock::build());
       commands->push_back(mmcs::server::command::CopyBlock::build());
       commands->push_back(mmcs::server::command::CreateNodeConfig::build());
       commands->push_back(mmcs::server::command::Delete::build());
       commands->push_back(mmcs::server::command::GenIoBlock::build());
       commands->push_back(mmcs::server::command::GenMidplaneBlock::build());
       commands->push_back(mmcs::server::command::GenBlock::build());
       commands->push_back(mmcs::server::command::GenBlocks::build());
       commands->push_back(mmcs::server::command::GenFullBlock::build());
       commands->push_back(mmcs::server::command::GenSmallBlock::build());
       commands->push_back(mmcs::server::command::GetBlockInfo::build());
       commands->push_back(mmcs::server::command::List::build());
       commands->push_back(mmcs::server::command::ListMidplanes::build());
       commands->push_back(mmcs::server::command::ListIo::build());
       commands->push_back(mmcs::server::command::ListIoLinks::build());
       commands->push_back(mmcs::server::command::DumpMachine::build());
       commands->push_back(mmcs::server::command::BootBlock::build());
       commands->push_back(mmcs::server::command::SetBlockInfo::build());
       commands->push_back(mmcs::server::command::SetBootOptions::build());
       commands->push_back(mmcs::server::command::Sql::build());
       commands->push_back(mmcs::server::command::KillMidplaneJobs::build());
       commands->push_back(mmcs::server::command::CompleteIoService::build());
       commands->push_back(mmcs::server::command::RefreshConfig::build());
       commands->push_back(mmcs::server::command::Status::build());
#endif // end WITH_DB
       commands->push_back(mmcs::common::command::Help::build());
       commands->push_back(mmcs::server::command::ShowBarrier::build());
       commands->push_back(mmcs::server::command::Connect::build());
#ifndef WITH_DB
       // these are mmcs_lite commands
       commands->push_back(mmcs::lite::command::CreateBlock::build());
       commands->push_back(mmcs::lite::MMCSCommand_bringup::build());
       commands->push_back(mmcs::lite::MMCSCommand_dump_ras::build());
       commands->push_back(mmcs::lite::MMCSCommand_gen_block::build());
       commands->push_back(mmcs::lite::MMCSCommand_gen_io_block::build());
       commands->push_back(mmcs::lite::MMCSCommand_killjob::build());
       commands->push_back(mmcs::lite::MMCSCommand_waitjob::build());
       commands->push_back(mmcs::lite::MMCSCommand_runjob::build());
       commands->push_back(mmcs::lite::MMCSCommand_show_envs::build());
       commands->push_back(mmcs::lite::MMCSCommand_wait_for_terminate::build());
       commands->push_back(mmcs::lite::MMCSCommand_d::build());
       commands->push_back(mmcs::console::command::Sleep::build());
       commands->push_back(mmcs::console::command::SubShell::build());
       commands->push_back(mmcs::console::command::RedirectInput::build());
       commands->push_back(mmcs::lite::MMCSCommand_delete_block::build());
       commands->push_back(mmcs::lite::MMCSCommand_boot_block::build());
       commands->push_back(mmcs::lite::command::DumpBlock::build());
       commands->push_back(mmcs::lite::command::DumpMachine::build());
       commands->push_back(mmcs::lite::command::Quit::build());
#endif // end !WITH_DB
       commands->push_back(mmcs::server::command::DiagWait::build());
       commands->push_back(mmcs::server::command::Disconnect::build());
       commands->push_back(mmcs::server::command::DumpPersonalities::build());
       commands->push_back(mmcs::server::command::GetBlockSize::build());
       commands->push_back(mmcs::server::command::Locate::build());
       commands->push_back(mmcs::server::command::LogLevel::build());
       commands->push_back(mmcs::server::command::McserverClients::build());
       commands->push_back(mmcs::server::command::McserverStatus::build());
       commands->push_back(mmcs::server::command::BlockStatus::build());
       commands->push_back(mmcs::server::command::Sysrq::build());
       commands->push_back(mmcs::server::command::Version::build());
       commands->push_back(mmcs::server::command::WriteCon::build());
       commands->push_back(mmcs::server::command::Wc::build());
#endif // end MMCS_SERVER
#ifdef BG_CONSOLE
       commands->push_back(mmcs::console::command::Comment::build());
       commands->push_back(mmcs::console::command::Quit::build());
       commands->push_back(mmcs::console::command::Sleep::build());
       commands->push_back(mmcs::console::command::SubShell::build());
       commands->push_back(mmcs::console::command::RedirectInput::build());
       commands->push_back(mmcs::console::command::DumpMachine::build());
       commands->push_back(mmcs::console::command::Redirect::build());
       commands->push_back(mmcs::console::command::RedirectBlock::build());
       commands->push_back(mmcs::console::command::RefreshConfig::build());
       commands->push_back(mmcs::console::command::Username::build());
       commands->push_back(mmcs::console::command::DumpBlock::build());
       commands->push_back(mmcs::common::command::Help::build());
       commands->push_back(mmcs::console::command::MmcsServerCmd::build());
       commands->push_back(mmcs::console::command::MmcsServerConnect::build());
#endif // end bg_console
    return commands;
}

} // anonymous namespace

namespace mmcs {

// Static initialization
EnforcerPtr MMCSCommandProcessor::_command_enforcer;

void
MMCSCommandProcessor::logCommand(const string& cmd, const deque<string>& args)
{
    if (cmd.size() > 0 || args.size() > 0) {
        ostringstream oss;
        if (cmd.size() != 0)
            oss << cmd << " ";
        for (unsigned i = 0; i < args.size(); ++i)
            oss << args[i] << " ";
        LOG_INFO_MSG( oss.str() );
    }
}

void
MMCSCommandProcessor::logCommand(const deque<string>& cmd_and_args)
{
    if (cmd_and_args.size() > 0) {
        ostringstream oss;
        for (unsigned i = 0; i < cmd_and_args.size(); ++i)
            oss << cmd_and_args[i] << " ";
        LOG_INFO_MSG( oss.str() );
    }
}

MMCSCommandMap*
MMCSCommandProcessor::createCommandMap(
        AbstractCommand::Attributes& attr,
        AbstractCommand::Attributes& mask,
        Properties::Map* externalCmds
        )
{
    // Initialize the command map
    MMCSCommandMap* cmdMap = new MMCSCommandMap;

#ifdef WITH_DB // No security in mmcs_lite
    if (attr.bgConsoleCommand() != true) {
        // Security in the server, not the client
        EnforcerPtr ep(new hlcs::security::Enforcer(Properties::getProperties() ));
        MMCSCommandProcessor::_command_enforcer = ep;
    }
#endif

    // Create a list of all MMCS commands
    vector<AbstractCommand*>* cmds = allCommands();

#ifdef BG_CONSOLE
    if (externalCmds != 0) {
        for (Properties::Map::const_iterator it = externalCmds->begin(); it != externalCmds->end(); ++it) {
            AbstractCommand::Attributes cmdattr;
            cmdattr.externalCommand(true);
            ExternalCommand* externalcmd = new ExternalCommand((*it).first.c_str(), (*it).second.c_str(), cmdattr);

            if (Properties::getProperty("USER").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(common::USER);
            else if (Properties::getProperty("ADMIN").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(common::ADMIN);
            else if (Properties::getProperty("SPECIAL").find(externalcmd->name()) != std::string::npos)
                externalcmd->attributes().helpCategory(common::SPECIAL);
            else
                externalcmd->attributes().helpCategory(common::DEFAULT);
            externalcmd->attributes().externalCommand(true);
            externalcmd->attributes().bgConsoleCommand(true);
            cmds->push_back(externalcmd);
        }
    }
#else
    (void)externalCmds;
#endif

    // Check each command for the attributes specified by the caller.
    // If they match, add them to the command map
    while (!cmds->empty()) {
        AbstractCommand* cmd = cmds->back(); // Get the last element in the list
        cmds->pop_back(); // Remove it from the list

        // Check the command attributes against the selection criteria
        if (
                (mask.requiresBlock() && cmd->attributes().requiresBlock() != attr.requiresBlock()) ||
                (mask.requiresTarget() && cmd->attributes().requiresTarget() != attr.requiresTarget()) ||
                (mask.requiresConnection() && cmd->attributes().requiresConnection() != attr.requiresConnection()) ||
                (mask.internalCommand() && cmd->attributes().internalCommand() != attr.internalCommand()) ||
                (mask.bgConsoleCommand() && cmd->attributes().bgConsoleCommand() != attr.bgConsoleCommand()) ||
                (mask.mmcsServerCommand() && cmd->attributes().mmcsServerCommand() != attr.mmcsServerCommand()) ||
                (mask.mmcsLiteCommand() && cmd->attributes().mmcsLiteCommand() != attr.mmcsLiteCommand())
           )
        {
            // LOG_TRACE_MSG("Deleting command " << cmd->name() << " from the command map.");
            delete cmd; // This command doesn't match the selection criteria
        }
        else  {
            if (!mask.externalCommand() && cmd->attributes().externalCommand() == true) {
                // If we did not specify external commands and this is one, delete it. (i.e. in mmcs_server).
                // LOG_TRACE_MSG("Deleting internal command " << cmd->name() << " from the command map.");
                delete cmd; // this command doesn't match the selection criteria
            }

            // LOG_TRACE_MSG("Adding command " << cmd->name() << " to the command map.");
            (*cmdMap)[cmd->name()] = cmd; // add the command to the command map
        }
    }

    // Return the commands matching the attributes specified by the caller
    return cmdMap;
}

deque<string>
MMCSCommandProcessor::parseCommand(const std::string &rStr)
{
    deque<string> words;
    const string szDelimiters = " \n";

    std::string::size_type lastPos(rStr.find_first_not_of(szDelimiters, 0));
    std::string::size_type pos(rStr.find_first_of(szDelimiters, lastPos));
    while (std::string::npos != pos || std::string::npos != lastPos) {
        words.push_back(rStr.substr(lastPos, pos - lastPos));
        lastPos = rStr.find_first_not_of(szDelimiters, pos);
        pos = rStr.find_first_of(szDelimiters, lastPos);
    }
    return words;
}

string
MMCSCommandProcessor::unparseCommand(const std::deque<string>& cmdStr)
{
    string str;
    if (cmdStr.size() > 0) {
        for (unsigned i = 0; i < cmdStr.size(); ++i)
            str.append(cmdStr[i]).append(" ");
    }
    if (str.size() > 0 && str[str.size()-1] != '\n') {
        str.append("\n");
    }
    return str;
}

#ifndef WITH_DB
procstat
MMCSCommandProcessor::validate_security(deque<string>& ,//cmdStr,
                                        mmcs_client::CommandReply& ,//reply,
                                        ConsoleController* ,//pController,
                                        AbstractCommand** ,//pCmd,
                                        std::string& ,//cmdName,
                                        std::vector<std::string>* /*validnames*/) {
    return procstat(0);
}
#else
procstat
MMCSCommandProcessor::validate_security(deque<string>& cmdStr,
                                        mmcs_client::CommandReply& reply,
                                        ConsoleController* pController,
                                        AbstractCommand** pCmd,
                                        std::string& cmdName,
                                        std::vector<std::string>* validnames) {

    if (_bg_console) { // Security happens in the server, the console skips it.
        return procstat(0);
    }

    // We're doing security.  Here are a few ground rules:
    // 1)  External commands' servers do their own security. We don't check it.
    // 2)  Internal commands have their own security requirements hard coded and
    //     stored in the command objects.
    // 3)  There are certain internal or trivial commands that don't access security
    //     objects. They're marked and we skip them.
    // 4)  bgadmin is a special authority.
    // 5)  We can have multiple authorities for security objects in a single command.
    //     They are stored in pairs. We need to loop through the pairs and make sure
    //     all objects are valid.
    // 6)  If we have a block object, we're going to use it as a block auth object.
    // 7)  If a block object is ALL we have, then we don't need to worry about sending
    //     a list of object names to the command.
    // 8)  Some commands are able to operate on objects in addition to the selected block
    //     object. They have a mechanism, getBlockObjects(), to return a list of those
    //     names when given a command string.
    // 9)  If a command returns a list of objects, we DON'T use the selected block
    //     and just validate the results of getBlockObjects() and send it back to them.
    // 10) An object may be validated for one action and invalidated for another.
    //     If a command requires both, the object may NOT be used.
    // 11) Hardware auth is all or nothing.

    server::BlockHelperPtr pBlock = pController->getBlockHelper();
    if ((*pCmd)->attributes().externalCommand() == true) {
        // We can return without security checking because external servers do their own.
        return CMD_EXTERNAL;
    }

    // Special internal commands without security objects.
    if ((*pCmd)->attributes().getInternalAuth() == true) {
        return procstat(0);
    }

    bool blockauth = false;  // Set this if we need authorization to a block

    const std::vector<AbstractCommand::Attributes::AuthPair>* aps = (*pCmd)->attributes().getAuthPairs();
    std::vector<std::string> bogus_objects;
    if (aps->size() == 0 && (*pCmd)->attributes().getBgAdminAuth() != true ) {
        reply << mmcs_client::FAIL;
        reply << "Unexpected internal error. Command " << cmdName << " invalid. Has no security data.";
        reply << mmcs_client::DONE;
        return CMD_INVALID;
    }

    if (pController->getUserType() == CxxSockets::Administrator) {
        LOG_TRACE_MSG("bgadmin authority granted");
        return CMD_EXECUTED;
    }

    if ((*pCmd)->attributes().getBgAdminAuth()) {
        RasEventImpl event(MMCSOps_0301);
        event.setDetail("USER", pController->getUser().getUser());
        event.setDetail("COMMAND", (*pCmd)->name());
        RasEventHandlerChain::handle(event);
        reply << mmcs_client::FAIL << event.getDetail(RasEvent::MESSAGE) << mmcs_client::DONE;
        BGQDB::putRAS(event);
        return CMD_INVALID;
    }

    // We have auth pairs
    for (
            std::vector<AbstractCommand::Attributes::AuthPair>::const_iterator it = aps->begin();
            it != aps->end();
            ++it
        )
    {
        const AbstractCommand::Attributes::AuthPair ap = *it;
        std::vector<std::string> objnames;
        if (ap.first == hlcs::security::Object::Block) {
            blockauth = true;
            if (pBlock) {
                objnames.push_back(pBlock->getBase()->getBlockName());
            }

            // See if the command can parse out some block IDs. It'll only do that
            // if it has to operate on something other than the selected block.
            // Note that the following will nuke anything stuffed in 'objnames'
            // in the if check above. It's supposed to do that. See rule #9 above.
            std::vector<std::string> t = (*pCmd)->getBlockObjects(cmdStr, (server::DBConsoleController*)(pController));
            bool good = false;
            if (!t.empty() && (*pCmd)->attributes().requiresBlock() == false) {
                // This means the command has at least one arg.

                // Copy_block needs to do its own special authorization. This lets it happen.
                procstat stat;
                const boost::shared_ptr<hlcs::security::Enforcer> e = MMCSCommandProcessor::_command_enforcer;
                if ((*pCmd)->doSpecialAuths(t, e, stat, pController->getUser()) == true) {
                    // LOG_TRACE_MSG("Special Auths done");
                    for (unsigned i = 0; i < t.size(); ++i) {
                        validnames->push_back(t[i]);
                    }
                    if (stat == CMD_INVALID) {
                        t.clear(); // No usable objects!
                        reply << mmcs_client::FAIL << "User has insufficient authority to necessary security objects." << mmcs_client::DONE;
                        return stat;
                    }
                } else {
                    for (std::vector<std::string>::const_iterator it = t.begin(); it != t.end(); ++it) {
                        BGQDB::BlockInfo binfo;
                        const bool foundindb = (BGQDB::getBlockInfo(*it, binfo) == BGQDB::OK);
                        const bool createaction = (ap.second == hlcs::security::Action::Create);
                        if ((foundindb && !createaction) || (!foundindb && createaction)) {
                            // If it's create, we'll call it good because it won't be in the db any way.
                            // LOG_TRACE_MSG("Got a valid block from command " << *it);
                            good = true;
                        } else if (!foundindb && !createaction) {
                            // LOG_DEBUG_MSG("Block is not in database and action is not create.");
                            if (std::find(bogus_objects.begin(), bogus_objects.end(), *it) == bogus_objects.end()) {
                                bogus_objects.push_back(*it);
                            }
                            good = false;
                        } else if (foundindb && createaction) {
                            reply << mmcs_client::FAIL << "Cannot create block already created." << mmcs_client::DONE;
                            return CMD_INVALID;
                        }
                    }
                }
                if (good || (*pCmd)->attributes().requiresObjNames()) {
                    // At least one name in the arg list is a valid block and we don't
                    // need a selected block so we'll use the arg list.
                    objnames = t;
                }
            }

            // Found no objnames and we don't require a =selected= block.
            if (objnames.size() == 0 && (*pCmd)->attributes().requiresBlock() == true) {
                // If there's no block object and we need one, and the command doesn't
                // even know about any, we better not even attempt to do security.
                reply << mmcs_client::FAIL;
                reply << "Unexpected internal error. Command security requires block security object but no block object exists.";
                reply << mmcs_client::DONE;
                return CMD_INVALID;
            }

        } else if (ap.first == hlcs::security::Object::Hardware) {
            objnames.clear(); // Empty string.  Hardware is all or nothing.
            try {
                const hlcs::security::Object hw(hlcs::security::Object::Hardware, "all hardware");
                if ( !MMCSCommandProcessor::_command_enforcer->validate(hw, ap.second, pController->getUser()) ) {
                    reply << mmcs_client::FAIL;
                    reply << "User " << pController->getUser().getUser() << " not authorized to hardware.";
                    reply << mmcs_client::DONE;
                    return CMD_INVALID;
                } else {
                    LOG_TRACE_MSG("User " << pController->getUser().getUser() << " authorized to hardware.");
                }
            } catch (const std::runtime_error& d) {
                reply << mmcs_client::FAIL << "Unable to validate object hardware.  " << d.what() << ". "  << (*pCmd)->description() << mmcs_client::DONE;
                return CMD_INVALID;
            } catch (const std::invalid_argument& v) {
                reply << mmcs_client::FAIL << "Unable to validate object hardware.  " << v.what() << ". "  << (*pCmd)->description() << mmcs_client::DONE;
                return CMD_INVALID;
            }
        } else {
            reply << mmcs_client::FAIL;
            reply << "Unexpected internal error. Invalid security object type: " << ap.first;
            reply << mmcs_client::DONE;
            return CMD_INVALID;
        }

        LOG_TRACE_MSG("Command " << cmdName << " wants to operate on " << objnames.size() << " objects.");

        // For each name returned by the command, we have to determine if we can use it.
        std::ostringstream msg;
        msg << "User " << pController->getUser().getUser()
            << " does not have appropriate authority for security object(s)";
        std::vector<std::string> failednames; // fill with names that enforcer disallows
        for (std::vector<std::string>::const_iterator i = objnames.begin(); i != objnames.end(); ++i) {
            LOG_TRACE_MSG("Validating object name " << *i);
            const hlcs::security::Object blockobj(hlcs::security::Object::Block, *i);
            try {
                if ( !MMCSCommandProcessor::_command_enforcer->validate(blockobj, ap.second, pController->getUser()) ) {
                    msg << " " << *i;
                    failednames.push_back(*i);
                } else {
                    LOG_TRACE_MSG("User " << pController->getUser().getUser() << " authorized to " << *i);
                    if (std::find(validnames->begin(), validnames->end(), *i) == validnames->end()) {
                        validnames->push_back(*i); // It's good. Save it.
                    }
                }
            } catch (const std::runtime_error& d) {
                reply << mmcs_client::FAIL << "Unable to validate object " << *i << ".  " << d.what() << ". "  << (*pCmd)->description() << mmcs_client::DONE;
                return CMD_INVALID;
            } catch (const std::invalid_argument& v) {
                reply << mmcs_client::FAIL << "Unable to validate object " << *i << ".  " << v.what() << ". "  << (*pCmd)->description() << mmcs_client::DONE;
                return CMD_INVALID;
            }
        }
        if ( !failednames.empty() ) {
            LOG_DEBUG_MSG( msg.str() );
        }

        // If none of the names the command wants to use are available, fail the command.
        if (validnames->size() == 0 && objnames.size() != 0) {
            reply << mmcs_client::FAIL << "Permission denied for user " << pController->getUser().getUser() << mmcs_client::DONE;
            return CMD_INVALID;
        }

        // Now check to make sure that none of the failed names are in the valid list.
        // This is a subtle issue. If a user needs BOTH B:E and B:R authority to perform
        // an action on that block, then that block name will show up in both validnames and the failednames .
        for (std::vector<std::string>::const_iterator it = failednames.begin(); it != failednames.end(); ++it) {
            const std::vector<std::string>::iterator found = std::find(validnames->begin(), validnames->end(), *it);
            if (found != validnames->end()) {
                // Uh oh. There's a bad one in the good list. Yank it.
                LOG_TRACE_MSG("Removing " << *found << " from list of usable objects");
                if (pBlock && *found == pBlock->getBase()->getBlockName()) {
                    pBlock.reset();
                }
                validnames->erase(found);
            }
        }
    }

    if (validnames->size() != 0) {
        LOG_TRACE_MSG("First validname of " << validnames->size() << " is " << validnames->at(0));
    }

    // Last check. If we need a block and don't have one, bail.
    if (!pBlock && validnames->size() == 0 && blockauth == true) {
        if ((*pCmd)->name() != "list_blocks" && (*pCmd)->name() != "gen_blocks") {
            // list_blocks is special because it can still succeed without all authorities.
            // gen_blocks is special because it requires block create but doesn't take a block name.
            if (bogus_objects.size() == 0)
                reply << mmcs_client::FAIL << "User has insufficient authority to a required security object or no objects were specified." << mmcs_client::DONE;
            else if(bogus_objects.size() == 1) {  // Some blocks either don't exist or no blocks were specified.
                reply << mmcs_client::FAIL << "Block";
                BOOST_FOREACH(std::string& curr_block, bogus_objects) {
                    reply << " " << curr_block;
                }
                reply << " does not exist." << mmcs_client::DONE;
            } else if(bogus_objects.size() > 1) {
                reply << mmcs_client::FAIL << "Blocks";
                BOOST_FOREACH(std::string& curr_block, bogus_objects) {
                    reply << " " << curr_block;
                }
                reply << " do not exist." << mmcs_client::DONE;
            }
            return CMD_INVALID;
        }
    }

    // If we have pBlock and that's the only object, and the command relies entirely
    // on the selected block as its object, then we don't bother with validnames at all.
    if (pBlock && validnames->size() == 1 && // Only one validname
       pBlock->getBase()->getBlockName() == validnames->at(0) &&
       (*pCmd)->attributes().requiresObjNames() != true) {
        // LOG_TRACE_MSG("BlockController is our only security object. Don't bother with name list");
        validnames->clear();
    }

    return procstat(0);
}
#endif

procstat
MMCSCommandProcessor::validate(
        deque<string>& cmdStr,
        mmcs_client::CommandReply& reply,
        ConsoleController* pController,
        server::BlockControllerTarget** pTarget,
        AbstractCommand** pCmd,
        std::vector<std::string>* validnames
        )
{
    server::BlockHelperPtr pBlock;  // selected BlockController
    *pTarget = NULL;                // initial return value for pTarget
    *pCmd = NULL;                   // initial return value for pCmd
    bool targetSpec_found = false;  // did the command contain a target specification?
    reply.reset();                  // make sure reply doesn't contain good status

    // Validate input
    if (cmdStr.size() == 0) {
        // Called with no command
        reply << mmcs_client::FAIL << "command?" << mmcs_client::DONE;
        return CMD_INVALID;
    }

    if (pController == NULL) {
        reply << mmcs_client::FAIL << "Internal error, invalid ConsoleController" << mmcs_client::DONE;
        return CMD_INVALID;
    }

    // Return a pointer to the BlockController selected by the ConsoleController
    pBlock = pController->getBlockHelper();

    // Check for target specification
    string targetSpec;
    if (cmdStr.front().c_str()[0] == '{') {
        targetSpec_found = true;
        targetSpec = cmdStr.front();
        cmdStr.pop_front(); // remove target spec from command string

        if (cmdStr.size() == 0) {
            // Called with no command
            reply << mmcs_client::FAIL << "command?" << mmcs_client::DONE;
            return CMD_INVALID;
        }

        if (cmdStr.front().c_str()[0] == '.') {
            // We've also got a cpu spec
            targetSpec += cmdStr.front();
            cmdStr.pop_front();
        }

        if (cmdStr.size() == 0) {
            // Called with no command
            reply << mmcs_client::FAIL << "command?" << mmcs_client::DONE;
            return CMD_INVALID;
        }

        if (targetSpec.at(targetSpec.length() - 1) != '}') {
            reply << mmcs_client::FAIL << "Invalid target" << mmcs_client::DONE;
            return CMD_INVALID;
        }
    } else {
        targetSpec = "{*}";
    }

    // Search for the command object for this command
    string cmdName = cmdStr.front();

    MMCSCommandMap::const_iterator mmcsCommandIter = _mmcsCommands->find(cmdName);

    if (mmcsCommandIter == _mmcsCommands->end()) {
        if (targetSpec_found) {
            cmdStr.push_front(targetSpec);
        }
        return CMD_NOT_FOUND;
    }

    *pCmd = mmcsCommandIter->second; // get the command object

    if ((*pCmd)->attributes().externalCommand() == false) {
        cmdStr.pop_front(); // remove command name from command string
    }

    // Check that command requirements are satisfied
    if ((*pCmd)->attributes().requiresBlock() && pBlock == NULL) {
        reply << mmcs_client::FAIL << "Block not selected" << mmcs_client::DONE;
        return CMD_INVALID;
#ifndef BG_CONSOLE
    } else if ((*pCmd)->attributes().requiresConnection() && (pBlock == NULL || (pBlock)->getBase()->isConnected() == false)) {
        reply << mmcs_client::FAIL << "Block not connected" << mmcs_client::DONE;
        return CMD_INVALID;
#endif
    }
    // if((*pCmd)->attributes().requiresBlock() && pBlock) {
    //     if(pBlock->getBase()->hardWareAccessBlocked()) {
    //         // Important, non-security related check.  If there's a dead subnet_mc associated
    //         // with this block's hardware, then fail the command.
    //         reply << mmcs_client::FAIL << "SubnetMc managing hardware for this block is temporarily unavailable." << mmcs_client::DONE;
    //         return CMD_INVALID;
    //     }
    // }

#ifndef BG_CONSOLE
    // Build target specification
    if ((*pCmd)->attributes().requiresTarget()) {
        if (pBlock != NULL) {
            *pTarget = new server::BlockControllerTarget(pBlock->getBase(), targetSpec, reply);
            if (reply.getStatus() < mmcs_client::CommandReply::STATUS_OK) {
                return CMD_INVALID;
            }
        } else {
            reply << mmcs_client::FAIL << "Block not selected" << mmcs_client::DONE;
            return CMD_INVALID;
        }
    }
#endif

    if ((*pCmd)->checkArgs(cmdStr) == false) {
        reply << mmcs_client::FAIL << "args? " << (*pCmd)->usage() << mmcs_client::DONE;
        return CMD_INVALID;
    }

#ifndef WITH_DB
    return procstat(0);
#else
    return validate_security(
            cmdStr,
            reply,
            pController,
            pCmd,
            cmdName,
            validnames
            );
#endif
}

procstat
MMCSCommandProcessor::execute(
        deque<string> cmdStr,
        mmcs_client::CommandReply& reply,
        ConsoleController* pController
        )
{
    server::BlockControllerTarget* pTarget = NULL;
    AbstractCommand* pCmd = NULL;

    // Validate input
    std::vector<std::string> validnames;  // list of object names the command can legally use.
    procstat status = validate(cmdStr,reply,pController,&pTarget,&pCmd, &validnames);

    // Execute the command
    if (status != CMD_INVALID) {
        status = invokeCommand(cmdStr, reply, pController, pTarget, pCmd, status, validnames);
    }

#ifndef BG_CONSOLE
    // Clean up
    if (pTarget) {
        delete pTarget;
    }
#endif

    // Log errors
    if (reply.getStatus() == mmcs_client::CommandReply::STATUS_NOT_SET)
        reply << mmcs_client::FAIL << "No reply from command" << mmcs_client::DONE;
    if (_logFailures && reply.getStatus() < 0) {
        if (pCmd != NULL) {
            LOG_ERROR_MSG(pCmd->name() << ": " << reply.str(true,0));
        } else if ( !cmdStr.empty() && cmdStr[0] == "replyformat" ) {
            reply << mmcs_client::OK << mmcs_client::DONE;
            status = procstat(0);
        } else {
            LOG_ERROR_MSG("Unknown command: " << reply.str(true,0));
        }
    }

    return status;
}

procstat
MMCSCommandProcessor::execute(
        string cmdName,
        deque<string> cmdArgs,
        mmcs_client::CommandReply& reply,
        ConsoleController* pController
        )
{
    cmdArgs.push_front(cmdName);
    return execute(cmdArgs, reply, pController);
}

procstat
MMCSCommandProcessor::invokeCommand(
        deque<string> cmdStr,
        mmcs_client::CommandReply& reply,
        ConsoleController* pController,
        server::BlockControllerTarget*  pTarget,
        AbstractCommand*  pCmd,
        procstat status,
        std::vector<std::string>& //validnames
        )
{
    // Execute the command
    if (status == 0) {
        try {
            status = CMD_EXECUTED;
            pCmd->execute(cmdStr, reply, pController, pTarget);
        } catch (const exception& e) {
            reply << mmcs_client::ABORT << e.what() << mmcs_client::DONE;
        }
    } else if (status == CMD_NOT_FOUND) {
        reply << mmcs_client::FAIL << "Command not found" << mmcs_client::DONE;
    }
    return status;
}

} // namespace mmcs
