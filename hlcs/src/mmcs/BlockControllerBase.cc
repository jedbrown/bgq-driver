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

#include <algorithm>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <list>
#include <map>
#include <netinet/in.h>
#include <pwd.h>
#include <set>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/types.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <boost/foreach.hpp>
#include <boost/format.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/posix_time/posix_time_io.hpp>
#include <xml/include/c_api/MCBootSteps.h>

#include <bgq_util/include/TCPconnect.h>

#ifdef WITH_DB
#include <db/include/api/BGQDBlib.h>
#endif
#include <bgq_util/include/LocationUtil.h>
#include <control/include/bgqconfig/BGQBlockNodeConfig.h>
#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>
#include <control/include/bgqconfig/BGQNodePos.h>
#include <control/include/bgqconfig/BGQPersonality.h>

#include <control/include/mcServer/MCServerRef.h>
#include <control/include/mcServer/MCServer_errno.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>
#include <log4cxx/mdc.h>
#include "SecureConnect.h"
#include "BlockControllerBase.h"
#include "MMCSCommandReply.h"
#include "MMCSProperties.h"
#include "HardwareBlockList.h"
#include "MMCSMasterMonitor.h"

LOG_DECLARE_FILE( "mmcs" );

using namespace std;
using namespace MCServerMessageSpec;

struct sockaddr_in BlockControllerBase::_local_addr;
CxxSockets::SockAddr BlockControllerBase::_local_saddr;

BlockControllerBase::BlockControllerBase(
        BGQMachineXML* machine,
        const std::string& userName,
        const std::string& blockName,
        bool delete_machine
        ) :
    _machineXML(machine),
    _blockXML(NULL),
    _block(NULL),
    _mboxMonitor(this),
    _smboxMonitor(this),
    _mcServer(),
    _numNodesActive(0),
    _numNodesStarted(0),
    _numNodesTotal(0),
    _numComputesTotal(0),
    _bootCookie(0),
    _savedBootCookie(0),
    _targetsetMode(WUAR),
    _diags(false),
    _insertDiagsRAS(false),
    _mailboxOutput(NULL),
    _rasLogFile(NULL),
    _mutex(PTHREAD_MUTEX_RECURSIVE_NP),
    _blockThreads(0),
    _disconnecting(false),
    _userName(userName),
    _blockName(blockName),
    _last_ras_event_time(0),
    _shutdown_sent_time(0),
    _isstarted(false),
    _isblock(false),
    _isIOblock(false),
    _isshared(false),
    _boot_block_start(0),
    _io_linkless_midplanes(0),
    _delmachine(delete_machine),
    _terminatedNodes(0),
    _do_barrier_ack(false),
    _block_shut_down(false),
    _check_complete(true),
    _rebooting(false),
    _domains(),
    _bootOptions(),
    _nodeset(false),
    _residual_target(0),
    _machine_config_data(""),
    _rebooted(false)
{
    LOG_DEBUG_MSG(__FUNCTION__);
    // nothing to do
}

BlockControllerBase::~BlockControllerBase()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);

    delete_block();
    if (_machineXML && _delmachine)
    {
        delete _machineXML;
	_machineXML = NULL;
    }

    if(MMCSProperties::getProperty(SECURE_MC) == "true") {
        if(_smboxMonitor.isStarted() == true) {
            _smboxMonitor.stop();
        }
    } else {
        if(_mboxMonitor.isStarted() == true) {
            _mboxMonitor.stop();
        }
    }
}

void BlockControllerBase::create_nodeset(std::vector<BCNodeInfo*> args, MMCSCommandReply& reply) {
    LOG_DEBUG_MSG(__FUNCTION__);
    if(args.size() < 1) {
        reply << FAIL << "No nodes specified" << DONE;
        return;
    }
    _delmachine = false;
    _nodeset = true;
    BOOST_FOREACH(BCNodeInfo* curr_node, args) {
        getNodes().push_back(curr_node);
    }
}

void
BlockControllerBase::pgood_reset(MMCSCommandReply& reply, BlockControllerTarget* pTarget)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    string pgood_regexp; 	        // regular expression for PgoodResetRequest message target

    // build a regular expression for the open compute and I/O nodes in the block
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
    {
	BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        if((_isIOblock && nodeInfo->isIOnode()) ||
           (!_isIOblock && !nodeInfo->isIOnode())) {
            // create a target set for sending pgood to the processor nodes
            if (pgood_regexp.empty())
                pgood_regexp.append("(").append(nodeInfo->location());
            else
                pgood_regexp.append("|").append(nodeInfo->location());
        }
    }

    // send a pgood reset request to the open compute and I/O nodes
    if (!pgood_regexp.empty())
    {
	pgood_regexp.append(")");

	MCServerMessageSpec::PgoodResetRequest mcPgoodResetRequest;
	mcPgoodResetRequest._expression.push_back(pgood_regexp);
	MCServerMessageSpec::PgoodResetReply   mcPgoodResetReply;
	try
	{
	    getMCServer()->pgoodReset(mcPgoodResetRequest, mcPgoodResetReply);
	}
	catch (exception &e)
	{
	    mcPgoodResetReply._rc = -1;
	    mcPgoodResetReply._rt = e.what();
	}
	catch (XML::Exception &e)
	{
	    ostringstream buf; buf << e;
	    mcPgoodResetReply._rc = -1;
	    mcPgoodResetReply._rt = buf.str();
	}
	if (mcPgoodResetReply._rc)
	{
	    reply << FAIL << mcPgoodResetReply._rt << DONE;
	    return;
	}
    }
    reply << OK << DONE;
}

void
BlockControllerBase::deleteTargets()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    if(!_nodeset) { // If we're a nodeset, we don't own the nodes.
        // delete BlockControllerBase targets
        for (unsigned i = 0; i < getNodes().size(); ++i)
            delete getNodes()[i];
        getNodes().clear();

        for (unsigned i = 0; i < getLinkchips().size(); ++i)
            delete getLinkchips()[i];
        getLinkchips().clear();

        for (unsigned i = 0; i < getIcons().size(); ++i) {
            BCIconInfo* icon = getIcons()[i];
            for(unsigned j = 0; j < dynamic_cast<BCNodecardInfo*>(icon)->_linkChips.size(); ++j) {
                delete dynamic_cast<BCNodecardInfo*>(icon)->_linkChips[j];
            }
            delete icon;
        }
        getIcons().clear();

        for(std::map<std::string,BCTargetInfo*>::iterator curr_pair = _targetLocationMap.begin();
            curr_pair != _targetLocationMap.end(); ++curr_pair) {
            //        BCTargetInfo* tg = curr_pair->second;
            // if(tg)
            //     delete tg;
        }
        _targetLocationMap.clear();

        for (unsigned i = 0; i < getTargets().size(); ++i) {
            // if(getTargets()[i] != 0)
            //     delete getTargets()[i];
        }
        getTargets().clear();
    }
}

int
BlockControllerBase::makeAndOpenTargetSet(MCServerRef* ref, bool temp_targetset, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets) {
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_INFO_MSG(__FUNCTION__);
    //
    // create the target set for the block resources
    //
    MCServerMessageSpec::MakeTargetSetRequest mcMakeTargetSetRequest( _blockName, _userName, temp_targetset);
    MCServerMessageSpec::MakeTargetSetReply   mcMakeTargetSetReply;

    //
    // Mark all specified targets as opened
    //
    for (unsigned i = 0; i < pTarget->getTargets().size(); ++i)
    {
	BCTargetInfo *targetInfo = pTarget->getTargets()[i];
	targetInfo->_open = true;	// mark this target as open
    }

    if (_isshared == false)	        // if we are sharing node cards, don't include them in the target set
    {
	// ensure that node cards for the targeted nodes are included in the target set
	for (unsigned i = 0; i < pTarget->getNodes().size(); ++i)
	{
	    BCNodeInfo *targetInfo = pTarget->getNodes()[i];
	    string nodecardLocation(targetInfo->location().substr(0,10));
	    BCNodecardInfo *nodecardInfo = (BCNodecardInfo*) _targetLocationMap[nodecardLocation];
	    if (nodecardInfo != NULL && nodecardInfo->_passthru_only == false)
		nodecardInfo->_open = true;	// mark this target as open

	}

	// ensure that link cards for the targeted link chips are included in the target set
	for (unsigned i = 0; i < pTarget->getLinkchips().size(); ++i)
	{
	    BCLinkchipInfo *targetInfo = pTarget->getLinkchips()[i];
	    string linkcardLocation(targetInfo->location().substr(0,9));
	}
    }

    // add all targeted nodes, node cards link chips and link cards to the target set
    for (unsigned i = 0; i < getTargets().size(); ++i)
    {
	BCTargetInfo* targetInfo = getTargets()[i];
	if (targetInfo->_open && targetInfo->_linkio == false && targetInfo->_passthru_only == false)
        {
	    mcMakeTargetSetRequest._location.push_back(targetInfo->location());
            LOG_TRACE_MSG( "adding " << targetInfo->location() << " to target set " << _blockName );
        }
    }

    // Add everything in the passed target if requested.
    if(add_targets) {
        for (unsigned i = 0; i < pTarget->getTargets().size(); ++i) {
            BCTargetInfo* targetInfo = pTarget->getTargets()[i];
            if(targetInfo->_passthru_only == false)
                mcMakeTargetSetRequest._location.push_back(targetInfo->location());
            LOG_TRACE_MSG( "adding extra target " << targetInfo->location() << " to target set " << _blockName );
        }
    }

    try
    {
	ref->makeTargetSet(mcMakeTargetSetRequest, mcMakeTargetSetReply);
    }
    catch (exception& e)
    {
	mcMakeTargetSetReply._rc = -1;
	mcMakeTargetSetReply._rt = e.what();
    }
    if (mcMakeTargetSetReply._rc)
    {
	if (mcMakeTargetSetReply._rc != BGERR_MCSERVER_TARGETSET_EXISTS)
	{
	    LOG_ERROR_MSG("makeTargetSet: " << mcMakeTargetSetReply._rt);
	    reply << FAIL << mcMakeTargetSetReply._rt << DONE;
            deque<string> dq;
	    disconnect(dq);
	    return 0;
	}
    }

    LOG_DEBUG_MSG("mcServer target set " << _blockName << " created");

    //
    // open the target set in Control mode unless otherwise specified
    //

    MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(
                                                                  _blockName,
								 _userName,  		// user name
								 _targetsetMode,
								 true);     		// autoclose
    MCServerMessageSpec::OpenTargetReply   mcOpenTargetSetReply;
    try
    {
	ref->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    }
    catch (exception &e)
    {
	mcOpenTargetSetReply._rc = -1;
	mcOpenTargetSetReply._rt = e.what();
    }
    if (mcOpenTargetSetReply._rc)
    {
	LOG_ERROR_MSG("openTargetSet: " << mcOpenTargetSetReply._rt);
	reply << FAIL << "openTargetSet: " << mcOpenTargetSetReply._rt << DONE;
        deque<string> dq;
	disconnect(dq);
	return 0;
    }
    else
	LOG_INFO_MSG("mcServer target set " << _blockName << " opened");

    return mcOpenTargetSetReply._handle;
}

void
BlockControllerBase::connect(deque<string> args, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    bool temp_targetset	= true;    	// create target set as temporary
    string szOutFile;			// console message file
    string szRasFile;			// ras message file
    bool usePgood = false;

    PthreadMutexHolder mutex;

    mutex.Lock(&_mutex);	// serialize this code

    if (!isCreated())
    {
        reply << FAIL << "block is not created" << DONE;
	return;
    }
    if (isConnected())
    {
        reply << FAIL << "block is already connected" << DONE;
	return;
    }
    if (pTarget == NULL  || pTarget->getTargets().size() == 0)
    {
	reply << FAIL << "no targets selected" << DONE;
	return;
    }

    if(hardWareAccessBlocked()) {
        std::ostringstream msg;
        msg << "BlockControllerBase::connect(): SubnetMc managing hardware for " << _blockName 
            << " temporarily unavailable.";
        LOG_ERROR_MSG(msg.str());
        reply << FAIL << msg.str() << DONE;
        return;
    }

    _targetsetMode = WUAR;
    for (unsigned i = 0; i < args.size(); ++i)
    {
        vector<string> tokens;
        tokens = Tokenize(args[i], "=");           // split based on the equal sign.

        if (tokens[0] == "outfile")                 { if (tokens.size() == 2) { szOutFile = tokens[1];  continue; } }
        else if (tokens[0] == "rasfile")            { if (tokens.size() == 2) { szRasFile = tokens[1];  continue; } }
        else if (tokens[0] == "no_pgood")           { usePgood = false;                                 continue; }
        else if (tokens[0] == "pgood")              { usePgood = true;                                  continue; }
        else if (tokens[0] == "mode") {
            if (tokens.size() == 2)
                {
                    if (tokens[1]      == "service")   { _targetsetMode = WENR; continue; }
                    else if (tokens[1] == "reserve")   { _targetsetMode = WUAR; continue; }
                    else if (tokens[1] == "monitor")   { _targetsetMode = RAAW; continue; }
                    else if (tokens[1] == "debug")     { _targetsetMode = WUAR; continue; }
                    else if (tokens[1] == "control")   { _targetsetMode = WUAR; continue; }
                    else
                        {
                            reply << FAIL << "unrecognized argument: " << tokens[0] << "=" << tokens[1] << DONE;
                            return;
                        }
                }

        }
        else if (tokens[0] == "targetset")
        {
            if (tokens.size() == 2)
            {
                if (tokens[1]       == "temp")  { temp_targetset = true;    continue; }
                else if (tokens[1]  == "perm")  { temp_targetset = false;   continue; }
                else
                {
                    reply << FAIL << "unrecognized argument: " << tokens[0] << "=" << tokens[1] << DONE;
                    return;
                }
            }
        } else {
            //            reply << FAIL << "unknown argument: " << args[i] << DONE;
            //            return;
        }
    }

    //
    // if outfile was specified on connect, attempt to open it here.
    //
    if (szOutFile.length() > 0)
    {
        FILE *pFile = fopen(szOutFile.c_str(), "wa");
        if (pFile == NULL)
        {
	    reply << FAIL << "can't open file: " << szOutFile << " - " << strerror(errno) << DONE;
            return;
        }
        _mailboxOutput = pFile;
    }
    else
	_mailboxOutput = NULL;


    //
    // if ras file was specified on connect, attempt to open it here.
    //
    FILE *pFile = fopen(szRasFile.c_str(), "wa");
    if (szRasFile.length() > 0)
    {
        if (pFile == NULL)
        {
	    reply << FAIL << "can't open file: " << szOutFile << " - " << strerror(errno) << DONE;
            return;
        }
        _rasLogFile = pFile;
    }
    else
	_rasLogFile = NULL;

    //
    // establish a socket connection to mcServer
    //
    if (!isConnected())
    {
        BlockControllerBase::mcserver_connect(_mcServer, _userName, reply);
	if (reply.getStatus() != 0)
	{
            deque<string> dq;
	    disconnect(dq);
	    return;
	}
	else
	    LOG_INFO_MSG("connected to mcServer");
    }
    _targetsetHandle = makeAndOpenTargetSet(_mcServer, temp_targetset, reply, pTarget, add_targets);
}

void
BlockControllerBase::disconnect(deque<string>& args)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_INFO_MSG(__FUNCTION__);
    MMCSCommandReply reply;
    deque<string> arrgs;
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);	// serialize this code

    if (!isConnected())
	return;

    if(_mcServer == 0) {
        LOG_DEBUG_MSG("Block " << _blockName << " is already disconnected.");
        _disconnecting = false;
        return;
    }

    time_t bst = getBootStartTime();
    if(bst) { // We're waiting on a boot
      struct tm tod_tm;		// output from localtime
      char time_str[16];		// output from ctime
      ostringstream pfx;		// log entry prefix

      pfx.str("");		// clear the work buffer

      localtime_r(const_cast<const time_t*>(&bst), &tod_tm);
      strftime(time_str, sizeof(time_str), "%b %d %H:%M:%S", &tod_tm);
      pfx << time_str;
      reply << FAIL << "Block has been booting since " << pfx.str() << DONE;
      return;
    }

    if (isStarted())
    {
        bool no_sysrq = false;
        if(args.size() != 0 && std::find(args.begin(), args.end(), "no_sysrq") != args.end()) {
            no_sysrq = true;
        }

        if(std::find(args.begin(), args.end(), "no_shutdown") == args.end()) {
            try {
                shutdown_block(reply, 0, no_sysrq);
            }
            catch (exception& e)
                {
                    reply << FAIL << e.what() << DONE;
                }
            if (reply.getStatus() != 0)
                {
                    LOG_ERROR_MSG("shutdown_block: " << reply.str());
                    reply.reset();
                }
        }
    }

    // Wait for the mailbox I/O to quiesce, then stop the MailboxMonitor
    //    quiesceMailbox();
    stopMailbox();		// terminate the mailbox polling thread

    PthreadMutexHolder mb_mutex;
    PthreadMutex* lockbox = getMailBoxLockBox();
    mb_mutex.Lock(lockbox);
    if ((_mailboxOutput != NULL) && (_mailboxOutput != stdout))
        fclose(_mailboxOutput);
    _mailboxOutput = stdout;
    mb_mutex.Unlock();
    if (_rasLogFile)
        fclose(_rasLogFile);
    _rasLogFile = NULL;

    // close the target set
    try
    {
	MCServerMessageSpec::CloseTargetRequest mcCloseTargetSetRequest(_blockName,"", _targetsetMode, _targetsetHandle);
	MCServerMessageSpec::CloseTargetReply   mcCloseTargetSetReply;
	_mcServer->closeTarget(mcCloseTargetSetRequest, mcCloseTargetSetReply);
	if (mcCloseTargetSetReply._rc) {
	    LOG_ERROR_MSG("closeTargetSet: " << mcCloseTargetSetReply._rt);
	} else {
	    LOG_INFO_MSG("mcServer target set " << _blockName << " closed");
	}

	// delete the target set
	MCServerMessageSpec::DeleteTargetSetRequest mcDeleteTargetSetRequest(_blockName, _userName);
	MCServerMessageSpec::DeleteTargetSetReply   mcDeleteTargetSetReply;
	_mcServer->deleteTargetSet(mcDeleteTargetSetRequest, mcDeleteTargetSetReply);
	if (mcDeleteTargetSetReply._rc) {
	    LOG_ERROR_MSG("deleteTargetSet: " << mcDeleteTargetSetReply._rt);
	} else {
	    LOG_INFO_MSG("mcServer target set " << _blockName << " deleted");
	}
    }
    catch (exception &e)
    {
	LOG_ERROR_MSG(e.what());
    }

    // close socket connection to mcServer
    MCServerRef* tempRef = _mcServer;
    _mcServer = NULL;
    delete tempRef;
    //    _mcServer->closeConnection();

    // mark the targets as closed
    for (vector<BCTargetInfo*>::iterator iter = getTargets().begin(); iter != getTargets().end(); ++iter) {
        (*iter)->_open = false;	// mark this target as closed
    }

    // reset CNBlockController status
    _numNodesActive	    = 0;
    _numNodesStarted = 0;
    //     _numNodesTotal   = 0;        Commented out, because the number of nodes doesnt change on a disconnect
    _terminatedNodes  = 0;
    _disconnecting	    = false;
    //    _isstarted		    = false;
    _do_barrier_ack         = false;
}

void
BlockControllerBase::initMachineConfig(MMCSCommandReply& reply)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    if (!_machineXML)
    {
        _helper->initMachineConfig(reply);
    }

    if(reply.getStatus() == MMCSCommandReply::STATUS_NOT_SET)
        reply << OK << DONE;
}

void
BlockControllerBase::delete_block()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);	// serialize this code

    //    _delmachine = true;
    // disconnect from BlockControllerBase targets
    //    disconnect();

    // delete BlockControllerBase targets
    deleteTargets();

    // reset BlockControllerBase status
    _isblock		    = false;

    if (_blockXML)
    {
	delete _blockXML;
	_blockXML = NULL;
    }
    if (_block)
    {
	delete _block;
	_block = NULL;
    }
    _blockName = "";
}

void
BlockControllerBase::delete_block(deque<string> args, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    delete_block();
    reply << OK << DONE;
}

void
BlockControllerBase::mcserver_connect(MCServerRef*& mcServer, string userName, MMCSCommandReply& reply)
{
    LOG_DEBUG_MSG(__FUNCTION__);

    //
    // establish a socket connection to mcServer
    //
    std::string ip = MMCSProperties::getProperty(MC_SERVER_IP);
    std::string port = MMCSProperties::getProperty(MC_SERVER_PORT);
    std::string mcserver = ip + ":" + port;
    if(MMCSProperties::getProperty(SECURE_MC) == "true") {
        try {
            bgq::utility::PortConfiguration::Pairs portpairs;
            std::string servname = "1206";

            bgq::utility::ClientPortConfiguration port_config(servname);
            port_config.setProperties(MMCSProperties::getProperties(), "");
            port_config.notifyComplete();
            bgq::utility::PortConfiguration::parsePortsStr(mcserver, servname, portpairs);
            CxxSockets::SecureTCPSocketPtr sock;
            SecureConnect::Connect(portpairs, sock);
            sock->getSockName(BlockControllerBase::_local_saddr);
            mcServer = new MCServerRef(sock);
	}
	catch (SecureConnect::SecureConnectException& e) {
            reply << FAIL << "Unable to connect to mc_server: " << e.what() << DONE;
            return;
        }
    } else {
        LOG_INFO_MSG("connecting to mcServer at " << mcserver);
        int fd = TCPconnect(mcserver.c_str(), 5, true);
        if (fd == -1)
            {
                reply << FAIL << "unable to connect to mcServer" << DONE;
                return;
            }

        socklen_t myAddr_len = sizeof(_local_addr);
        int rc = getsockname(fd, (struct sockaddr*) &_local_addr, &myAddr_len);
        if (rc != 0)
            {
                reply << FAIL << "Bad socket file descriptor: " << strerror(errno) << DONE;
                close(fd);
                return;
            }

        //    mcServer = MCServerRef::create(fd);
        mcServer = new MCServerRef(fd);
    }
    //    mcServer->_closed = false;
    // sign in to the mcServer
    char hostname[32];
    gethostname(hostname, 32);
    hostname[31]='\0';
    MCServerMessageSpec::ConnectRequest mcConnectRequest(atoi(MCServerMessageSpec::VERSION.c_str()), userName, hostname);
    MCServerMessageSpec::ConnectReply   mcConnectReply;
    try
    {
	(mcServer)->connect(mcConnectRequest, mcConnectReply);
    }
    catch (exception &e)
    {
	mcConnectReply._rc = -1;
	mcConnectReply._rt = e.what();
    }
    if (mcConnectReply._rc)
    {
	reply << FAIL << "mcserver_connect: " << mcConnectReply._rt << DONE;
	return;
    }
    reply << OK << DONE;
}

bool
BlockControllerBase::isConnected()
{
    LOG_TRACE_MSG(__FUNCTION__ << " " << _mcServer);
    if(_mcServer != 0) {
        return true;
    }
    return false;
}

int
BlockControllerBase::decrBlockThreads()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    int numThreads;
    numThreads = --_blockThreads;
    return numThreads;
}

int
BlockControllerBase::incrBlockThreads()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    int numThreads;
    numThreads = ++_blockThreads;
    return numThreads;
}

int
BlockControllerBase::getBlockThreads()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    int numThreads;

    PthreadMutexHolder mutex;
    //    mutex.Lock(&_mutex);
    numThreads = _blockThreads;
    //    mutex.Unlock();
    return numThreads;
}

bool
BlockControllerBase::readyToDie()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);
    if(_blockThreads == 0 && (_disconnecting || _mcServer == 0)) {
        return true;
    } else
        return false;
}

bool
BlockControllerBase::isDisconnecting()
{
    bool disconnecting;
    PthreadMutexHolder mutex;

    mutex.Lock(&_mutex);
    disconnecting = _disconnecting;
    mutex.Unlock();
    return disconnecting;
}

void
BlockControllerBase::setDisconnecting(bool disconnectOption, string disconnectReason)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;

    if(_disconnecting == true)
        return;

    mutex.Lock(&_mutex);
    if (disconnectOption == true && _disconnecting == false)
        {
            _disconnecting = true;
            _disconnectReason = disconnectReason;
        }
    else if (disconnectOption == false)
        {
            _disconnecting = false;
            _disconnectReason = "";
        }
    mutex.Unlock();
}

void
BlockControllerBase::setIsBlock(bool isBlock)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;

    mutex.Lock(&_mutex);
    if (!isCreated())
	_isblock = isBlock;
    mutex.Unlock();
}

BCNodeInfo *
BlockControllerBase::findNodeInfo(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    for (unsigned i = 0; i < getNodes().size(); ++i)
        {
            BCNodeInfo *nodeInfo = getNodes()[i];
            if ((nodeInfo->personality().Network_Config.Acoord == a) &&
                (nodeInfo->personality().Network_Config.Bcoord == b) &&
                (nodeInfo->personality().Network_Config.Ccoord == c) &&
                (nodeInfo->personality().Network_Config.Dcoord == d) &&
                (nodeInfo->personality().Network_Config.Ecoord == e))
                {
                    return nodeInfo;
                }
        }
    return NULL;
}

BCNodeInfo *
BlockControllerBase::findNodeInfo(string location)
{
     LOGGING_DECLARE_BLOCK_ID_MDC;
     LOGGING_DECLARE_BLOCK_USER_MDC;
     return dynamic_cast<BCNodeInfo*>(_targetLocationMap[location]);
}

BCNodecardInfo *
BlockControllerBase::findNodecardInfo(string location)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    return dynamic_cast<BCNodecardInfo*>(_targetLocationMap[location]);
}

BCLinkchipInfo *
BlockControllerBase::findLinkchipInfo(string location)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    return dynamic_cast<BCLinkchipInfo*>(_targetLocationMap[location]);
}

void
BlockControllerBase::processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    // Find the node target from the location
    std::string jtag = BGQTopology::processorCardNameFromJtagPort(consoleMessage._jtagPort);
    string location = consoleMessage._cardLocation;

    // This is a hack because mc_server is giving us inconsistent messages.  See issue 2509.
    if(location.find("-J") == std::string::npos) // If the location doesn't have the jtag, add it.
        location = location + "-" + jtag;

    BCNodeInfo*  nodeInfo = findNodeInfo(location);
    if (nodeInfo != NULL)    				// did we find a node from the specified location?
        {
            for (vector<string>::iterator it = consoleMessage._lines.begin(); it != consoleMessage._lines.end(); ++it)
                {
                    printConsoleMessage(nodeInfo->_locateId, consoleMessage._cpu, consoleMessage._threadId, *it);
                }

        }
    else
        {
            if(_blockName != "DefaultControlEventListener")
                LOG_ERROR_MSG("console message received with invalid location: " << location);
        }

    // unformatted raw XML
    std::ostringstream msg;
    consoleMessage.write(msg);
    LOG_TRACE_MSG(msg.str());
}

void
BlockControllerBase::printConsoleMessage(unsigned nodeIndex, unsigned cpuNum, unsigned threadId, const string& message)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_TRACE_MSG(__FUNCTION__);
    bool consoleMessagePrinted = false;
    bool replaceChars = false;
    char buf[1024];
    char* buf_ptr = buf;
    unsigned chars_written;
    const char* format = "{%u}.%u.%u: %s\n";

    LOG_TRACE_MSG("Printing console message");

    if (message.length() == 0)
        return;  // nothing to do.


    char *source;
    char *source2;
    const char* format2 = "{%u}.%u.%u: %s";
    char convunit[3];
    unsigned result;
    int midlen;

    // The following code reverts the unprintable characters to their original hex value
    // Since unprintable characters (tab, line feeds, etc.) do not flow through XML properly,
    // MC converts them into   ~^XX^~  and then this code converts them back.
    source = const_cast<char*>( strstr(message.c_str(),"~^") );
    if ((source  != NULL) &&
	isxdigit(source[2]) &&
	isxdigit(source[3]) &&
	(source[4] == '^') &&
	(source[5] == '~') ) {

        replaceChars = true;
        source[0] = '\0';
        chars_written = sprintf(buf, format2, nodeIndex, cpuNum, threadId, message.c_str());
        buf_ptr += chars_written;

        convunit[0] = source[2];
        convunit[1] = source[3];
        convunit[2] = '\0';
        result = strtoul(convunit, 0, 16);

        *(buf_ptr++) = (unsigned char) result;
        *(buf_ptr) = '\0';

        source += 6;

        while(1) {

            source2 = strstr(source,"~^");
            if ((source2  != NULL) &&
                isxdigit(source2[2]) &&
                isxdigit(source2[3]) &&
                (source2[4] == '^') &&
                (source2[5] == '~') ) {

                source2[0] = '\0';
                if (source != source2)
                    strcat(buf,source);

                midlen = strlen(source);
                buf_ptr += midlen;
                source += midlen;

                convunit[0] = source2[2];
                convunit[1] = source2[3];
                convunit[2] = '\0';
                result = strtoul(convunit, 0, 16);

                *(buf_ptr++) = (unsigned char) result;
                *(buf_ptr) = '\0';

                source += 6;
            } else {
                strcat(buf,source);
                strcat(buf,"\n");
                chars_written = strlen(buf);
                break;
            }


        }

    }  else {


        // format the output into a buffer
        chars_written = snprintf(buf, sizeof(buf), format, nodeIndex, cpuNum, threadId, message.c_str());
       if (chars_written >= sizeof(buf))
            {
                //	buf_ptr = (char*) malloc(chars_written + 1);
                buf_ptr = new char[chars_written + 1];
                snprintf(buf_ptr, chars_written+1, "%s", message.c_str());
            }
        for (int buflen = chars_written; buflen && buf[buflen-1] == '\n'; --buflen)
            buf[buflen] = '\0'; // leave one trailing newline /*accessing beyond memory*/

    }

    if ((message.find("Software Test PASS") != string::npos)||(message.find("SoftwareTestPASS") != string::npos)) {
        LOG_TRACE_MSG("Software Test PASS message received.");
        if (_terminatedNodes < 0)
            --_terminatedNodes;
        else
            ++_terminatedNodes;
    } else if ((message.find("Software Test FAIL") != string::npos)||(message.find("SoftwareTestFAIL") != string::npos)) {
        LOG_TRACE_MSG("Software Test FAIL message received.");
        if (_terminatedNodes < 0)
            --_terminatedNodes;
        else {
            ++_terminatedNodes;
            _terminatedNodes = _terminatedNodes * -1;
        }
    }

    // There are various destinations for mailbox output:
    // 1) an optional file specified via the --iologdir parameter at mmcs startup
    // 2) an optional file specified on the connect outfile parameter
    // 3) if neither of the above are specified, the mmcs log is used
    // 4) an optional socket specified via the _redirectSock (mmcs_server only)

    // create prefix for mailbox output to somewhat mimic the log4cxx formatting
    // since we really just care about the timestamp, user, and block.
    //
    //  %d{yyyy-MM-dd HH:mm:ss.SSS} (%-5p) [%t] %c: %m%n
    //
    // The differences between this format and a typical mmcs log4cxx log are:
    //   1) no thread ID
    //   2) no logger name
    //   3) no severity (DEBUG, INFO, etc)
    //
    // note: facet ownership is transferred to the locale, it does not
    // need to be explicitly deleted
    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet( "%Y-%m-%d %H:%M:%s" );
    std::ostringstream logprefix;
    logprefix.imbue( std::locale(logprefix.getloc(), facet) );
    logprefix << boost::posix_time::microsec_clock::local_time() << " {" << _blockName << "} [" << _userName << "]";

    if (_mailboxOutput)
        {
            fprintf(_mailboxOutput, "%s", logprefix.str().c_str());
            fprintf(_mailboxOutput, "%s", buf);
            fflush(_mailboxOutput);
            consoleMessagePrinted = true;
        }

    // log mailbox output to a log file
    if (nodeIndex < getNodes().size())
        {
            BCNodeInfo *nodeInfo = getNodes()[nodeIndex];
            if (nodeInfo->_mailboxOutput)
                {
                    fprintf(nodeInfo->_mailboxOutput, "%s", logprefix.str().c_str());
                    fprintf(nodeInfo->_mailboxOutput, "%s", buf);
                    fflush(nodeInfo->_mailboxOutput);
                    consoleMessagePrinted = true;
                }
        }
    if (!consoleMessagePrinted)
        {
            buf[chars_written-1] = '\0';
            LOG_TRACE_MSG(buf);
            buf[chars_written-1] = '\n';
        }

    // log mailbox output to a socket
    if (_redirectSock != 0)
        {
            CxxSockets::Message msg;
            msg << buf;
            try {
                _redirectSock->Send(msg);
            } catch (CxxSockets::CxxError& e)
                {
                    LOG_WARN_MSG("BlockControllerBase::printConsoleMessage -- I/O error: " << strerror(errno) << " -- terminating redirection");
                    _redirectSock.reset();
                }
        }
    if ((buf_ptr != buf) && (replaceChars == false))
	delete [] buf_ptr;
}

void
BlockControllerBase::printRASMessage(RasEvent& rasEvent)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);

    // There are various destinations for RAS messages:
    // 1) an optional file specified on the rasfile parameter of connect
    // 2) an optional file specified on the outfile parameter of connect.
    //    RAS messages are logged here if the rasfile parameter is not specified
    // 3) The RAS message is always logged in the MMCS log at a debug severity

    // The format of the data written to the RAS log file is the serialized XML format of the RAS event
    if (_rasLogFile)		// rasfile parameter
    {
	ostringstream ostring;
	ostring << rasEvent;
	fprintf(_rasLogFile, "%s\n", ostring.str().c_str());
	fflush(_rasLogFile);
    }
    else if (_mailboxOutput && !_rasLogFile) // outfile parameter specified without rasfile parameter
    {
        // format the RAS message
        ostringstream message;
        message << rasEvent ;
        if (message.str()[message.str().length()-1] != '\n')
            message << "\n";
        string logprefix = string();
        fprintf(_mailboxOutput, "%s", logprefix.c_str());
        fprintf(_mailboxOutput, "%s", message.str().c_str());
        fflush(_mailboxOutput);
    }

    LOG_DEBUG_MSG( rasEvent );
}

void
BlockControllerBase::startMailbox(MMCSCommandReply& reply)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    if(MMCSProperties::getProperty(SECURE_MC) == "true")
        _smboxMonitor.start(reply);
    else
        _mboxMonitor.start(reply);
}

void
BlockControllerBase::stopMailbox()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    if(MMCSProperties::getProperty(SECURE_MC) == "true")
        _smboxMonitor.stop();
    else _mboxMonitor.stop();
}

void
BlockControllerBase::quiesceMailbox(BlockControllerTarget* target)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    time_t now;			// current time
    double secs_since_last_ras_event;
    int    delay_timeout = atoi(MMCSProperties::getProperty(SHUTDOWN_TIMEOUT).c_str()); // get the shutdown timeout

#ifdef WITH_DB
    BGQDB::STATUS result;	// getBlockStatus return code
    BGQDB::BLOCK_STATUS bState;	// getBlockStatus return state
#endif


#if 0				// don't skip _mboxMonitor.stop(), it must closeMailboxListener
    // check if the mailbox monitor is already stopped
    if (!getMailboxMonitor().isStarted())
	return;
#endif

    std::vector<std::string> failed_nodes;

    // wait for either ras events to quiesce or node shutdown completion
    while (delay_timeout > 0 && (_last_ras_event_time || _shutdown_sent_time))
    {
	LOG_INFO_MSG("quiesceMailbox() waiting for ras events and node shutdown");
	sleep(5);			// wait 5 seconds
	delay_timeout -= 5;	        // wait for a maximum of SHUTDOWN_TIMEOUT seconds
	time(&now);			// get current time
	if (_last_ras_event_time)	// wait for RAS events to slow down
	{
	    secs_since_last_ras_event = difftime(now, _last_ras_event_time);
	    if (secs_since_last_ras_event >= 5.0)
		_last_ras_event_time = 0;
	}
	if (_shutdown_sent_time)	// wait for nodes to shut down
	{

            if(MMCSProperties::getProperty(BOOT_COMPLETE) == "true") {
                // Check to see if all of the nodes are done.  If they are, _shutdown_sent_time goes to 0.
                failed_nodes = checkShutdownComplete(target);
                if(failed_nodes.size() == 0)
                    _shutdown_sent_time = 0;
            }
	}

#ifdef WITH_DB
	// check for block in proper state
	if ((result = BGQDB::getBlockStatus(_blockName, bState)) == BGQDB::OK)
	{
	    if (bState == BGQDB::FREE)
	    {
		LOG_WARN_MSG("quiesceMailbox() found block state = FREE, terminating");
		delay_timeout = 0; // exit waiting loop
	    }
	}
#endif
    }
    if (_last_ras_event_time)
	LOG_WARN_MSG("quiesceMailbox() discarding further RAS events");
    if (_shutdown_sent_time) {
	LOG_WARN_MSG("quiesceMailbox() node shutdown did not complete");
        if(MMCSProperties::getProperty(BOOT_COMPLETE) == "true") {
            std::ostringstream logmsg;
            logmsg << "Failed nodes are ";
            for(std::vector<std::string>::iterator it = failed_nodes.begin();
                it != failed_nodes.end(); ++it) {
                logmsg << *it;
            }
            LOG_DEBUG_MSG(logmsg.str());
        }
    }

    _last_ras_event_time = _shutdown_sent_time = 0;

}

void
BlockControllerBase::startRedirection(CxxSockets::SecureTCPSocketPtr sock, MMCSCommandReply& reply)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    _redirectSock = sock;
}

void
BlockControllerBase::resetRedirection() {
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    if(!_redirectSock) return;
    LOG_DEBUG_MSG("Shutting down redirect socket");
    _redirectSock->Shutdown(CxxSockets::Socket::RECEIVE);
    _redirectSock.reset();
}

void
BlockControllerBase::stopRedirection(MMCSCommandReply& reply, bool shutdown)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    if(!_redirectSock) return;
    if(isIOBlock()) {
        std::deque<std::string> args;
        args.push_back("no_shutdown");
        disconnect(args);
    }
    _redirectSock.reset();
    reply << OK << DONE;
}

std::vector<string>
BlockControllerBase::Tokenize(const std::string &rStr,
			  const std::string &szDelimiters)
{
    LOG_TRACE_MSG(__FUNCTION__);
    std::vector<string> words;

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

unsigned
BlockControllerBase::getBootCookie()
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    struct timeval now;
    unsigned cookie;

    gettimeofday(&now, NULL);	// get time in secs, usecs
    // high order byte = # secs since last minute
    // low order 3 bytes = # microseconds since last second
    cookie = ((now.tv_sec % 60) << 24) | (now.tv_usec & 0x00FFFFFF);

    // save it in the object
    _savedBootCookie = _bootCookie = cookie;
    return cookie;
}

bool
BlockControllerBase::parseSteps(MMCSCommandReply& reply, MCServerMessageSpec::BootBlockRequest& req, std::string& steps)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    // Rip out the 'steps={' text
    steps.erase(steps.begin(), steps.begin() + steps.find("=") + 1);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(",");
    tokenizer tok(steps, sep);
    int vector_index = 0;
    bool gotvkr = false;
    BOOST_FOREACH(std::string curr_step, tok) {
        if(curr_step == "enableNodes") {
            req._bootSteps.push_back(BootStep::enableNodes);
        } else if(curr_step == "initDevbus") {
            req._bootSteps.push_back( BootStep::initDevbus);
        } else if(curr_step == "startTraining") {
            req._bootSteps.push_back( BootStep::startTraining);
        } else if(curr_step == "enableLinkChips") {
            req._bootSteps.push_back( BootStep::enableLinkChips);
        } else if(curr_step == "trainLinkChips") {
            req._bootSteps.push_back( BootStep::trainLinkChips);
        } else if(curr_step == "trainTorus") {
            req._bootSteps.push_back( BootStep::trainTorus);
        } else if(curr_step == "installFirmware") {
            req._bootSteps.push_back(BootStep::installFirmware);
        } else if(curr_step == "installPersonalities") {
            req._bootSteps.push_back( BootStep::installPersonalities);
        } else if(curr_step == "startCores") {
            req._bootSteps.push_back( BootStep::startCores);
        } else if(curr_step == "verifyMailbox") {
            req._bootSteps.push_back( BootStep::verifyMailbox);
        } else if(curr_step == "monitorMailbox") {
            req._bootSteps.push_back( BootStep::monitorMailbox);
        } else if(curr_step == "verifyMailboxReady") {
            req._bootSteps.push_back( BootStep::verifyMailboxReady);
        } else if(curr_step == "installKernelImages") {
            req._bootSteps.push_back( BootStep::installKernelImages);
        } else if(curr_step == "configureDomains") {
            req._bootSteps.push_back( BootStep::configureDomains);
        } else if(curr_step == "launchKernels") {
            req._bootSteps.push_back( BootStep::launchKernels);
        } else if(curr_step == "verifyKernelReady") {
            req._bootSteps.push_back( BootStep::verifyKernelReady);
            gotvkr = true;
        } else if(curr_step == "noop") {
            req._bootSteps.push_back( BootStep::noop);
        } else if(curr_step == "cardNoop") {
            req._bootSteps.push_back( BootStep::cardNoop);
        } else {
            reply << FAIL << "Invalid step " << curr_step << " specified" << DONE;
            return false;
        }
        ++vector_index;
    }
    if(!gotvkr && (steps.length() != 0))
        _check_complete = false;  // We got some steps, and verifyKernelReady wasn't one of them.
    return true;
}

bool
BlockControllerBase::parseDomains(MCServerMessageSpec::BootBlockRequest& req, std::string& domains)
{
    LOGGING_DECLARE_BLOCK_ID_MDC;
    LOGGING_DECLARE_BLOCK_USER_MDC;
    LOG_DEBUG_MSG(__FUNCTION__);
    int open, close; open = close = 0;
    for(unsigned int cnt = 0; cnt < domains.length(); ++cnt) {
        if(domains[cnt] == '{')
            ++open;
        else if(domains[cnt] == '}')
            ++close;
    }

    if(open != close) {
        LOG_ERROR_MSG("Mismatched braces in " << domains);
        return false;
    }

    // Initialize the domain vector in the boot block request
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep("}");

    tokenizer tok(domains, sep);
    BOOST_FOREACH(std::string curr_dom, tok) {
        // Create the domain object
        MCServerMessageSpec::BootBlockRequest::Domain msgdomain;

        // Default value
        msgdomain._configAddress = 0;

        // Rip out the 'domain={' text
        curr_dom.erase(curr_dom.begin(), curr_dom.begin() + curr_dom.find("{") + 1);

        // Break down the fields
        boost::char_separator<char> osep(" ");
        boost::char_separator<char> dollar_sep("$");
        tokenizer domint(curr_dom, osep);
        BOOST_FOREACH(std::string field, domint) {
            if(field.find("cores=") != std::string::npos) {
                // pull out 'cores='
                field.erase(field.begin(), field.begin() + 6);
                tokenizer cores(field, dollar_sep);
                tokenizer::iterator core_tok = cores.begin();
                try {
                    msgdomain._startCore = boost::lexical_cast<unsigned>(*core_tok);
                } catch(boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("Could not read start core value " << *core_tok);
                }
                ++core_tok;

                int ec = 0;
                try {
                    ec = boost::lexical_cast<int>(*core_tok);
                } catch(boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("Could not read end core value " << *core_tok);
                }
                if(ec != -1)
                    msgdomain._endCore = (unsigned)(ec);
                else
                    msgdomain._endCore = BGQ_MAX_CORE;
            } else if(field.find("custaddr=") != std::string::npos) {
                // pull out 'custaddr='
                field.erase(field.begin(), field.begin() + 9);

                if ( field.substr(0,2) == "0x" ) {
                    // strip off 0x hexadecimal prefix
                    field.erase(0,2);
                    istringstream ca(field);
                    ca >> std::hex >> msgdomain._configAddress;
                    if ( !ca ) {
                        LOG_ERROR_MSG("Could not read node configuration address: " << field);
                    }
                } else {
                    // assume decimal
                    istringstream ca(field);
                    ca >> msgdomain._configAddress;
                    if ( !ca ) {
                        LOG_ERROR_MSG("Could not read node configuration address: " << field);
                    }
                }
            } else if(field.find("memory=") != std::string::npos) {
                // pull out 'memory='
                field.erase(field.begin(), field.begin() + 7);
                tokenizer memory(field, dollar_sep);
                tokenizer::iterator mem_tok = memory.begin();

                std::string res;
                std::string memstr = *mem_tok;
                istringstream sa(memstr);

                // Check if its hex
                if(memstr.substr(0,2) == "0x") {
                    memstr.erase(0,2);
                    if (!(sa >> std::hex >> res)  ||
                        !(boost::algorithm::all( memstr, boost::algorithm::is_xdigit()))) {
                        LOG_ERROR_MSG("Could not read memory start value " << *mem_tok);
                    }
                    msgdomain._startAddress = strtoll(memstr.c_str(), 0, 16);
                } else {
                    msgdomain._startAddress = strtoll(memstr.c_str(), 0, 10);
                }

                ++mem_tok;

                memstr = *mem_tok;
                istringstream ea(memstr);

                // Check if its hex
                if(memstr.substr(0,2) == "0x") {
                    memstr.erase(0,2);
                    if (!(ea >> std::hex >> res)  ||
                        !(boost::algorithm::all( memstr, boost::algorithm::is_xdigit()))) {
                        LOG_ERROR_MSG("Could not read memory end value " << *mem_tok);
                    }
                    msgdomain._endAddress = strtoll(memstr.c_str(), 0, 16);
                } else {
                    msgdomain._endAddress = strtoll(memstr.c_str(), 0, 10);
                }
            } else if(field.find("images=") != std::string::npos) {
                // pull out 'images='
                field.erase(field.begin(), field.begin() + 7);
                msgdomain._imageSet = field;
            } else if(field.find("options=") != std::string::npos) {
                // pull out 'options='
                field.erase(field.begin(), field.begin() + 8);
                msgdomain._options = field;
            } else if(field.find("id=") != std::string::npos) {
                // slurp this option, it is not used in boot request
            } else {
                // assume option
                msgdomain._options += " ";
                msgdomain._options += field;
            }
        }
        if(!msgdomain._imageSet.empty())
            req._domains.push_back(msgdomain);
    }
    return true;
}

std::vector<std::string> BlockControllerBase::checkShutdownComplete(BlockControllerTarget* target) {
    LOG_DEBUG_MSG(__FUNCTION__);
    MCServerMessageSpec::VerifyKernelShutdownRequest shutreq;
    shutreq._cookie = _bootCookie;
    MCServerMessageSpec::VerifyKernelShutdownReply   shutrep;

    if(target != 0) {
        // Just going to do a subset of nodes.
        BOOST_FOREACH(BCNodeInfo* node, target->getNodes()) {
            shutreq._location.push_back(node->_location);
        }
    } else {
        for(unsigned int i = 0; i < getNodes().size(); ++i) {
            BCNodeInfo* ni = getNodes()[i];
            if(ni->_linkio == false)
                shutreq._location.push_back(ni->_location);
        }
    }
    if(!hardWareAccessBlocked()) // Don't do the check if the hardware is unavailable.
        _mcServer->verifyKernelShutdown(shutreq, shutrep);
    if(shutrep._compute_not_halted.size() == 0) {
        LOG_INFO_MSG("All nodes halted");
    } else
        LOG_DEBUG_MSG(shutrep._compute_not_halted.size() << " nodes not yet completed.");
    return shutrep._compute_not_halted;
}

bool BlockControllerBase::checkComplete(MMCSCommandReply& reply,
                                        std::vector<std::string>& bad_node_locs,
                                        std::vector<std::string>& good_nodes,
                                        MCServerMessageSpec::VerifyKernelReadyRequest& bootreq,
                                        unsigned nodecount) {
    LOG_TRACE_MSG(__FUNCTION__);
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);	// serialize this code

    std::ostringstream locations;
    unsigned bad_locs = 0;

    MCServerMessageSpec::VerifyKernelReadyReply bootrep;
    if(_mcServer && !hardWareAccessBlocked())
        _mcServer->verifyKernelReady(bootreq, bootrep);
    else
        return false;
    LOG_DEBUG_MSG(bootrep._compute_not_ready.size() << " nodes not yet ready.");
    bad_node_locs = bootrep._compute_not_ready;

    // Mark the good ones 'initialized'.  'Good' nodes are nodes that are
    // not in the list of bad ones returned by verifyKernelReady AND are
    // not in the list of nodes that were in error when we started.
    BOOST_FOREACH(std::string& curr_node, bootreq._location) {
        if(std::find(bad_node_locs.begin(), bad_node_locs.end(), curr_node) == bad_node_locs.end()) {
            // Succeeded. 
            if(std::find(_error_nodes.begin(), _error_nodes.end(), curr_node) == _error_nodes.end()) {
                // Also, don't put it in the good list if it was marked in error.
                BCNodeInfo* node = dynamic_cast<BCNodeInfo*>(_targetLocationMap[curr_node]);
                node->_initialized = true;
                good_nodes.push_back(curr_node);
            }
        }
    }

    // Remove any of the errored nodes from the bad list.
    BOOST_FOREACH(std::string& enode, _error_nodes) {
        std::vector<std::string>::iterator nodeit =
            std::find(bootrep._compute_not_ready.begin(),
                      bootrep._compute_not_ready.end(),
                      enode);
        if(nodeit != bootrep._compute_not_ready.end()) {
            // This error node is in the list of compute_not_ready nodes.
            // Remove it.
            bootrep._compute_not_ready.erase(std::remove(bootrep._compute_not_ready.begin(),
                                                         bootrep._compute_not_ready.end(),
                                                         enode), bootrep._compute_not_ready.end());
        }
    }

    if(bootrep._compute_not_ready.size() == 0) {
        return true;  // None waiting.  We're done!
    }

    bad_locs = bootrep._compute_not_ready.size();
    for(unsigned int i =  0; i < bootrep._compute_not_ready.size(); ++i) {
        locations << bootrep._compute_not_ready[i] << ", ";
    }

    std::ostringstream repmsg;

    if(bad_locs <= 128)
        repmsg << "Nodes " << locations.str() << " failed to complete boot processing.";
    else if(bad_locs == bootreq._location.size())
        repmsg << "All nodes failed to complete boot processing.";
    else
        repmsg << bad_locs << " nodes failed to complete boot processing.";

    bootrep._compute_not_ready.clear();
    LOG_TRACE_MSG(repmsg.str());
    reply << FAIL << repmsg.str();
    return false;
}

bool
BlockControllerBase::openLog(BCNodeInfo* node)
{
    if ( !node ) return false;

    std::string logpath = MMCSProperties::getProperty(MMCS_LOGDIR);
    if ( logpath.empty() ) {
        node->_mailboxOutput = NULL;
        return true;
    }

    // close previous log if left open
    if ( node->_mailboxOutput ) {
        if ( fclose(node->_mailboxOutput) ) {
            const int error = errno;
            LOG_WARN_MSG(
                    "could not close descriptor " << fileno( node->_mailboxOutput) << " for " <<
                    node->location() << ": " << strerror(error)
                    );

            node->_mailboxOutput = NULL;
        } else {
            LOG_TRACE_MSG( "closed log file for " << node->location() );
        }
    }

    logpath.append( "/" );
    logpath.append( node->location() );
    logpath.append( ".log" );

    node->_mailboxOutput = fopen( logpath.c_str(), "a");
    if ( !node->_mailboxOutput ) {
        const int error = errno;
        LOG_WARN_MSG( "could not open log file: " << logpath );
        LOG_WARN_MSG( "errno " << error << " (" << strerror(error) << ")" );
        return false;
    }

    LOG_INFO_MSG("{" << node->_locateId << "} I/O log file: " << logpath);
    return true;
}

bool BlockControllerBase::hardWareAccessBlocked() {
    // If we're blocking anything, check to see if it's anything I need.
    if(HardwareBlockList::list_size() != 0) {
        // See if any of my hardware is in the list.
        std::string rackstring;
        BOOST_FOREACH(BCIconInfo*& icon, _icons) {
            rackstring = icon->location().substr(0,3);
            if(HardwareBlockList::find_in_list(rackstring) == true)
                return true;
        }
    }
    return false;
}
