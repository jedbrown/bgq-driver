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

#include <boost/tokenizer.hpp>
#include <boost/foreach.hpp>
#include "MMCSCommand_inject_ras.h"
#include "MMCSCommandReply.h"
#include "DBBlockController.h"
#include "ConsoleController.h"

#include "ras.h"
#include <ras/include/RasEventImpl.h>
#include <bgq_util/include/string_tokenizer.h>

MMCSCommand_inject_ras*
MMCSCommand_inject_ras::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.mmcsServerCommand(true);             // server command
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(false);           // this is an internal use command
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(SPECIAL);           // 'help special'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new MMCSCommand_inject_ras("inject_ras", "inject_ras", commandAttributes);
}

void
MMCSCommand_inject_ras::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  ConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
//     if (args.size() > 0)
//     {
// 	reply << FAIL << "args? " << usage << DONE;
//         return;
//     }

    BlockHelperPtr pBlock = pController->getBlockHelper();	// get selected block

    // Build up our RAS event.
    // Set some defaults
    string id = "USER_0101";
    string sev = "ERROR";
    string msg = "Default ras injection error";
    string loc = "R00-M0-N00-J00";
    string payload = "dummy";
    string jtag = "J00";
    string cpu = "0";
    string ecid = "0000000000000000000000000000";
    string sn = "0";
    string control = "none";

    for (std::deque<string>::iterator i = args.begin(); i != args.end(); ++i) {
        StringTokenizer tokenizer;
        unsigned int numTokens = tokenizer.tokenize(*i, "=");
        if (numTokens != 2) {
            // bad format
            reply << FAIL << "unsupported argument: " << *i << DONE;
            return;
        }
        if(tokenizer[0] == "id")
            id = tokenizer[1];
        else if(tokenizer[0] == "severity")
            sev = tokenizer[1];
        else if(tokenizer[0] == "message")
            msg = tokenizer[1];
        else if(tokenizer[0] == "location")
            loc = tokenizer[1];
        else if(tokenizer[0] == "payload")
            payload = tokenizer[1];
        else if(tokenizer[0] == "jtag")
            jtag = tokenizer[1];
        else if(tokenizer[0] == "cpu")
            cpu = tokenizer[1];
        else if(tokenizer[0] == "ecid")
            ecid = tokenizer[1];
        else if(tokenizer[0] == "serial")
            sn = tokenizer[1];
	else if(tokenizer[0] == "control_action")
            control = tokenizer[1];
        else {
	  reply << FAIL << "bogus value: " << *i << DONE;
	  return;
        }
    }

    // Make and send our event
    RasEventImpl event(MMCSOps_000E);
    event.setDetail(RasEvent::SEVERITY, sev);
    event.setDetail(RasEvent::MESSAGE, msg);
    event.setDetail(RasEvent::LOCATION, loc);
    event.setDetail(RasEvent::MBOX_PAYLOAD, payload);
    event.setDetail(RasEvent::JTAG_PORT, jtag);
    event.setDetail(RasEvent::CPU, cpu);
    event.setDetail(RasEvent::ECID, ecid);
    event.setDetail(RasEvent::SERIAL_NUMBER, sn);
    event.setDetail(RasEvent::CONTROL_ACTION, control);
    pBlock->processRASMessage(event);

    reply << OK << DONE;
}

void
MMCSCommand_inject_ras::help(deque<string> args,
				   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Inject a RAS message, here is what you can specify"
	  << "; id"
	  << "; severity"
	  << "; message"
          << "; location"
	  << "; payload"
	  << "; jtag"
	  << "; cpu"
	  << "; ecid"
	  << "; serial"
	  << "; control_action"
	  << DONE;
}


MMCSCommand_inject_console*
MMCSCommand_inject_console::build()
{
    MMCSCommandAttributes commandAttributes;
    commandAttributes.requiresBlock(true);             // does require a BlockController object
    commandAttributes.mmcsServerCommand(true);             // server command
    commandAttributes.requiresConnection(false);       // does not require  mc_server connections
    commandAttributes.requiresTarget(false);           // does not require a BlockControllerTarget object
    commandAttributes.internalCommand(false);           // this is an internal use command
    commandAttributes.mmcsLiteCommand(true);
    commandAttributes.helpCategory(SPECIAL);           // 'help special'  will include this command's summary
    MMCSCommandAttributes::AuthPair hardwareexecute(hlcs::security::Object::Hardware, hlcs::security::Action::Execute);
    commandAttributes.addAuthPair(hardwareexecute);
    return new MMCSCommand_inject_console("inject_console", "inject_console", commandAttributes);
}

void
MMCSCommand_inject_console::execute(deque<string> args,
				  MMCSCommandReply& reply,
				  ConsoleController* pController,
				  BlockControllerTarget* pTarget)
{
    BlockHelperPtr pBlock = pController->getBlockHelper();	// get selected block

    // Build up our console message.
    // Set some defaults
    std::string location("");
    unsigned jtag = 0;
    unsigned cpu = 0;
    unsigned thread_id = 0;
    unsigned time_secs = 0;
    unsigned time_usecs = 0;
    std::string msg("Empty console message");

    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    for (std::deque<string>::iterator i = args.begin(); i != args.end(); ++i) {

        StringTokenizer tokenizer;
        unsigned int numTokens = tokenizer.tokenize(*i, "=");
        if (numTokens != 2) {
            // bad format
            reply << FAIL << "unsupported argument: " << *i << DONE;
            return;
        }

        if(tokenizer[0] == "location")
            location = tokenizer[1];
        else if(tokenizer[0] == "jtag")
            jtag = atoi(tokenizer[1].c_str());
        else if(tokenizer[0] == "cpu")
            cpu = atoi(tokenizer[1].c_str());
        else if(tokenizer[0] == "thread_id")
            thread_id = atoi(tokenizer[1].c_str());
        else if(tokenizer[0] == "time_secs")
            time_secs = atoi(tokenizer[1].c_str());
        else if(tokenizer[0] == "time_usecs")
            time_usecs = atoi(tokenizer[1].c_str());
        else if(tokenizer[0] == "msg") {
            msg = tokenizer[1];
        }
        else {
	  reply << FAIL << "bogus value: " << *i << DONE;
	  return;
        }
    }

    MCServerMessageSpec::ConsoleMessage conmess(location, jtag, cpu, thread_id, time_secs, time_usecs);
    // break up the strings
    boost::char_separator<char> sem_sep(";");
    tokenizer sem_tok(msg, sem_sep);
    BOOST_FOREACH(std::string curr_string, sem_tok) {
        conmess._lines.push_back(curr_string);
    }

    // StringTokenizer msg_tokenizer;
    // unsigned int toks = msg_tokenizer.tokenize(msg, ";");
    // for(unsigned int i = 0; i < toks; ++i) {
    //     conmess._lines.push_back(msg_tokenizer[i]);
    // }

    pBlock->processConsoleMessage(conmess);

    reply << OK << DONE;
}

void
MMCSCommand_inject_console::help(deque<string> args,
				   MMCSCommandReply& reply)
{
    // the first data written to the reply stream should be 'OK' or 'FAIL'
    reply << OK << description()
	  << ";Inject a Console message, here is what you can specify"
	  << "; location"
	  << "; jtag"
	  << "; cpu"
          << "; threadid"
	  << "; time_secs"
	  << "; time_usecs"
          << "; msg"
          << ";"
          << "; msg is a semicolon separated list of strings"
          << "; unfortunately, we're not very good at handling spaces in the msg string right now."
	  << DONE;
}
