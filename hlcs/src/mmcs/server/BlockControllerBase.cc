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

#include "BlockControllerBase.h"

#include "BCLinkchipInfo.h"
#include "BCNodecardInfo.h"
#include "BCNodeInfo.h"
#include "BlockControllerTarget.h"
#include "BlockHelper.h"
#include "HardwareBlockList.h"
#include "IOBlockController.h"
#include "SecureConnect.h"

#include "common/Properties.h"

#include <control/include/mcServer/defaults.h>
#include <control/include/mcServer/MCServer_errno.h>

#include <ras/include/RasEventImpl.h>
#include <ras/include/RasEventHandlerChain.h>

#include <utility/include/Log.h>

#include <utility/include/cxxsockets/SecureTCPSocket.h>

#include <utility/include/portConfiguration/PortConfiguration.h>

#include <xml/include/c_api/MCBootSteps.h>

#include <boost/filesystem/path.hpp>
#include <boost/scoped_array.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

LOG_DECLARE_FILE( "mmcs.server" );

using namespace std;

using mmcs::common::Properties;

namespace mmcs {
namespace server {

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
    _mcServer(),
    _numNodesStarted(0),
    _numNodesTotal(0),
    _numComputesTotal(0),
    _bootCookie(0),
    _targetsetMode(MCServerMessageSpec::WUAR),
    _diags(false),
    _insertDiagsRAS(false),
    _mailboxOutput(NULL),
    _rasLogFile(NULL),
    _mutex(PTHREAD_MUTEX_RECURSIVE_NP),
    _blockThreads(0),
    _disconnecting(false),
    _userName(userName),
    _blockName(blockName),
    _isstarted(false),
    _isshared(false),
    _boot_block_start(),
    _io_linkless_midplanes(),
    _delmachine(delete_machine),
    _terminatedNodes(0),
    _do_barrier_ack(false),
    _block_shut_down(false),
    _check_complete(true),
    _rebooting(false),
    _domains(),
    _machine_config_data()
{
    BOOST_ASSERT( !_userName.empty() );
    BOOST_ASSERT( !_blockName.empty() );
}

BlockControllerBase::~BlockControllerBase()
{
    delete_block();
    if (_machineXML && _delmachine) {
        delete _machineXML;
        _machineXML = NULL;
    }

    if (_mboxMonitor.isStarted()) {
        _mboxMonitor.stop();
    }
}

void
BlockControllerBase::pgood_reset(
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget
        )
{
    string pgood_regexp; // regular expression for PgoodResetRequest message target

    const bool isIoBlock( this->isIoBlock() );

    // build a regular expression for the open compute and I/O nodes in the block
    for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
        BCNodeInfo *nodeInfo = pTarget->getNodes()[i];
        if ((isIoBlock && nodeInfo->isIOnode()) || (!isIoBlock && !nodeInfo->isIOnode())) {
            // create a target set for sending pgood to the processor nodes
            if (pgood_regexp.empty())
                pgood_regexp.append("(").append(nodeInfo->location());
            else
                pgood_regexp.append("|").append(nodeInfo->location());
        }
    }

    // send a pgood reset request to the open compute and I/O nodes
    if (!pgood_regexp.empty()) {
        pgood_regexp.append(")");

        MCServerMessageSpec::PgoodResetRequest mcPgoodResetRequest;
        mcPgoodResetRequest._expression.push_back(pgood_regexp);
        MCServerMessageSpec::PgoodResetReply   mcPgoodResetReply;
        try {
            LOG_TRACE_MSG( "Sending pgood request." );
            getMCServer()->pgoodReset(mcPgoodResetRequest, mcPgoodResetReply);
        } catch (const std::exception &e) {
            mcPgoodResetReply._rc = -1;
            mcPgoodResetReply._rt = e.what();
        } catch (const XML::Exception &e) {
            ostringstream buf; buf << e;
            mcPgoodResetReply._rc = -1;
            mcPgoodResetReply._rt = buf.str();
        }

        if (mcPgoodResetReply._rc) {
            reply << mmcs_client::FAIL << mcPgoodResetReply._rt << mmcs_client::DONE;
            return;
        }
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

bool
BlockControllerBase::isIoBlock() const
{
    return dynamic_cast<const IOBlockController*>(this);
}

void
BlockControllerBase::deleteTargets()
{
    // delete BlockControllerBase targets
    for (unsigned i = 0; i < getNodes().size(); ++i)
        delete getNodes()[i];
    getNodes().clear();

    for (unsigned i = 0; i < getLinkchips().size(); ++i)
        delete getLinkchips()[i];

    getLinkchips().clear();

    for (unsigned i = 0; i < getIcons().size(); ++i) {
        BCIconInfo* icon = getIcons()[i];
        for (unsigned j = 0; j < dynamic_cast<BCNodecardInfo*>(icon)->_linkChips.size(); ++j) {
            delete dynamic_cast<BCNodecardInfo*>(icon)->_linkChips[j];
        }
        delete icon;
    }
    getIcons().clear();

    _targetLocationMap.clear();

    getTargets().clear();
}

int
BlockControllerBase::makeAndOpenTargetSet(
        MCServerRef* const ref,
        const bool temp_targetset,
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget
        )
{
    // create the target set for the block resources
    MCServerMessageSpec::MakeTargetSetRequest mcMakeTargetSetRequest(_blockName, _userName, temp_targetset);

    // Mark all specified targets as opened
    for (unsigned i = 0; i < pTarget->getTargets().size(); ++i) {
        BCTargetInfo* const targetInfo = pTarget->getTargets()[i];
        targetInfo->_open = true; // mark this target as open
    }

    if (_isshared == false) { // if we are sharing node cards, don't include them in the target set
        // ensure that node cards for the targeted nodes are included in the target set
        for (unsigned i = 0; i < pTarget->getNodes().size(); ++i) {
            BCNodeInfo* const targetInfo = pTarget->getNodes()[i];
            const string nodecardLocation(targetInfo->location().substr(0,10));
            BCNodecardInfo* const nodecardInfo = static_cast<BCNodecardInfo*>(_targetLocationMap[nodecardLocation]);
            if (nodecardInfo != NULL && nodecardInfo->_passthru_only == false)
                nodecardInfo->_open = true; // mark this target as open
        }
    }

    // Add all targeted nodes, node cards link chips and link cards to the target set
    for (unsigned i = 0; i < getTargets().size(); ++i) {
        BCTargetInfo* const targetInfo = getTargets()[i];
        if (targetInfo->_open && targetInfo->_linkio == false && targetInfo->_passthru_only == false) {
            mcMakeTargetSetRequest._location.push_back(targetInfo->location());
            // LOG_TRACE_MSG( "Adding " << targetInfo->location() << " to target set " << _blockName );
        }
    }

    MCServerMessageSpec::MakeTargetSetReply mcMakeTargetSetReply;
    try {
        ref->makeTargetSet(mcMakeTargetSetRequest, mcMakeTargetSetReply);
    } catch (const std::exception& e) {
        mcMakeTargetSetReply._rc = -1;
        mcMakeTargetSetReply._rt = e.what();
    }

    if (mcMakeTargetSetReply._rc) {
        if (mcMakeTargetSetReply._rc != BGERR_MCSERVER_TARGETSET_EXISTS) {
            LOG_ERROR_MSG("Error making target set: " << mcMakeTargetSetReply._rt);
            reply << mmcs_client::FAIL << mcMakeTargetSetReply._rt << mmcs_client::DONE;
            deque<string> dq;
            mmcs_client::CommandReply bogus;
            disconnect(dq, bogus);
            return 0;
        }
    }

    LOG_TRACE_MSG("Target set " << _blockName << " created.");

    const MCServerMessageSpec::OpenTargetRequest mcOpenTargetSetRequest(
            _blockName,
            _userName,
            _targetsetMode,
            true
            );
    MCServerMessageSpec::OpenTargetReply mcOpenTargetSetReply;
    try {
        ref->openTarget(mcOpenTargetSetRequest, mcOpenTargetSetReply);
    } catch (const std::exception &e) {
        mcOpenTargetSetReply._rc = -1;
        mcOpenTargetSetReply._rt = e.what();
    }

    if (mcOpenTargetSetReply._rc) {
        LOG_ERROR_MSG("Error opening target set: " << mcOpenTargetSetReply._rt);
        reply << mmcs_client::FAIL << "Error opening target set: " << mcOpenTargetSetReply._rt << mmcs_client::DONE;
        deque<string> dq;
        mmcs_client::CommandReply bogus;
        disconnect(dq, bogus);
        return 0;
    } else {
        LOG_TRACE_MSG("Target set " << _blockName << " opened.");
    }

    return mcOpenTargetSetReply._handle;
}

void
BlockControllerBase::connect(
        deque<string> args,
        mmcs_client::CommandReply& reply,
        const BlockControllerTarget* pTarget
        )
{
    bool temp_targetset = true;
    string szOutFile;
    string szRasFile;

    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    if (isConnected()) {
        reply << mmcs_client::FAIL << "Block is already connected." << mmcs_client::DONE;
        return;
    }

    if (pTarget == NULL || pTarget->getTargets().empty()) {
        reply << mmcs_client::FAIL << "No targets selected." << mmcs_client::DONE;
        return;
    }

    if (hardWareAccessBlocked()) {
        std::ostringstream msg;
        msg << "SubnetMc managing hardware for " << _blockName << " is temporarily unavailable.";
        LOG_ERROR_MSG(msg.str());
        reply << mmcs_client::FAIL << msg.str() << mmcs_client::DONE;
        return;
    }

    _targetsetMode = MCServerMessageSpec::WUAR;
    for (unsigned i = 0; i < args.size(); ++i) {
        const vector<string> tokens( Tokenize(args[i], "=") );

        if (tokens[0] == "outfile")                { if (tokens.size() == 2) { szOutFile = tokens[1];  continue; } }
        else if (tokens[0] == "rasfile")           { if (tokens.size() == 2) { szRasFile = tokens[1];  continue; } }
        else if (tokens[0] == "mode") {
            if (tokens.size() == 2)
            {
                if (tokens[1]      == "service")   { _targetsetMode = MCServerMessageSpec::WENR; continue; }
                else if (tokens[1] == "reserve")   { _targetsetMode = MCServerMessageSpec::WUAR; continue; }
                else if (tokens[1] == "monitor")   { _targetsetMode = MCServerMessageSpec::RAAW; continue; }
                else if (tokens[1] == "debug")     { _targetsetMode = MCServerMessageSpec::WUAR; continue; }
                else if (tokens[1] == "control")   { _targetsetMode = MCServerMessageSpec::WUAR; continue; }
                else
                {
                    reply << mmcs_client::FAIL << "Unrecognized argument: " << tokens[0] << "=" << tokens[1] << mmcs_client::DONE;
                    return;
                }
            }
        }
        else if (tokens[0] == "targetset")
        {
            if (tokens.size() == 2) {
                if (tokens[1]       == "temp") { temp_targetset = true;    continue; }
                else if (tokens[1]  == "perm") { temp_targetset = false;   continue; }
                else
                {
                    reply << mmcs_client::FAIL << "Unrecognized argument: " << tokens[0] << "=" << tokens[1] << mmcs_client::DONE;
                    return;
                }
            }
        }
    }

    // if outfile was specified on connect, attempt to open it here.
    if (!szOutFile.empty()) {
        if ( !boost::filesystem::path(szOutFile).is_complete() ) {
            reply << mmcs_client::FAIL << "Outfile " << szOutFile << " is not fully qualified." << mmcs_client::DONE;
            return;
        }
        // glibc specific x flag for exclusive
        FILE* const pFile = fopen(szOutFile.c_str(), "wx");
        if (pFile == NULL) {
            reply << mmcs_client::FAIL << "Cannot open outfile: " << szOutFile << " - " << strerror(errno) << mmcs_client::DONE;
            return;
        }
        _mailboxOutput = pFile;
    }  else {
        _mailboxOutput = NULL;
    }

    // If ras file was specified on connect, attempt to open it here.
    if (!szRasFile.empty()) {
        if ( !boost::filesystem::path(szRasFile).is_complete() ) {
            reply << mmcs_client::FAIL << "rasfile " << szRasFile << " is not fully qualified." << mmcs_client::DONE;
            return;
        }
        // glibc specific x flag for exclusive
        FILE* const pFile = fopen(szRasFile.c_str(), "wx");
        if (pFile == NULL) {
            reply << mmcs_client::FAIL << "Cannot open rasfile: " << szRasFile << " - " << strerror(errno) << mmcs_client::DONE;
            return;
        }
        _rasLogFile = pFile;
    } else {
        _rasLogFile = NULL;
    }

    // Establish a socket connection to mcServer
    if (!isConnected()) {
        BlockControllerBase::mcserver_connect(_mcServer, _userName, reply);
        if (reply.getStatus() != 0) {
            deque<string> dq;
            mmcs_client::CommandReply bogus;
            disconnect(dq, bogus);
            return;
        } else {
            LOG_DEBUG_MSG("Successfully connected to mcServer.");
        }
    }
    _targetsetHandle = makeAndOpenTargetSet(_mcServer, temp_targetset, reply, pTarget);
}

void
BlockControllerBase::disconnect(
        const deque<string>& args,
        mmcs_client::CommandReply& reply
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    if (!isConnected()) {
        reply << mmcs_client::FAIL << "Block is not connected." << mmcs_client::DONE;
        return;
    }

    const boost::posix_time::ptime bst( _boot_block_start );
    if ( !bst.is_not_a_date_time() ) { // We're waiting on a boot
        reply << mmcs_client::FAIL << "Block has been booting since " << bst << mmcs_client::DONE;
        return;
    }

    if (isStarted()) {
        if (std::find(args.begin(), args.end(), "no_shutdown") == args.end()) {
            try {
                shutdown_block(reply, args);
            } catch (const std::exception& e) {
                reply << mmcs_client::FAIL << e.what() << mmcs_client::DONE;
            }

            if (reply.getStatus() != 0) {
                LOG_ERROR_MSG("Error on block shutdown: " << reply.str());
                reply.reset();
            }
        }
    }

    stopMailbox();// terminate the mailbox polling thread

    PthreadMutexHolder mb_mutex;
    mb_mutex.Lock(&_mailboxLockBox);

    if ((_mailboxOutput != NULL) && (_mailboxOutput != stdout)) {
        fclose(_mailboxOutput);
    }

    _mailboxOutput = stdout;
    mb_mutex.Unlock();

    if (_rasLogFile) {
        fclose(_rasLogFile);
    }
    _rasLogFile = NULL;

    // Close the target set
    try {
        const MCServerMessageSpec::CloseTargetRequest mcCloseTargetSetRequest(_blockName, "", _targetsetMode, _targetsetHandle);
        MCServerMessageSpec::CloseTargetReply mcCloseTargetSetReply;
        _mcServer->closeTarget(mcCloseTargetSetRequest, mcCloseTargetSetReply);
        if (mcCloseTargetSetReply._rc) {
            LOG_ERROR_MSG("Error closing target set: " << mcCloseTargetSetReply._rt);
        } else {
            LOG_TRACE_MSG("Target set " << _blockName << " closed.");
        }

        // Delete the target set
        const MCServerMessageSpec::DeleteTargetSetRequest mcDeleteTargetSetRequest(_blockName, _userName);
        MCServerMessageSpec::DeleteTargetSetReply mcDeleteTargetSetReply;
        _mcServer->deleteTargetSet(mcDeleteTargetSetRequest, mcDeleteTargetSetReply);
        if (mcDeleteTargetSetReply._rc) {
            LOG_ERROR_MSG("Error deleting target set: " << mcDeleteTargetSetReply._rt);
        } else {
            LOG_TRACE_MSG("Target set " << _blockName << " deleted.");
        }
    } catch (const std::exception& e) {
        LOG_ERROR_MSG(__FUNCTION__ << "() " << e.what());
    }

    // Close socket connection to mcServer
    MCServerRef* tempRef = _mcServer;
    _mcServer = NULL;
    delete tempRef;

    // Mark the targets as closed
    for (vector<BCTargetInfo*>::iterator iter = getTargets().begin(); iter != getTargets().end(); ++iter) {
        (*iter)->_open = false; // mark this target as closed
    }

    // Reset CNBlockController status
    _numNodesStarted = 0;
    _terminatedNodes  = 0;
    _disconnecting = false;
    _do_barrier_ack = false;

    reply << mmcs_client::OK << mmcs_client::DONE;
}

void
BlockControllerBase::initMachineConfig(
        mmcs_client::CommandReply& reply
        )
{
    if (!_machineXML) {
        _helper->initMachineConfig(reply);
    }

    if (reply.getStatus() == mmcs_client::CommandReply::STATUS_NOT_SET) {
        reply << mmcs_client::OK << mmcs_client::DONE;
    }
}

void
BlockControllerBase::delete_block()
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    // Delete BlockControllerBase targets
    deleteTargets();

    if (_blockXML) {
        delete _blockXML;
        _blockXML = NULL;
    }

    if (_block) {
        delete _block;
        _block = NULL;
    }
}

void
BlockControllerBase::mcserver_connect(
        MCServerRef*& mcServer,
        string userName,
        mmcs_client::CommandReply& reply
        )
{
    // Establish a connection to mcServer
    const std::string ip = Properties::getProperty(MC_SERVER_IP);
    const std::string port = Properties::getProperty(MC_SERVER_PORT);
    try {
        bgq::utility::PortConfiguration::Pairs portpairs;
        bgq::utility::PortConfiguration::parsePortsStr(ip + ":" + port, boost::lexical_cast<std::string>(mc_server::ClientPort), portpairs);

        CxxSockets::SecureTCPSocketPtr sock;
        SecureConnect::Connect(portpairs, sock);
        mcServer = new MCServerRef(sock);
    } catch (const std::exception& e) {
        reply << mmcs_client::FAIL << "Unable to connect to mc_server: " << e.what() << mmcs_client::DONE;
        return;
    }

    // Sign in to the mcServer
    char hostname[32];
    gethostname(hostname, 32);
    hostname[31]='\0';
    const MCServerMessageSpec::ConnectRequest connectRequest(atoi(MCServerMessageSpec::VERSION.c_str()), userName, hostname);
    MCServerMessageSpec::ConnectReply connectReply;
    try {
        mcServer->connect(connectRequest, connectReply);
    } catch (const std::exception &e) {
        connectReply._rc = -1;
        connectReply._rt = e.what();
    }

    if (connectReply._rc) {
        reply << mmcs_client::FAIL << __FUNCTION__ << ": " << connectReply._rt << mmcs_client::DONE;
        return;
    }
    reply << mmcs_client::OK << mmcs_client::DONE;
}

int
BlockControllerBase::decrBlockThreads()
{
    int numThreads;
    numThreads = --_blockThreads;
    return numThreads;
}

int
BlockControllerBase::incrBlockThreads()
{
    int numThreads;
    numThreads = ++_blockThreads;
    return numThreads;
}

int
BlockControllerBase::getBlockThreads()
{
    int numThreads;

    numThreads = _blockThreads;
    return numThreads;
}

bool
BlockControllerBase::readyToDie()
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);
    if (_blockThreads == 0 && (_disconnecting || _mcServer == 0)) {
        return true;
    } else {
        return false;
    }
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
BlockControllerBase::setDisconnecting(
        bool disconnectOption,
        const std::string& disconnectReason
        )
{
    PthreadMutexHolder mutex;

    if (_disconnecting == true)
        return;

    mutex.Lock(&_mutex);
    if (disconnectOption == true && _disconnecting == false) {
        _disconnecting = true;
        _disconnectReason = disconnectReason;
    } else if (disconnectOption == false) {
        _disconnecting = false;
        _disconnectReason.clear();
    }
    mutex.Unlock();
}

BCNodeInfo *
BlockControllerBase::findNodeInfo(
        const unsigned a,
        const unsigned b,
        const unsigned c,
        const unsigned d,
        const unsigned e
        )
{
    for (unsigned i = 0; i < getNodes().size(); ++i) {
        BCNodeInfo* const nodeInfo = getNodes()[i];
        if (
            (nodeInfo->personality().Network_Config.Acoord == a) &&
            (nodeInfo->personality().Network_Config.Bcoord == b) &&
            (nodeInfo->personality().Network_Config.Ccoord == c) &&
            (nodeInfo->personality().Network_Config.Dcoord == d) &&
            (nodeInfo->personality().Network_Config.Ecoord == e)
           )
        {
            return nodeInfo;
        }
    }
    return NULL;
}

BCNodeInfo*
BlockControllerBase::findNodeInfo(
        const std::string& location
        ) const
{
    const std::map<std::string,BCTargetInfo*>::const_iterator result = _targetLocationMap.find( location );
    if ( result == _targetLocationMap.end() )
        return NULL;

    return dynamic_cast<BCNodeInfo*>( result->second );
}

BCNodecardInfo*
BlockControllerBase::findNodeBoardInfo(
        const std::string& location
        )
{
    return dynamic_cast<BCNodecardInfo*>(_targetLocationMap[location]);
}

void
BlockControllerBase::processConsoleMessage(
        const MCServerMessageSpec::ConsoleMessage& consoleMessage
        ) 
{
    // Find the node target from the location
    const std::string jtag = BGQTopology::processorCardNameFromJtagPort(consoleMessage._jtagPort);
    string location = consoleMessage._cardLocation;

    // This is a hack because mc_server is giving us inconsistent messages. See issue 2509.
    if (location.find("-J") == std::string::npos) // If the location doesn't have the jtag, add it.
        location = location + "-" + jtag;

    const BCNodeInfo* nodeInfo = findNodeInfo(location);
    if (nodeInfo != NULL) {   // Did we find a node from the specified location?
        for (vector<string>::const_iterator it = consoleMessage._lines.begin(); it != consoleMessage._lines.end(); ++it) {
            printConsoleMessage(nodeInfo->_locateId, consoleMessage._cpu, consoleMessage._threadId, *it);
        }
    } else if(_blockName != mc_server::DefaultListener) {
        LOG_ERROR_MSG("Console message received with invalid location: " << location);
    }

    // LOG_TRACE_MSG( consoleMessage );
}

void
BlockControllerBase::printConsoleMessage(
        unsigned nodeIndex,
        unsigned cpuNum,
        unsigned threadId,
        const string& message
        ) 
{
    if (message.empty()) {
        return;
    }

    bool consoleMessagePrinted = false;
    unsigned chars_written;
    const char* format = "{%u}.%u.%u: %s\n";

    const size_t buffer_size = message.size() + 100; // overhead for format string
    boost::scoped_array<char> buf(new char[buffer_size]);

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

        source[0] = '\0';
        chars_written = snprintf(buf.get(), buffer_size, format2, nodeIndex, cpuNum, threadId, message.c_str());
        char* buf_ptr = buf.get();
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
                    strcat(buf.get(),source);

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
                strcat(buf.get(),source);
                strcat(buf.get(),"\n");
                chars_written = strlen(buf.get());
                break;
            }
        }
    }  else {
        // format the output into a buffer
        chars_written = snprintf(buf.get(), buffer_size, format, nodeIndex, cpuNum, threadId, message.c_str());
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
    // Note: facet ownership is transferred to the locale, it does not need to be explicitly deleted.
    boost::posix_time::time_facet* facet = new boost::posix_time::time_facet( "%Y-%m-%d %H:%M:%s" );
    std::ostringstream logprefix;
    logprefix.imbue( std::locale(logprefix.getloc(), facet) );
    logprefix << boost::posix_time::microsec_clock::local_time() << " {" << _blockName << "} [" << _userName << "]";

    if (_mailboxOutput) {
        fprintf(_mailboxOutput, "%s", logprefix.str().c_str());
        fprintf(_mailboxOutput, "%s", buf.get());
        fflush(_mailboxOutput);
        consoleMessagePrinted = true;
    }

    // Log mailbox output to a log file
    if (nodeIndex < getNodes().size()) {
        BCNodeInfo *nodeInfo = getNodes()[nodeIndex];
        if (nodeInfo->_mailboxOutput) {
            fprintf(nodeInfo->_mailboxOutput, "%s", logprefix.str().c_str());
            fprintf(nodeInfo->_mailboxOutput, "%s", buf.get());
            fflush(nodeInfo->_mailboxOutput);
            consoleMessagePrinted = true;
        }
    }

    if (!consoleMessagePrinted) {
        buf[chars_written-1] = '\0';
        LOG_TRACE_MSG(buf);
        buf[chars_written-1] = '\n';
    }

    // Log mailbox output to a socket
    if (_redirectSock != 0) {
        CxxSockets::Message msg;
        msg << buf.get();
        try {
            _redirectSock->Send(msg);
        } catch (const CxxSockets::Error& e) {
            LOG_WARN_MSG(__FUNCTION__ << " -- I/O error: " << strerror(errno) << " -- terminating redirection.");
            _redirectSock.reset();
        }
    }
}

void
BlockControllerBase::printRASMessage(
        const RasEvent& rasEvent
        ) const
{
    // There are various destinations for RAS messages:
    // 1) an optional file specified on the rasfile parameter of connect
    // 2) an optional file specified on the outfile parameter of connect.
    //    RAS messages are logged here if the rasfile parameter is not specified
    // 3) The RAS message is always logged in the MMCS log at a debug severity

    // The format of the data written to the RAS log file is the serialized XML format of the RAS event
    if (_rasLogFile) {
        ostringstream ostring;
        ostring << rasEvent;
        fprintf(_rasLogFile, "%s", ostring.str().c_str());
        fflush(_rasLogFile);
    } else if (_mailboxOutput) {
        ostringstream ostring;
        ostring << rasEvent;
        fprintf(_mailboxOutput, "%s", ostring.str().c_str());
        fflush(_mailboxOutput);
    }

    LOG_DEBUG_MSG( rasEvent );
}

void
BlockControllerBase::startMailbox(
        mmcs_client::CommandReply& reply
        )
{
    LOG_DEBUG_MSG("Starting mailbox monitoring.");
    _mboxMonitor.start(reply);
}

void
BlockControllerBase::stopMailbox()
{
    LOG_DEBUG_MSG("Stopping mailbox monitoring.");
    _mboxMonitor.stop();
}

void
BlockControllerBase::startRedirection(
        CxxSockets::SecureTCPSocketPtr sock,
        mmcs_client::CommandReply& reply
        )
{
    LOG_DEBUG_MSG("Starting mailbox redirection.");
    _redirectSock = sock;
}

void
BlockControllerBase::resetRedirection()
{
    if (!_redirectSock) {
        return;
    }

    LOG_DEBUG_MSG("Resetting mailbox redirection.");
    _redirectSock->Shutdown(CxxSockets::Socket::RECEIVE);
    _redirectSock.reset();
}

void
BlockControllerBase::stopRedirection(
        mmcs_client::CommandReply& reply
        )
{
    if (!_redirectSock) {
        return;
    }

    LOG_DEBUG_MSG("Stopping mailbox redirection.");
    if ( this->isIoBlock() && _targetsetMode == MCServerMessageSpec::RAAW ) {
        // disconnect if we opened the target set for redirection
        const std::deque<std::string> args( 1, "no_shutdown" );
        disconnect(args, reply);
    }
    _redirectSock.reset();
    reply << mmcs_client::OK << mmcs_client::DONE;
}

std::vector<string>
BlockControllerBase::Tokenize(
        const std::string& input,
        const std::string& delimiters
        )
{
    std::vector<string> words;

    std::string::size_type lastPos(input.find_first_not_of(delimiters, 0));
    std::string::size_type pos(input.find_first_of(delimiters, lastPos));
    while (std::string::npos != pos || std::string::npos != lastPos) {
        words.push_back(input.substr(lastPos, pos - lastPos));
        lastPos = input.find_first_not_of(delimiters, pos);
        pos = input.find_first_of(delimiters, lastPos);
    }
    return words;
}

unsigned
BlockControllerBase::getBootCookie()
{
    struct timeval now;
    unsigned cookie;

    gettimeofday(&now, NULL); // get time in secs, usecs
    // high order byte = # secs since last minute
    // low order 3 bytes = # microseconds since last second
    cookie = ((now.tv_sec % 60) << 24) | (now.tv_usec & 0x00FFFFFF);

    // save it in the object
    _bootCookie = cookie;
    return cookie;
}

bool
BlockControllerBase::parseSteps(
        mmcs_client::CommandReply& reply,
        MCServerMessageSpec::BootBlockRequest& req,
        std::string& steps
        )
{
    if ( steps.empty() ) {
        return true;
    }

    _check_complete = false;
    // Rip out the 'steps={' text
    steps.erase(steps.begin(), steps.begin() + steps.find("=") + 1);
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    const boost::char_separator<char> sep(",");
    const tokenizer tok(steps, sep);

    BOOST_FOREACH(const std::string& curr_step, tok) {
        if(curr_step == "enableNodes") {
            req._bootSteps.push_back(BootStep::enableNodes);
        } else if(curr_step == "initDevbus") {
            req._bootSteps.push_back(BootStep::initDevbus);
        } else if(curr_step == "startTraining") {
            req._bootSteps.push_back(BootStep::startTraining);
        } else if(curr_step == "enableLinkChips") {
            req._bootSteps.push_back(BootStep::enableLinkChips);
        } else if(curr_step == "trainLinkChips") {
            req._bootSteps.push_back(BootStep::trainLinkChips);
        } else if(curr_step == "trainTorus") {
            req._bootSteps.push_back(BootStep::trainTorus);
        } else if(curr_step == "installFirmware") {
            req._bootSteps.push_back(BootStep::installFirmware);
        } else if(curr_step == "installPersonalities") {
            req._bootSteps.push_back(BootStep::installPersonalities);
        } else if(curr_step == "startCores") {
            req._bootSteps.push_back(BootStep::startCores);
        } else if(curr_step == "verifyMailbox") {
            req._bootSteps.push_back(BootStep::verifyMailbox);
        } else if(curr_step == "monitorMailbox") {
            req._bootSteps.push_back(BootStep::monitorMailbox);
        } else if(curr_step == "verifyMailboxReady") {
            req._bootSteps.push_back(BootStep::verifyMailboxReady);
        } else if(curr_step == "installKernelImages") {
            req._bootSteps.push_back(BootStep::installKernelImages);
        } else if(curr_step == "configureDomains") {
            req._bootSteps.push_back(BootStep::configureDomains);
        } else if(curr_step == "launchKernels") {
            req._bootSteps.push_back(BootStep::launchKernels);
        } else if(curr_step == "verifyKernelReady") {
            req._bootSteps.push_back(BootStep::verifyKernelReady);
            _check_complete = true;
        } else if(curr_step == "noop") {
            req._bootSteps.push_back(BootStep::noop);
        } else if(curr_step == "cardNoop") {
            req._bootSteps.push_back(BootStep::cardNoop);
        } else {
            reply << mmcs_client::FAIL << "Invalid step " << curr_step << " specified." << mmcs_client::DONE;
            return false;
        }
    }

    return true;
}

bool
BlockControllerBase::parseDomains(
        MCServerMessageSpec::BootBlockRequest& req,
        const std::string& domains
        )
{
    int open, close; open = close = 0;
    for (unsigned int cnt = 0; cnt < domains.length(); ++cnt) {
        if (domains[cnt] == '{') {
            ++open;
        } else if(domains[cnt] == '}') {
            ++close;
        }
    }

    if (open != close) {
        LOG_ERROR_MSG("Mismatched braces in " << domains);
        return false;
    }

    // Initialize the domain vector in the boot block request
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    const boost::char_separator<char> sep("}");

    const tokenizer tok(domains, sep);
    BOOST_FOREACH(std::string curr_dom, tok) {
        // Create the domain object
        MCServerMessageSpec::BootBlockRequest::Domain msgdomain;

        // Default value
        msgdomain._configAddress = 0;

        // Rip out the 'domain={' text
        curr_dom.erase(curr_dom.begin(), curr_dom.begin() + curr_dom.find("{") + 1);

        // Break down the fields
        const boost::char_separator<char> osep(" ");
        const boost::char_separator<char> dollar_sep("$");
        const tokenizer domint(curr_dom, osep);
        BOOST_FOREACH(std::string field, domint) {
            if (field.find("cores=") != std::string::npos) {
                // pull out 'cores='
                field.erase(field.begin(), field.begin() + 6);
                const tokenizer cores(field, dollar_sep);
                tokenizer::const_iterator core_tok = cores.begin();
                try {
                    msgdomain._startCore = boost::lexical_cast<unsigned>(*core_tok);
                } catch (const boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("Could not read start core value " << *core_tok);
                }
                ++core_tok;

                int ec = 0;
                try {
                    ec = boost::lexical_cast<int>(*core_tok);
                } catch(const boost::bad_lexical_cast& e) {
                    LOG_ERROR_MSG("Could not read end core value " << *core_tok);
                }

                if (ec != -1) {
                    msgdomain._endCore = (unsigned)(ec);
                } else {
                    msgdomain._endCore = 16;
                }
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
            } else if (field.find("memory=") != std::string::npos) {
                // pull out 'memory='
                field.erase(field.begin(), field.begin() + 7);
                const tokenizer memory(field, dollar_sep);
                tokenizer::const_iterator mem_tok = memory.begin();

                std::string res;
                std::string memstr = *mem_tok;
                istringstream sa(memstr);

                // Check if its hex
                if (memstr.substr(0,2) == "0x") {
                    memstr.erase(0,2);
                    if (!(sa >> std::hex >> res) || !(boost::algorithm::all( memstr, boost::algorithm::is_xdigit()))) {
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
                if (memstr.substr(0,2) == "0x") {
                    memstr.erase(0,2);
                    if (!(ea >> std::hex >> res) || !(boost::algorithm::all( memstr, boost::algorithm::is_xdigit()))) {
                        LOG_ERROR_MSG("Could not read memory end value " << *mem_tok);
                    }
                    msgdomain._endAddress = strtoll(memstr.c_str(), 0, 16);
                } else {
                    msgdomain._endAddress = strtoll(memstr.c_str(), 0, 10);
                }
            } else if (field.find("images=") != std::string::npos) {
                // pull out 'images='
                field.erase(field.begin(), field.begin() + 7);
                msgdomain._imageSet = field;
            } else if (field.find("options=") != std::string::npos) {
                // pull out 'options='
                field.erase(field.begin(), field.begin() + 8);
                msgdomain._options = field;
            } else if (field.find("id=") != std::string::npos) {
                // slurp this option, it is not used in boot request
            } else {
                // assume option
                msgdomain._options += " ";
                msgdomain._options += field;
            }
        }
        if (!msgdomain._imageSet.empty()) {
            req._domains.push_back(msgdomain);
        }
    }
    return true;
}

bool
BlockControllerBase::checkComplete(
        std::vector<std::string>& bad_node_locs,
        std::set<std::string>& good_nodes,
        const MCServerMessageSpec::VerifyKernelReadyRequest& bootreq
        )
{
    PthreadMutexHolder mutex;
    mutex.Lock(&_mutex);

    MCServerMessageSpec::VerifyKernelReadyReply bootrep;
    if (_mcServer && !hardWareAccessBlocked()) {
        _mcServer->verifyKernelReady(bootreq, bootrep);
    } else {
        return false;
    }

    if ( bad_node_locs.size() != bootrep._compute_not_ready.size() ) {
        LOG_INFO_MSG(bootrep._compute_not_ready.size() << " nodes not yet ready.");
    } else {
        LOG_DEBUG_MSG(bootrep._compute_not_ready.size() << " nodes not yet ready.");
    }
    bad_node_locs = bootrep._compute_not_ready;

    // Mark the good ones 'initialized'.  'Good' nodes are nodes that are
    // not in the list of bad ones returned by verifyKernelReady AND are
    // not in the list of nodes that were in error when we started.
    BOOST_FOREACH(const std::string& curr_node, bootreq._location) {
        if (std::find(bad_node_locs.begin(), bad_node_locs.end(), curr_node) == bad_node_locs.end()) {
            // Succeeded.
            if (std::find(_error_nodes.begin(), _error_nodes.end(), curr_node) == _error_nodes.end()) {
                // Also, don't put it in the good list if it was marked in error.
                BCNodeInfo* node = dynamic_cast<BCNodeInfo*>(_targetLocationMap[curr_node]);
                node->_initialized = true;
                good_nodes.insert(curr_node);
            }
        }
    }

    // Remove any of the errored nodes from the bad list.
    BOOST_FOREACH(const std::string& enode, _error_nodes) {
        const std::vector<std::string>::const_iterator nodeit(
                std::find(
                    bootrep._compute_not_ready.begin(),
                    bootrep._compute_not_ready.end(),
                    enode
                    )
                );
        if (nodeit != bootrep._compute_not_ready.end()) {
            // This error node is in the list of compute_not_ready nodes, remove it.
            bootrep._compute_not_ready.erase(
                    std::remove(
                        bootrep._compute_not_ready.begin(),
                        bootrep._compute_not_ready.end(),
                        enode
                        ),
                    bootrep._compute_not_ready.end()
                    );
        }
    }

    if (bootrep._compute_not_ready.empty()) {
        return true;  // None waiting.  We're done!
    }

    return false;
}

bool
BlockControllerBase::openLog(
        BCNodeInfo* node
        )
{
    if ( !node ) {
        return false;
    }

    std::string logpath = Properties::getProperty(MMCS_LOGDIR);
    if ( logpath.empty() ) {
        node->_mailboxOutput = NULL;
        return true;
    }

    // Close previous log if left open
    if ( node->_mailboxOutput ) {
        if ( fclose(node->_mailboxOutput) ) {
            const int error = errno;
            LOG_WARN_MSG(
                    "Could not close descriptor " << fileno( node->_mailboxOutput) << " for " <<
                    node->location() << ": " << strerror(error)
                    );

            node->_mailboxOutput = NULL;
        } else {
            // LOG_TRACE_MSG( "Closed log file for " << node->location() );
        }
    }

    logpath.append( "/" );
    logpath.append( node->location() );
    logpath.append( ".log" );

    node->_mailboxOutput = fopen( logpath.c_str(), "a");
    if ( !node->_mailboxOutput ) {
        const int error = errno;
        LOG_WARN_MSG( "Could not open log file: " << logpath << " errno: " << error << " (" << strerror(error) << ")" );
        return false;
    }

    LOG_DEBUG_MSG("{" << node->_locateId << "} I/O log file: " << logpath);
    return true;
}

bool
BlockControllerBase::hardWareAccessBlocked()
{
    // If we're blocking anything, check to see if it's anything we need.
    if (HardwareBlockList::list_size() != 0) {
        // See if any of my hardware is in the list.
        std::string rackstring;
        BOOST_FOREACH(const BCIconInfo* icon, _icons) {
            rackstring = icon->location().substr(0,3);
            if (HardwareBlockList::find_in_list(rackstring) == true) {
                return true;
            }
        }
    }
    return false;
}

} } // namespace mmcs::server
