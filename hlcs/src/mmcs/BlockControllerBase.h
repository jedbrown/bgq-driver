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
 * \file BlockController.h
 */

#ifndef BLOCK_CONTROLLER_BASE_H
#define BLOCK_CONTROLLER_BASE_H

#include <deque>
#include <vector>
#include <list>
#include <tr1/memory>
#include <utility/include/cxxsockets/SocketTypes.h>
#include <xml/include/c_api/MCServerMessageSpec.h>
#include "MMCSProperties.h"
#include "BlockControllerNodeInfo.h"
#include "PerformanceCounters.h"
#include "BlockControllerIconInfo.h"
#include "BlockControllerBllInfo.h"
#include "BlockControllerTarget.h"
#include "MailboxMonitor.h"
#include "SecureMailboxMonitor.h"
#include "RackBitset.h"

#define LOGGING_DECLARE_BLOCK_ID_MDC \
    log4cxx::MDC _blockid_mdc_( "blockId", std::string("{") + _blockName + "} " );
#define LOGGING_DECLARE_BLOCK_USER_MDC \
    log4cxx::MDC _block_user_mdc_( "user", std::string("[") + _userName + "] " );

// forward declarations
class BGQMachineXML;
class MCServerRef;
class MMCSCommandReply;
class RasEvent;
class BlockHelper;
class CheckBootComplete;

namespace MCServerMessageSpec
{
    class ConsoleMessage;
}

using namespace std;

static const int BGQ_MAX_CORE = 16;

typedef boost::shared_ptr<BlockHelper> BlockHelperPtr;
typedef std::map<std::string,std::string> NodeCustomization;

class BlockControllerBase : private boost::noncopyable, public boost::enable_shared_from_this<BlockControllerBase>
{
    friend class MailboxMonitor;
    friend class SecureMailboxMonitor;
    friend class MailboxListener;
    friend class SecureMailboxListener;
    friend class BlockHelper;
    friend class DBBlockController;
    friend class LiteBlockController;
    friend class DefaultControlEventListener;
    friend class CheckBootComplete;
    friend class CNBlockController;
public:
    // types

    BlockHelper* _helper;
public:
    /*!
     * \brief ctor.
     */
    BlockControllerBase(
            BGQMachineXML* machine,         //!< [in] machine description
            const std::string& userName,    //!< [in] user name
            const std::string& blockName,    //!< [in] block name
            bool delete_machine
            );

    virtual				~BlockControllerBase();
    void                setHelper(BlockHelper* h) { /*BlockHelper* k(h); _helper = k; */ _helper = h; }
    BlockHelper*        getHelper() { return _helper; }
    virtual void	    create_block(deque<string> args, MMCSCommandReply& reply, std::istream* blockStreamXML=NULL) {}
    virtual void        create_nodeset(vector<BCNodeInfo*> args, MMCSCommandReply& reply);
    virtual void        delete_block(deque<string> args, MMCSCommandReply& reply);
    virtual void	    boot_block(deque<string> args, MMCSCommandReply& reply, const NodeCustomization& ncfg = NodeCustomization(), const PerformanceCounters::Timer::Ptr = PerformanceCounters::Timer::Ptr() ) { }
    virtual void		shutdown_nodes(MMCSCommandReply& reply) {}
    virtual void		reboot_nodes(BlockControllerTarget* pTarget, deque<string> args, MMCSCommandReply& reply) {}
    virtual void        pgood_reset(MMCSCommandReply& reply, BlockControllerTarget* pTarget);
    virtual void		shutdown_block(MMCSCommandReply& reply, BlockControllerTarget* target, bool no_sysrq) {}
    virtual void        show_barrier(MMCSCommandReply& reply) {}
    virtual void		mcserver_connect(MMCSCommandReply& reply) { BlockControllerBase::mcserver_connect(_mcServer, _userName, reply); }
    static  void		mcserver_connect(MCServerRef*& mcServer, string userName, MMCSCommandReply& reply);
    virtual void		connect(deque<string> args, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets = false);
    virtual void	    disconnect(deque<string>& args);
    virtual bool        processRASMessage(RasEvent& rasEvent) { return false; }
    virtual void		processConsoleMessage(MCServerMessageSpec::ConsoleMessage& consoleMessage);

    bool				isCreated()		       { return (_targets.size() > 0 || _nodeset == true); }
    bool 				isConnected();
    bool 				isStarted() 		   { return _isstarted; };
    virtual void        setReconnected()       { return; }
    bool                isIOBlock()            { return _isIOblock; }
    CxxSockets::SecureTCPSocketPtr	getRedirectSock()  { return _redirectSock; }
    FILE* 				getRasLogFile()		   { return _rasLogFile; }
    void  				setRasLogFile(FILE* rasLog) 	{ _rasLogFile = rasLog; }
    PthreadMutex*       getMailBoxLockBox()    { return &_mailboxLockBox; }
    bool 				isDisconnecting();
    void 				setDisconnecting(bool disconnecting, string disconnectReason);
    bool 				peekDisconnecting()    { return _disconnecting; } 	// NOT mutex protected, for display purposes only
    int  				incrBlockThreads();	   // increment the number of threads using this block
    int  				decrBlockThreads();	   // decrement the number of threads using this block
    int  				getBlockThreads();
    bool                readyToDie();
    int  				peekBlockThreads() 	   { return _blockThreads; } 	// NOT mutex protected, for display purposes only
    bool                doCheckCompete()       { return _check_complete; }
    const string&		disconnectReason() 	   { return _disconnectReason; }
    BGQBlockNodeConfig*	getBlockNodeConfig()   { return _block; }
    BGQMachineXML*		getMachineXML() 	   { return _machineXML; } 	// only set for mmcs_hw
    BGQBlockXML*		getBlockXML() 		   { return _blockXML; }       	// only set for mmcs_hw
    unsigned			numNodesActive() 	   { return _numNodesActive; }
    unsigned			numNodesStarted() 	   { return _numNodesStarted; }
    void				decrNumNodesActive()   { if (_numNodesActive) _numNodesActive--;  }
    const string&		getUserName() 		   { return _userName; }
    const string&		getBlockName() 		   { return _blockName; }
    MCServerRef*	    getMCServer()		   { return _mcServer; }
    int                 terminatedNodes()      { return _terminatedNodes; }
    unsigned            numNodesTotal()        { return _numNodesTotal;}
    static std::vector<string>  Tokenize(const std::string &rStr, const std::string &szDelimiters);
    static bool         openLog(BCNodeInfo* node);

    // BlockControllerTarget methods
    vector<BCTargetInfo*>&		getTargets()   { return _targets; }
    vector<BCNodeInfo*>&		getNodes()     { return _nodes; }
    vector<BCLinkchipInfo*>&	getLinkchips() { return _linkchips; }
    vector<BCIconInfo*>&		getIcons()     { return _icons; }
    BCNodeInfo*			findNodeInfo(unsigned a, unsigned b, unsigned c, unsigned d, unsigned e);
    BCNodeInfo*			findNodeInfo(string location);
    BCNodecardInfo*		findNodecardInfo(string location);
    BCLinkchipInfo*		findLinkchipInfo(string location);
    BCTargetInfo*		findTargetInfo(string location) { return _targetLocationMap[location]; }
    int                 linkless_mps()         { return _io_linkless_midplanes; }
    // Mailbox methods
    virtual void		startMailbox(MMCSCommandReply& reply);
    virtual void		quiesceMailbox(BlockControllerTarget* target);
    virtual void		stopMailbox();
    virtual bool		isMailboxStarted()     { if( _mboxMonitor.isStarted() || _smboxMonitor.isStarted()) return true; else return false; }

    // redirection
    void				startRedirection(CxxSockets::SecureTCPSocketPtr sock, MMCSCommandReply& reply);
    void				stopRedirection(MMCSCommandReply& reply, bool shutdown = false);
    void                resetRedirection();
    time_t              getBootStartTime()     { return _boot_block_start; }
    bool                getShared()            { return _isshared; }
    void                setShared(bool tf)     { _isshared = tf; }
    bool                getRebooting()         { return _rebooting; }
    void                setRebooting(bool tf)  { _rebooting = tf; }
    bool                hardWareAccessBlocked();
    void                setRebooted(bool tf)   { _rebooted = tf; }
    void                setBootCookie(int c)   { _bootCookie = c; }
protected:
    virtual void	    printConsoleMessage(unsigned nodeIndex, unsigned cpuNum, unsigned threadId, const string& message);
    virtual void		printRASMessage(RasEvent& rasEvent);
    virtual void		delete_block();
    virtual void		initMachineConfig(MMCSCommandReply& reply);
    void				deleteTargets();
    int                 makeAndOpenTargetSet(MCServerRef* ref, bool temp_targetset, MMCSCommandReply& reply, BlockControllerTarget* pTarget, bool add_targets = false);

    unsigned			getBootCookie();           // generate a unique boot id
    bool                parseSteps(MMCSCommandReply& rep, MCServerMessageSpec::BootBlockRequest& req, std::string& steps);
    bool                parseDomains(MCServerMessageSpec::BootBlockRequest& req, std::string& domstring);
    std::vector<std::string>    checkShutdownComplete(BlockControllerTarget* target);
    bool                checkComplete(MMCSCommandReply& reply,
                                      std::vector<std::string>& bad_node_locs,
                                      std::set<std::string>& good_nodes,
                                      MCServerMessageSpec::VerifyKernelReadyRequest& bootreq,
                                      unsigned nodecount);
    bool                doBarrierAck()         { return _do_barrier_ack; }
    void                doBarrierAck(bool tf)  { _do_barrier_ack = tf; }
protected:
    BGQMachineXML*          	   _machineXML;           	   // information about the entire machine (xml format)
    BGQBlockXML*            	   _blockXML;             	   // information about the block (xml format)
    BGQBlockNodeConfig*     	   _block;                	   // current block being controlled
    vector<BCTargetInfo*>		   _targets;   			       // all targets (nodes, node cards, link chips, etc)
    vector<BCNodeInfo*>			   _nodes;      			   // node target connection and status information
    vector<BCLinkchipInfo*>		   _linkchips;        		   // link chip connection and status information
    vector<BCIconInfo*> 	       _icons;         		       // icon chip connection and status information
    map<string,BCTargetInfo*>	   _targetLocationMap;         // map location to target
    MailboxMonitor			       _mboxMonitor;               // thread to receive mailbox output
    SecureMailboxMonitor           _smboxMonitor;
    MCServerRef*                   _mcServer;		           // connection to mcServer
    static struct sockaddr_in      _local_addr;                // local sockaddr struct for mcserver connection
    static CxxSockets::SockAddr    _local_saddr;
    unsigned                	   _numNodesActive;        	   // number of nodes booted, but not terminated.
    unsigned                	   _numNodesStarted;	       // number of compute nodes that have received a "start" command
    unsigned                	   _numNodesTotal;     	       // number of nodes in the block
    unsigned                	   _numComputesTotal;  	       // number of compute nodes in the block
    unsigned				       _bootCookie;			       // for relating mc RAS events with BootBlockRequest
    unsigned			           _savedBootCookie;		   // for relating performance counters to mc performance counters
    MCServerMessageSpec::LockMode  _targetsetMode;             // mcServer target set open mode: CONTROL, MONITOR, DEBUG, or SERVICE
    int                            _targetsetHandle;           // mcServer handle returned on OpenTargetSetReply
    bool                    	   _diags;                     // This is diags, RAS is handled differently
    bool                           _insertDiagsRAS;            // Inserts diags RAS into database
    PthreadMutex                   _mailboxLockBox;
    FILE*                   	   _mailboxOutput;         	   // mailbox output file
    FILE*                    	   _rasLogFile;            	   // optional output file for ras data
    CxxSockets::SecureTCPSocketPtr _redirectSock;	           // file descriptor for output redirection
    PthreadMutex  			       _mutex;				       // block controller mutex
    volatile int			       _blockThreads;	   		   // number of threads that are using this BlockControllerBase
    volatile bool 			       _disconnecting;  		   // terminate gracefully
    string  				       _userName;		   	       // current block user
    string  				       _blockName;		   	       // block name
    int                            _createId;
    time_t				           _last_ras_event_time;   	   // time of last kernel fatal ras event for this block
    time_t				           _shutdown_sent_time;    	   // time that shutdown was sent to I/O nodes
    bool  				           _isstarted;		           // start command issued
    bool				           _isIOblock;                 // create_block (t) or create_targetset (f) used to create
    bool				           _isshared;			       // node cards may be shared between blocks
    string 				           _disconnectReason;	       // reason for asynchronous disconnect
    time_t                         _boot_block_start;          // This is when we requested the block was booted and lasts until it's done
    int                            _io_linkless_midplanes;
    bool                           _delmachine;
    int                            _terminatedNodes;
    bool                           _do_barrier_ack;
    bool                           _block_shut_down;
    NodeCustomization              _nodeCustomization;
    bool                           _check_complete;            // True if verifyKernelReady boot step specified.
    bool                           _rebooting;
    std::string                    _domains;
    std::string                    _bootOptions;
    bool                           _nodeset;
    BlockControllerTarget*         _residual_target;
    std::string                    _machine_config_data;       // Machine-wide node customization data.
    boost::posix_time::time_duration  _total_barrier_duration;
    boost::mutex                   _init_free_lock;
    bool                           _rebooted;
    std::vector<std::string>       _error_nodes;
    bool                           _tolerating_faults;
};



#endif
