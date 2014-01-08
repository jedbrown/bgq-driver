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

#ifndef _BGAGENT_REP_H
#define _BGAGENT_REP_H

#include <log4cxx/mdc.h>
//#include <utility/include/DropoffQueue.h>
#include "BGAgentBase.h"
#include "BinaryController.h"
#include "BGMasterProtocol.h"

class BinaryController;
class BGAgentBase;
class Alias;
class AliasList;

typedef std::pair<boost::shared_ptr<BinaryController>, boost::shared_ptr<BGAgentBase> > BinaryLocation;

//! \brief Representation of a bgagent internal to bgmaster_server
//! Also available to the client api.
class BGAgentRep : public BGAgentBase, public boost::enable_shared_from_this<BGAgentRep>
{
    //! No data members.  All inherited from the base.
    //! This allows for safe casting.

    void doCompleteRequest(BGMasterAgentProtocolSpec::CompleteRequest& compreq);
    void doFailedRequest(BGMasterAgentProtocolSpec::FailedRequest& failreq);

    //! These functions are non-locking.  Functions that call them must lock the agent object!
    void executePolicyAndClear_nl(BinaryControllerPtr binptr, boost::shared_ptr<BGAgentRep> rep_p, int signo = 0);

    //! \brief Just execute the policy, don't clear.
    void executePolicy_nl(BinaryId& reqbid, boost::shared_ptr<BGAgentRep> rep_p, 
                          boost::shared_ptr<Alias> al, int signo = 0);
    BinaryId startBin_nl(BGMasterAgentProtocolSpec::StartRequest& startreq,
                  BGMasterAgentProtocolSpec::StartReply& startrep);
    void stopBin_nl(const BinaryId& bid,
                 const BinaryLocation& binloc,
                 const int signal,
                 BGMasterAgentProtocolSpec::StopReply& stoprep,
                 bool failover);

#if 0
    //! \brief Call this when the agent dies and we need to clean up our records of
    //!        running binaries.  Locking.
    void emptyBinaries();
#endif

    void agentAbend(std::ostringstream& msg);
public:
    BGAgentRep() {}
    BGAgentRep(BGMasterAgentProtocolPtr& p,
               BGMasterAgentProtocolSpec::JoinRequest& joinreq,
               BGMasterAgentProtocolSpec::JoinReply& joinrep);

    ~BGAgentRep();

    //! process incoming =agent= requests
    //! \returns true if a soft error
    bool processRequest();

    void waitMessages();

    void startPoller();

    //! \brief Stop a binary and execute a policy for it.  If there is no policy,
    //!        this method will fail with an exception.
    void stopBinaryAndExecutePolicy(BinaryId& bid,
                                    const BinaryLocation& binloc,
                                    const int signal,
                                    BGMasterAgentProtocolSpec::StopReply& stoprep,
                                    BinaryControllerPtr binptr,
                                    std::string& trigger);

    BinaryId startBin(BGMasterAgentProtocolSpec::StartRequest& startreq,
                      BGMasterAgentProtocolSpec::StartReply& startrep);

    void stopBin(const BinaryId& bid,
                 const BinaryLocation& binloc,
                 const int signal,
                 BGMasterAgentProtocolSpec::StopReply& stoprep,
                 bool failover);

    //! \brief Stop all binaries running on this agent.  Locking.
    void stopAllBins(BGMasterClientProtocolSpec::StopReply& stoprep, int signal);

    //! \brief Cancel polling thread processing and optionally end the agent process.
    //! \param agent Also end the agent process.
    //! \param binaries Also end the managed binaries.
    //! \param signal Signal to use to end the binaries.
    void cancel(const bool agent, const bool binaries, const unsigned signal);

    //! \brief request the bgagentd process to end
    bool endAgent(BGMasterAgentProtocolSpec::End_agentReply& agentdierep);

    int binCount() { return _binaries.size(); }
    //    DropoffQueue<BGMasterAgentProtocolSpec::StartRequest> _agent_request_queue;
};

#endif
