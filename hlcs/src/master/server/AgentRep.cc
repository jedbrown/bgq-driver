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

#include "AgentRep.h"
#include "AgentManager.h"
#include "Alias.h"
#include "AliasList.h"
#include "MasterController.h"
#include "ras.h"

#include "common/BinaryController.h"

#include "../lib/exceptions.h"



#include <boost/scope_exit.hpp>


#include <pthread.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

LOG_DECLARE_FILE( "master" );

const int AGENTFAIL = -1;
const int SPECIAL_ABEND = -2;

AgentRep::AgentRep(
        AgentProtocolPtr prot,
        const BGMasterAgentProtocolSpec::JoinRequest& joinreq,
        BGMasterAgentProtocolSpec::JoinReply& joinrep
        ) :
    _failover(true),
    _orderly(false),
    _my_tid( 0 ),
    _agent_mutex( )
{
    _agent_id = BGAgentId(joinreq._port, joinreq._ip_address);
    _host = CxxSockets::Host(joinreq._ip_address);
    _prot = prot;

    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);

    // Spin through the list of existing binaries and create binary controllers and alias entries.
    // We'll run through this code if bgmaster_server is restarted while servers are running under the control of agents.
    // We'll also do this if the connection to the agent goes down and gets restarted.
    BOOST_FOREACH(const BGMasterAgentProtocolSpec::JoinRequest::WorkingBins& wb, joinreq._running_binaries) {
        // First, we need to check the list of aliases to determine if it is valid.
        // Then check to see if this binary id is already under accounting.  Then, if not, update the alias.
        // After that, we need to update the list of managed binaries for the agent
        AliasPtr alias;
        if ( !MasterController::_aliases.find_alias( wb._alias, alias) ) {
            LOG_INFO_MSG( "Did not find alias for running binary " << wb._binary_id );
            joinrep._bad_bins.push_back(wb._binary_id);
            continue;
        }

        BOOST_ASSERT( alias );
        LOG_INFO_MSG("Found running binary id " << wb._binary_id << " for alias " << alias->get_name());

        BinaryLocation unused;
        if (MasterController::get_agent_manager().findBinary(wb._binary_id, unused)) {
            LOG_DEBUG_MSG( "alias accounting already current" );
            continue;
        }
        alias->add_binary(wb._binary_id);

        const BinaryControllerPtr binary(
                new BinaryController(
                    wb._binary_id,
                    wb._name, 
                    wb._alias, 
                    alias->get_user(),
                    0,
                    BinaryController::RUNNING
                    )
                );
        this->addController( binary );
        LOG_DEBUG_MSG("Updated alias and agent's binary list for " << alias->get_name());
    }
}

AgentRep::~AgentRep()
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
}

void
AgentRep::stopBin(
        const BinaryId& bid,
        const BinaryLocation& location,
        const int signal,
        BGMasterAgentProtocolSpec::StopReply& stoprep,
        bool failover = false
        )
{
    boost::mutex::scoped_lock scoped_lock(_agent_mutex);
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    stopBin_nl(bid, location, signal, stoprep, failover);
}

void
AgentRep::stopBin_nl(
        const BinaryId& bid,
        const BinaryLocation& location,
        const int signal,
        BGMasterAgentProtocolSpec::StopReply& stoprep,
        bool failover
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_DEBUG_MSG("Stop non-locking for binary id " << bid.str());
    // If it's a fail-over request, we're going to let fail-over happen.
    // Otherwise, we don't want an inadvertent fail-over.
    failover?failover=false:failover=true;
    location.first->stopping(failover);

    // Build the stop request and reply that go to the agent
    BGMasterAgentProtocolSpec::StopRequest stop_to_agent(bid.str(), signal);

    bool success = false;
    int retries = 20;
    while (!success && retries) {
        stoprep._rc = exceptions::OK;
        try {
            _prot->stop(stop_to_agent, stoprep);
        } catch (const CxxSockets::SoftError& err) {
            LOG_WARN_MSG("Agent connection interrupted, will retry.");
            success = false;
            --retries;
            usleep(750);
            continue;
        } catch (const CxxSockets::Error& err) {
            std::string nrs("No requester socket.");
            if (err.errcode == -1 || err.what() == nrs) {
                // Agent not yet set up. Just return.
                return;
            }

            // Client aborted with an incomplete transmission.
            std::ostringstream errmsg;
            errmsg << "Agent connection ended during end agent request. " << err.what();
            agentAbend(errmsg);
        }

        if (stoprep._rc == exceptions::OK || stoprep._rc == exceptions::INFO) {
            success = true;
            location.first->set_status(BinaryController::COMPLETED);
            LOG_DEBUG_MSG("Removing stopped binary id " << bid.str() << " from agent.");
            this->removeController( location.first );
            // Update database with ras message
            AliasPtr alptr;
            BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
                if (al->find_binary(bid)) {
                    alptr = al;
                }
            }
            std::map<std::string, std::string> details;
            std::string alias_name;
            details["BIN"] = bid.str();
            if (alptr) {
                alias_name = alptr->get_name();
            }
            details["ALIAS"] = alias_name;
            int rsignal = stoprep._status._exit_status;
            if (!rsignal) {
                details["SIGNAL"] = boost::lexical_cast<std::string>(0);
            } else {
                details["SIGNAL"] = boost::lexical_cast<std::string>(rsignal);
            }
            MasterController::putRAS(BINARY_STOP_RAS, details);
            std::ostringstream stopmsg;
            stopmsg << "Binary id " << bid.str() << " alias " << alias_name << " stopped on " << _agent_id.str();
            LOG_INFO_MSG(stopmsg.str());
            MasterController::addHistoryMessage(stopmsg.str());
        }

        BinaryId myid = bid;

        // Now find my alias and decrement
        if (failover) {
            BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
                if (al->find_binary(myid)) {
                    al->remove_binary(myid);  // Remove the id from the alias' list.
                }
            }
        }
    }
}

BinaryId
AgentRep::startBin(
        const BGMasterAgentProtocolSpec::StartRequest& startreq,
        BGMasterAgentProtocolSpec::StartReply& startrep
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    const BinaryId bid("0");
    if (MasterController::get_end_requested()) {
        return bid;  // Don't do anything if we're ending.
    }
    boost::mutex::scoped_lock scoped_lock(_agent_mutex);
    return startBin_nl(startreq, startrep);
}

void
AgentRep::agentAbend(
        const std::ostringstream& msg
        )
{
    LOG_TRACE_MSG(__FUNCTION__);
    LOG_ERROR_MSG(msg.str());
    std::map<std::string, std::string> details;
    details["AGENT_ID"] = get_agent_id();
    MasterController::putRAS(AGENT_FAIL_RAS, details);
    MasterController::handleErrorMessage(msg.str());
    _ending = true;
}

BinaryId
AgentRep::startBin_nl(
        const BGMasterAgentProtocolSpec::StartRequest& startreq,
        BGMasterAgentProtocolSpec::StartReply& startrep
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    BinaryId bid("0");

    if (MasterController::get_end_requested()) {
        return bid;  // Don't do anything if we're ending.
    }

    // Make sure we haven't already started the alias associated with
    // this request in another thread (that we aren't over its instance limit!)
    // We already lock this method so we can't have two threads in this code.
    // We just need to be sure we don't stack up a start from a failover/restart
    // and a new agent connection.
    BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
        if (al->get_name() == startreq._alias) {
            if (al->check_instances() == false) {
                LOG_ERROR_MSG("Cannot start an additional instance of " << al->get_name());
                return bid;
            }
        }
    }

    LOG_DEBUG_MSG("Sending start request");
    try {
        _prot->start(startreq,startrep);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_ERROR_MSG("Agent connection error during start send request.");
        return bid;
    } catch (const CxxSockets::Error& err) {
        std::string nrs("No requester socket.");
        if (err.errcode == -1 || err.what() == nrs) {
            // Agent not yet set up. Just return.
            return bid;
        }
        // Client aborted with an incomplete transmission.
        // Bad, bad, bad.  This object must die.
        std::ostringstream errmsg;
        errmsg << "Agent connection ended during start send request. " << err.what();
        agentAbend(errmsg);
        return bid;
    }

    LOG_DEBUG_MSG("Received start reply");

    // If the start was successful, then we need to add a binary controller to the agent representation.
    if (startrep._rc == exceptions::OK && startrep._status._binary_id != "0") {
        LOG_DEBUG_MSG("Adding binary " << startrep._status._binary_id);
        BinaryId b(startrep._status._binary_id);
        bid = b;

        AliasPtr al;
        MasterController::_aliases.find_alias(startreq._alias, al);
        const BinaryControllerPtr bincont(
                new BinaryController(
                    bid, 
                    al->get_path(),
                    startreq._alias,
                    al->get_user(),
                    0,
                    BinaryController::RUNNING
                    )
                );
        addController(bincont);

        // Update database with ras message
        std::map<std::string, std::string> details;
        details["BIN"] = bid.str();
        details["ALIAS"] = startreq._alias;
        MasterController::putRAS(BINARY_START_RAS, details);
        std::ostringstream startmsg;
        startmsg << "Started alias " << al->get_name() << " with binary id " << bid.str() << " on agent " << _agent_id.str() << ".";
        MasterController::addHistoryMessage(startmsg.str());
        al->add_binary(bid);
        LOG_INFO_MSG(startmsg.str());
    } else {
        const int estat = startrep._status._exit_status;
        int signo = 0;
        std::ostringstream msg;
        msg << "Binary " << startreq._alias << ": " << startrep._status._binary_id << " exited.";
        if (WIFEXITED(estat)) {
            const int statnum = WEXITSTATUS(estat);
            msg << " Status: " << statnum;
        }

        if (WIFSIGNALED(estat)) {
            signo = WTERMSIG(estat);
            msg << " Signal: " << signo << ".";
            if (WCOREDUMP(estat)) {
                msg << " Core dumped on " << _host.fqhn() << ".";
            }
            LOG_INFO_MSG(msg.str());
        }

        if (startrep._rt.length() != 0) {
            msg << " Error text is " << startrep._rt << ".";
        }

        // Something went wrong, update RAS.
        std::map<std::string, std::string> details;
        details["ALIAS"] = startreq._alias;
        details["BIN"] = startrep._status._binary_id;
        details["SIGNAL"] = boost::lexical_cast<std::string>(signo);
        details["ESTAT"] = boost::lexical_cast<std::string>(estat);
        details["EMSG"] = startrep._rt;
        MasterController::handleErrorMessage(msg.str());
        MasterController::putRAS(BINARY_FAIL_RAS, details);
    }
    return bid;
}

void
setTriggerFromSignal(
        Policy::Trigger& t,
        const int signo
        )
{
    switch(signo) {
    case AGENTFAIL:
        t = Policy::AGENT_ABEND;
        break;
    case SIGKILL:
        t = Policy::KILL_REQUESTED;
        break;
    case SPECIAL_ABEND:
    default:
        // All other signals fall through as indications of abnormal endings.
        t = Policy::BINARY_ABEND;
        break;
    }
}

void
setTriggerFromString(
        Policy::Trigger& t,
        const std::string& trigger
        )
{
    if (trigger == "b") {
        t = Policy::BINARY_ABEND;
    } else {
        if (trigger == "k") {
            t = Policy::KILL_REQUESTED;
        } else {
            t = Policy::AGENT_ABEND;
        }
    }
}

void
AgentRep::stopBinaryAndExecutePolicy(
        const BinaryId& bid,
        const BinaryLocation& binloc,
        const int signal,
        BGMasterAgentProtocolSpec::StopReply& stoprep,
        const std::string& trigger
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    boost::mutex::scoped_lock scoped_lock(_agent_mutex);
    // First see if there's a policy.
    BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
        if (al->find_binary(bid)) {
            al->remove_binary(bid);
            Policy::Trigger t;
            if (trigger.length() == 0) {
                setTriggerFromSignal(t, signal);
            } else {
                setTriggerFromString(t, trigger);
            }
            BinaryControllerPtr null;
            const AgentRepPtr rep_p = al->evaluatePolicy(t, _agent_id, bid, null);
            if (rep_p) {
                const int sigtosend = (signal == SIGUSR2?SIGUSR2:SIGTERM);
                stopBin_nl(bid, binloc, sigtosend, stoprep, true);
                // We've already removed the binary, so just execute the policy!
                BinaryId binid = binloc.first->get_binid();
                executePolicy_nl(binid, rep_p, al, signal);
                // Now get rid of the old binary controller.
                this->removeController( binloc.first );
            } else {
                // Put the binary back since we couldn't stop it.
                al->add_binary(bid);
                throw exceptions::ConfigError(exceptions::WARN, "Could not execute policy, check configuration.");
            }
        }
    }
}

void
AgentRep::executePolicy_nl(
        const BinaryId& reqbid,
        AgentRepPtr rep_p,
        AliasPtr al,
        const int signo
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);

    bool i_unlocked = false;
    BOOST_SCOPE_EXIT( (&i_unlocked) (&_agent_mutex) ) {
        if (i_unlocked) { // Make sure we're locked again when we get out of here.
            _agent_mutex.lock();
        }
    } BOOST_SCOPE_EXIT_END;

    try {
        if (!rep_p) {
            Policy::Trigger t;
            setTriggerFromSignal(t, signo);
            // No agent rep passed.
            BinaryControllerPtr null;
            rep_p = al->evaluatePolicy(t, _agent_id, reqbid, null);
        }
        if (rep_p) {
            BGMasterAgentProtocolSpec::StartRequest agentreq(al->get_path(), al->get_args(),
                                                             al->get_logdir(), al->get_name(),
                                                             al->get_user());
            std::ostringstream logmsg;
            logmsg << "start request path=" << agentreq._path << " "
                   << "arguments=" << agentreq._arguments << " "
                   << "logdir=" << al->get_logdir() << " "
                   << "user=" << agentreq._user << " ";
            LOG_TRACE_MSG(logmsg.str());

            BGMasterAgentProtocolSpec::StartReply agentrep;
            agentrep._rc = exceptions::OK;

            BinaryId b;
            // If it's me, I'm already locked.
            if (rep_p->get_agent_id() == get_agent_id()) {
                b = rep_p->startBin_nl(agentreq, agentrep);
            } else {
                // rep_p->_agent_request_queue.dropOff(agentreq);
                // Otherwise, I need the locking version. But I'm going to have to release my lock
                // while I do this, then re-obtain it. I don't like this, because I'm under a scope
                // lock further up the call stack. At some point, we need a neutral third party to
                // handle this.  Perhaps we dump on a DoQ and interrupt the monitor thread.
                _agent_mutex.unlock();
                i_unlocked = true;
                b = rep_p->startBin(agentreq, agentrep);
            }

            if (b.str() != "0") {
                al->add_binary(b);
            } else {
                LOG_WARN_MSG("Not starting alias " << al->get_name());
            }
        }
    } catch (const exceptions::InternalError& e) {
        std::ostringstream msg;
        msg << "Unable to execute policy for " << reqbid.str() << "|" << al->get_name() << " " << e.what();
        MasterController::handleErrorMessage(msg.str());
        // Any RAS should have been dropped by the Alias object.
    }
}

void
AgentRep::executePolicyAndClear_nl(
        BinaryControllerPtr binptr,
        AgentRepPtr rep_p, 
        const int signo
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    const BinaryId reqbid = binptr->get_binid();
    if (binptr->stopping() != true) {
        // We haven't explicitly stopped, so we have to check our policy
        BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
            if (al->find_binary(reqbid)) {
                // This alias has my binary id, so remove my id and execute the policy
                al->remove_binary(reqbid);
                executePolicy_nl(reqbid, rep_p, al, signo);
            }
        }
    } else {
        // No policy, so remove only.
        BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
            al->remove_binary(reqbid);
        }
    }

    // Now get rid of the old binary controller.
    this->removeController( binptr );
}

void
AgentRep::doCompleteRequest(
        const BGMasterAgentProtocolSpec::CompleteRequest& compreq
        )
{
    boost::mutex::scoped_lock scoped_lock(_agent_mutex);
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    // First, find our binary id
    const BinaryId reqbid(compreq._status._binary_id);

    // Find the corresponding controller
    BinaryControllerPtr bptr;
    if ( !this->find_binary(reqbid, bptr) ) {
        LOG_WARN_MSG( "Binary " << reqbid.str() << " is not in our list." );
    } else {
        bptr->set_status(BinaryController::COMPLETED);
        LOG_INFO_MSG("Removing completed binary id " << reqbid.str() << "|" << bptr->get_binary_bin_path());
        this->removeController( bptr );

        // Now find my alias and decrement
        BOOST_FOREACH(const AliasPtr& al, MasterController::_aliases) {
            if (al->find_binary(reqbid)) {
                al->remove_binary(reqbid);
                break;
            }
        }
    }

    if (compreq._exit_status != 0) {
        const int status = WEXITSTATUS(compreq._exit_status);
        std::ostringstream error;
        error << "Binary id " << reqbid.str() << "|" << bptr->get_alias_name() << " failed with exit status " << status;
        MasterController::handleErrorMessage(error.str());
    }

    // Update database with RAS message
    std::map<std::string, std::string> details;
    details["BIN"] = reqbid.str();
    details["ALIAS"] = bptr->get_alias_name();
    std::string signal = "0";
    details["SIGNAL"] = signal;
    MasterController::putRAS(BINARY_STOP_RAS, details);
    std::ostringstream startmsg;
    startmsg << "Binary id " << reqbid.str() << " for alias " << bptr->get_alias_name() << " stopped on " << _agent_id.str();
    MasterController::addHistoryMessage(startmsg.str());

    BGMasterAgentProtocolSpec::CompleteReply comprep(0, "success");

    try {
        _prot->sendReply(comprep.getClassName(), comprep);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_ERROR_MSG("Agent connection error during complete reply send.");
        return;
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        // Bad, bad, bad.  This object must die
        std::ostringstream msg;
        msg << "Agent connection ended during complete reply send.";
        agentAbend(msg);
    }
    return;
}

void
AgentRep::doFailedRequest(
        const BGMasterAgentProtocolSpec::FailedRequest& failreq
        )
{
    boost::mutex::scoped_lock scoped_lock(_agent_mutex);
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    // First, find our binary id
    const BinaryId reqbid(failreq._status._binary_id);

    // Update our exit status
    BinaryControllerPtr binptr;
    int signo = 0;
    if (find_binary(reqbid, binptr)) {
        binptr->set_status(BinaryController::COMPLETED);
        const int estat = failreq._status._exit_status;
        int statnum = 1;
        std::ostringstream msg;
        msg << "Alias " << binptr->get_alias_name() << " binary id " << binptr->get_binid().str() << " exited";
        if (WIFEXITED(estat)) {
            statnum = WEXITSTATUS(estat);
            msg << ". Status: " << statnum;
        }
        binptr->set_exit_status(statnum);

        // Log what happened
        if (WIFSIGNALED(estat)) {
            signo = WTERMSIG(estat);
            msg << ". Signal: " << signo;
            if (WCOREDUMP(estat)) {
                msg << ". Core dumped on " << _host.fqhn();
            }
            LOG_INFO_MSG(msg.str());
        }

        // Update database with RAS message
        std::map<std::string, std::string> details;
        details["ALIAS"] = binptr->get_alias_name();
        details["BIN"] = reqbid.str();
        details["SIGNAL"] = boost::lexical_cast<std::string>(signo);
        details["ESTAT"] = boost::lexical_cast<std::string>(estat);
        details["EMSG"] = msg.str();
        MasterController::putRAS(BINARY_FAIL_RAS, details);
        MasterController::handleErrorMessage(msg.str());
    } else {
        // Whoa!  We don't know about this binary?
        // This can happen in situation where mc_server ends first and then mmcs_server fails so
        // it may be normal for this to occur.
        std::ostringstream msg;
        msg << "Could not find binary " << failreq._status._binary_id << " that failed on agent "
            << _agent_id.str() << ". This is normal in some instances.";
        // Don't return here.  It might be a buffered fail request saved by the agent.
        // This can happen if bgmaster_server restarts. We'll note it as an error, execute any policy, and send a reply.
        MasterController::handleErrorMessage(msg.str());
    }

    if (!signo) {
        // This thing must have gone EXIT_FAILURE
        signo = SPECIAL_ABEND;
    }

    if (binptr) {
        AgentRepPtr p;
        executePolicyAndClear_nl(binptr, p, signo);
    }

    BGMasterAgentProtocolSpec::FailedReply failrep(0, "success");

    try {
        _prot->sendReply(failrep.getClassName(), failrep);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_ERROR_MSG("Agent connection error during failed reply send.");
        return;
    } catch (const CxxSockets::Error& err) {
        // Client aborted with an incomplete transmission
        // Bad, bad, bad.  This object must die
        std::ostringstream msg;
        msg << "Agent connection ended during failed reply send.";
        agentAbend(msg);
        return;
    }
}

bool
AgentRep::processRequest()
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    // These will be requests coming from the bgagent
    std::string request_name;
    try {
        _prot->getName(request_name);
    } catch (const CxxSockets::SoftError& err) {
        // For soft errors, we just back out and let it try again
        LOG_ERROR_MSG("Agent connection error processing request from agent.");
        return true;
    } catch (const CxxSockets::Error& err) {
        // Any other error, we bail
        std::ostringstream msg;
        msg << "Agent connection ended.";
        agentAbend(msg);
        return false;
    }

    LOG_DEBUG_MSG("Request " << request_name << " received from agent " << get_agent_id().str());

    if (request_name == "CompleteRequest") {
        BGMasterAgentProtocolSpec::CompleteRequest compreq;
        try {
            _prot->getObject(&compreq);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again
            LOG_ERROR_MSG("Agent connection error processing request from agent.");
            return true;
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission.
            // Bad, bad, bad.  This object must die.
            std::ostringstream msg;
            msg << "Agent connection ended.";
            agentAbend(msg);
            return false;
        }
        doCompleteRequest(compreq);
    } else if (request_name == "FailedRequest") {
        BGMasterAgentProtocolSpec::FailedRequest failreq;
        try {
            _prot->getObject(&failreq);
        } catch (const CxxSockets::SoftError& err) {
            // For soft errors, we just back out and let it try again.
            LOG_ERROR_MSG("Agent connection error processing request from agent.");
            return true;
        } catch (const CxxSockets::Error& err) {
            // Client aborted with an incomplete transmission.
            // Bad, bad, bad.  This object must die.
            std::ostringstream msg;
            msg << "Agent connection ended.";
            agentAbend(msg);
            return false;
        }
        doFailedRequest(failreq);
    } else {
        // Bad request, must end, but can't fail over because the agent is still alive.
        LOG_ERROR_MSG("Bad request " << request_name << " received.");
        _failover = false;
        _ending = true;
    }
    return false;
}

void
AgentRep::waitMessages()
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    // Wait for requests and send responses from the associated bgagent.

    _my_tid = pthread_self();

    bool soft = false;
    while (!_ending) {
        // Process message
        soft = processRequest();
    }

    if (!_orderly) {
        // We got here because we lost contact with our agent or something
        // bad in our communications protocol. Cut a RAS message.
        LOG_INFO_MSG("Connected bgagentd " << get_agent_id().str() << " on " << get_host().uhn() << " failed.");
    }

    if (MasterController::get_agent_manager().get_ending_agents()) {
        _failover = false;
    }

    // We are here either because we got a hard error on the bgagent connection or because we got a
    // soft error and we are ending anyway. If we are ending anyway, let that happen and don't expedite it here.
    if (!soft) {
        // We're done here. Get rid of it.
        AgentRepPtr self_ptr = shared_from_this();
        MasterController::get_agent_manager().removeAgent(self_ptr);
    }
}

void
AgentRep::startPoller()
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    _agent_socket_poller = boost::thread(&AgentRep::waitMessages, this);
}

void
AgentRep::cancel(
        const bool binaries,
        const int signal
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);
    std::ostringstream os;
    os << std::setbase(16);
    os << _my_tid;

    LOG_DEBUG_MSG("Canceling agent thread " << os.str());
    // First, end all of the binaries:
    BGMasterClientProtocolSpec::StopReply reply;
    reply._rc = exceptions::OK;

    if (binaries) {
        stopAllBins(reply, signal);
    } else {
        // Mark all bins UNINITIALIZED to free all waiters.
        boost::mutex::scoped_lock scoped_lock(_agent_mutex);
        const Binaries binaries = this->get_binaries();
        for ( Binaries::const_iterator i = binaries.begin(); i != binaries.end(); ++i ) {
            BinaryControllerPtr rit = *i;
            LOG_DEBUG_MSG("Setting alias " << rit->get_alias_name() << " to UNINITIALIZED");
            rit->set_status(BinaryController::UNINITIALIZED);
        }
    }

    _ending = true;
    _orderly = true;
    if ( _my_tid ) {
        pthread_kill(_my_tid, SIGUSR1);
    }
    _agent_socket_poller.join();
}

void
AgentRep::stopAllBins(
        BGMasterClientProtocolSpec::StopReply& reply,
        const int signal
        )
{
    LOGGING_DECLARE_ID_MDC(_agent_id.str());
    LOG_TRACE_MSG(__FUNCTION__);

    Binaries binaries;
    {
        boost::mutex::scoped_lock scoped_lock(_agent_mutex);
        binaries = this->get_binaries();
    }

    LOG_INFO_MSG("Stopping " << binaries.size() << " binaries on agent " << get_agent_id().str() << " with signal " << signal);
    for (Binaries::const_iterator i = binaries.begin(); i != binaries.end(); ++i) {
        const BinaryControllerPtr rit = *i;
        if (rit) {
            LOG_INFO_MSG("Stopping alias " << rit->get_alias_name() << " with binary id " << rit->get_binid().str()
                         << " on agent " << get_agent_id().str() << ".");
            BinaryLocation location;
            location.first = rit;

            BGMasterAgentProtocolSpec::StopReply stop_from_agent;
            stop_from_agent._rc = exceptions::OK;

            stopBin_nl(rit->get_binid(), location, signal, stop_from_agent, false);
            // Now collect the response and add it to the reply
            const BGMasterClientProtocolSpec::StopReply::BinaryStatus binstat_to_return(
                    stop_from_agent._status._binary_id,
                    stop_from_agent._status._exit_status
                    );

            reply._statuses.push_back(binstat_to_return);
            if (stop_from_agent._rc != exceptions::OK) {
                reply._rc = stop_from_agent._rc;
                reply._rt = stop_from_agent._rt;
            }
        }
    }
}
