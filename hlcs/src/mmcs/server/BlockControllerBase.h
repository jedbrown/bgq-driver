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

#ifndef MMCS_SERVER_BLOCK_CONTROLLER_BASE_H_
#define MMCS_SERVER_BLOCK_CONTROLLER_BASE_H_

#include "PerformanceCounters.h"
#include "MailboxMonitor.h"

#include "types.h"

#include "libmmcs_client/CommandReply.h"

#include <bgq_util/include/pthreadmutex.h>

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <control/include/bgqconfig/xml/BGQMachineXML.h>

#include <control/include/mcServer/MCServerRef.h>

#include <ras/include/RasEvent.h>

#include <boost/enable_shared_from_this.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include <deque>
#include <iosfwd>
#include <map>
#include <set>
#include <string>
#include <vector>

#include <stdio.h>
#include <time.h>

namespace mmcs {

namespace lite {
    
class BlockController;
class MMCSCommand_delete_block;

} // namespace lite

namespace server {

class BootFailure;
class CheckBootComplete;

typedef std::map<std::string,std::string> NodeCustomization;

class BlockControllerBase : private boost::noncopyable, public boost::enable_shared_from_this<BlockControllerBase>
{
    friend class BlockHelper;
    friend class BootFailure;
    friend class CheckBootComplete;
    friend class DBBlockController;
    friend class DefaultListener;
    friend class MailboxMonitor;
    friend class MailboxListener;
    friend class lite::BlockController;
    friend class lite::MMCSCommand_delete_block;

public:
    /*!
     * \brief ctor.
     */
    BlockControllerBase(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& userName,    //!< [in] user name
            const std::string& blockName,   //!< [in] block name
            bool delete_machine
            );

    virtual             ~BlockControllerBase();
    virtual void        create_block(std::deque<std::string> args, mmcs_client::CommandReply& reply, std::istream* blockStreamXML=NULL) {}
    virtual void        boot_block(std::deque<std::string> args, mmcs_client::CommandReply& reply, const NodeCustomization& ncfg = NodeCustomization(), const PerformanceCounters::Timer::Ptr = PerformanceCounters::Timer::Ptr() ) { }
    virtual void        pgood_reset(mmcs_client::CommandReply& reply, const BlockControllerTarget* pTarget);
    virtual void        shutdown_block(mmcs_client::CommandReply& reply, const std::deque<std::string>& args) { }
    virtual void        show_barrier(mmcs_client::CommandReply& reply) {}
    void                mcserver_connect(mmcs_client::CommandReply& reply) { this->mcserver_connect(_mcServer, _userName, reply); }
    static  void        mcserver_connect(MCServerRef*& mcServer, std::string userName, mmcs_client::CommandReply& reply);
    virtual void        connect(std::deque<std::string> args, mmcs_client::CommandReply& reply, const BlockControllerTarget* pTarget);
    virtual void        disconnect(const std::deque<std::string>& args, mmcs_client::CommandReply& reply);
    virtual bool        processRASMessage(RasEvent& rasEvent) { return false; }
    void                processConsoleMessage(const MCServerMessageSpec::ConsoleMessage& consoleMessage);

    bool                isConnected() const    { return _mcServer; }
    bool                isStarted()            { return _isstarted; };
    virtual void        setReconnected()       { return; }
    bool                isIoBlock() const;
    CxxSockets::SecureTCPSocketPtr    getRedirectSock()  { return _redirectSock; }
    bool                isDisconnecting();
    void                setDisconnecting(bool disconnecting, const std::string& disconnectReason);
    bool                peekDisconnecting() const { return _disconnecting; }     // NOT mutex protected, for display purposes only
    int                 incrBlockThreads();    // increment the number of threads using this block
    int                 decrBlockThreads();    // decrement the number of threads using this block
    int                 getBlockThreads();
    bool                readyToDie();
    int                 peekBlockThreads() const { return _blockThreads; }     // NOT mutex protected, for display purposes only
    const std::string&  disconnectReason() const { return _disconnectReason; }
    BGQBlockNodeConfig* getBlockNodeConfig()   { return _block; }
    BGQMachineXML*      getMachineXML()        { return _machineXML; }
    BGQBlockXML*        getBlockXML()          { return _blockXML; }
    unsigned            numNodesStarted()      { return _numNodesStarted; }
    const std::string&  getUserName() const    { return _userName; }
    const std::string&  getBlockName() const   { return _blockName; }
    MCServerRef*        getMCServer()          { return _mcServer; }
    int                 terminatedNodes()      { return _terminatedNodes; }
    static std::vector<std::string>  Tokenize(const std::string& string, const std::string& delimiters);
    static bool         openLog(BCNodeInfo* node);

    // BlockControllerTarget methods
    std::vector<BCTargetInfo*>&      getTargets()   { return _targets; }
    std::vector<BCNodeInfo*>&        getNodes()     { return _nodes; }
    std::vector<BCLinkchipInfo*>&    getLinkchips() { return _linkchips; }
    std::vector<BCIconInfo*>&        getIcons()     { return _icons; }
    BCNodeInfo*            findNodeInfo(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e);
    BCNodeInfo*            findNodeInfo(const std::string& location) const;
    BCNodecardInfo*        findNodeBoardInfo(const std::string& location);
    const std::set<std::string>& linkless_mps() const { return _io_linkless_midplanes; }

    // Mailbox methods
    void                startMailbox(mmcs_client::CommandReply& reply);
    void                stopMailbox();
    bool                isMailboxStarted() const { return _mboxMonitor.isStarted(); }

    // redirection
    void                startRedirection(CxxSockets::SecureTCPSocketPtr sock, mmcs_client::CommandReply& reply);
    void                stopRedirection(mmcs_client::CommandReply& reply);
    void                resetRedirection();
    const boost::posix_time::ptime& getBootStartTime() const { return _boot_block_start; }
    bool                getShared() const { return _isshared; }
    void                setShared(bool tf)     { _isshared = tf; }
    bool                hardWareAccessBlocked();
    void                setBootCookie(int c)   { _bootCookie = c; }

protected:
    void                printConsoleMessage(unsigned nodeIndex, unsigned cpuNum, unsigned threadId, const std::string& message);
    void                printRASMessage(const RasEvent& rasEvent) const;
    void                delete_block();
    void                initMachineConfig(mmcs_client::CommandReply& reply);
    void                deleteTargets();
    int                 makeAndOpenTargetSet(MCServerRef* const ref, bool temp_targetset, mmcs_client::CommandReply& reply, const BlockControllerTarget* pTarget);

    unsigned            getBootCookie();           // generate a unique boot id
    bool                parseSteps(mmcs_client::CommandReply& rep, MCServerMessageSpec::BootBlockRequest& req, std::string& steps);
    bool                parseDomains(MCServerMessageSpec::BootBlockRequest& req, const std::string& domstring);
    bool checkComplete(
            std::vector<std::string>& bad_node_locs,
            std::set<std::string>& good_nodes,
            const MCServerMessageSpec::VerifyKernelReadyRequest& bootreq
            );

protected:
    BlockHelper*                   _helper;
    BGQMachineXML*                 _machineXML;                  // information about the entire machine (xml format)
    BGQBlockXML*                   _blockXML;                    // information about the block (xml format)
    BGQBlockNodeConfig*            _block;                       // current block being controlled
    std::vector<BCTargetInfo*>     _targets;                     // all targets (nodes, node cards, link chips, etc)
    std::vector<BCNodeInfo*>       _nodes;                       // node target connection and status information
    std::vector<BCLinkchipInfo*>   _linkchips;                   // link chip connection and status information
    std::vector<BCIconInfo*>       _icons;                       // icon chip connection and status information
    std::map<std::string,BCTargetInfo*>  _targetLocationMap;     // map location to target
    MailboxMonitor                 _mboxMonitor;
    MCServerRef*                   _mcServer;                    // connection to mcServer
    unsigned                       _numNodesStarted;             // number of compute nodes that have received a "start" command
    unsigned                       _numNodesTotal;               // number of nodes in the block
    unsigned                       _numComputesTotal;            // number of compute nodes in the block
    unsigned                       _bootCookie;                  // for relating mc RAS events with BootBlockRequest
    MCServerMessageSpec::LockMode  _targetsetMode;               // mcServer target set open mode: CONTROL, MONITOR, DEBUG, or SERVICE
    int                            _targetsetHandle;             // mcServer handle returned on OpenTargetSetReply
    bool                           _diags;                       // This is diags, RAS is handled differently
    bool                           _insertDiagsRAS;              // Inserts diags RAS into database
    PthreadMutex                   _mailboxLockBox;
    FILE*                          _mailboxOutput;               // mailbox output file
    FILE*                          _rasLogFile;                  // optional output file for ras data
    CxxSockets::SecureTCPSocketPtr _redirectSock;                // file descriptor for output redirection
    PthreadMutex                   _mutex;                       // block controller mutex
    volatile int                   _blockThreads;                // number of threads that are using this BlockControllerBase
    volatile bool                  _disconnecting;               // terminate gracefully
    const std::string              _userName;                    // current block user
    std::string                    _blockName;                   // block name
    bool                           _isstarted;                   // start command issued
    bool                           _isshared;                    // node cards may be shared between blocks
    std::string                    _disconnectReason;            // reason for asynchronous disconnect
    boost::posix_time::ptime       _boot_block_start;            // This is when we requested the block was booted and lasts until it's done
    std::set<std::string >         _io_linkless_midplanes;
    const bool                     _delmachine;
    int                            _terminatedNodes;
    bool                           _do_barrier_ack;
    bool                           _block_shut_down;
    NodeCustomization              _nodeCustomization;
    bool                           _check_complete;              // True if verifyKernelReady boot step specified.
    bool                           _rebooting;
    std::string                    _domains;
    std::string                    _machine_config_data;         // Machine-wide node customization data.
    std::vector<std::string>       _error_nodes;
    bool                           _tolerating_faults;
};

} } // namespace mmcs::server

#endif
