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

#ifndef MMCS_SERVER_BC_NODE_INFO_H_
#define MMCS_SERVER_BC_NODE_INFO_H_

#include "BCTargetInfo.h"

#include <control/include/bgqconfig/BGQMidplaneNodeConfig.h>
#include <control/include/bgqconfig/BGQNodePos.h>
#include <control/include/bgqconfig/BGQPersonality.h>

#include <cstdio>

namespace mmcs {
namespace server {

// Information needed by BlockController to boot and communicate with a BGQ node via jtag.
//

//
// Node states
//
enum {
   NST_IN_RESET,               // node is idle in reset
   NST_PROGRAM_RUNNING,        // program is running,
   NST_TERMINATED,             // all cores terminated
};

class BCNodeInfo : public BCTargetInfo
{
public:
    // node position
    //
    BGQNodePos      _pos;                // position of this node within the midplane
    BGQIONodePos    _iopos;
    // node status
    //
    unsigned        _state;              // state of this node.
    bool            _initialized;        // is the kernel ready for job submission?
    bool            _haltComplete;       // did we receive 'Shutdown complete'?
    FILE            *_mailboxOutput;     // mailbox output file for I/O node logs
    Personality_t   _iopersonality;      // Used only for a link IO node in a CN block

public:
    // Initialize a chip and connect to it.
    //
    BCNodeInfo()
        : _pos(0,0),
        _state(NST_IN_RESET),
        _initialized(false),
        _haltComplete(false),
        _mailboxOutput(NULL)
    {
        _linkio = false;
    }

    BCNodeInfo(const std::string& loc)
        :  BCTargetInfo(loc),
        _pos(loc.substr(7,3).c_str(),loc.substr(11,3).c_str()),
        _state(NST_IN_RESET),
        _initialized(false),
        _haltComplete(false),
        _mailboxOutput(NULL)
    {
        _linkio = false;
    }

    ~BCNodeInfo()
    {
        if (_mailboxOutput)
            fclose(_mailboxOutput);
    }

    bool isIOnode() const {
        if (_linkio) {
            return true;
        }
        if (ioboardNodeConfig() != 0) {
            return true;
        } else {
            return false;
        }
    }

    Personality_t& personality() {
        if (_linkio) { // We're a link training IO node
            return _iopersonality;
        } else if (midplaneNodeConfig() != NULL) {
            return midplaneNodeConfig()->nodeConfig(_pos)->_personality;
        } else if (ioboardNodeConfig()) {  // Might be an io board
            return ioboardNodeConfig()->nodeConfig(_iopos)->_personality;
        } else
            return default_personality;

        return (midplaneNodeConfig() != NULL) ? midplaneNodeConfig()->nodeConfig(_pos)->_personality : default_personality;
    }

    const char* nodeCardPos() { return BGQTopology::nodeCardNameFromPos(_pos.nodeCard()); }
    std::string nodeCardLocation() { return midplaneNodeConfig()->nodeCardLocation(_pos.nodeCard()); }
    const char* processorCardPos() { return BGQTopology::processorCardNameFromJtagPort(_pos.jtagPort()); }
    const char* processorPos() { return "0"; }
    void init_location()
    {
        if (midplaneNodeConfig()) {
            std::ostringstream ostr;
            ostr << midplaneNodeConfig()->posInMachine() << "-"  << nodeCardPos() << "-"  << processorCardPos();
            _location = ostr.str();
        } else if (ioboardNodeConfig()) {  // Might be an io board
            std::ostringstream ostr;
            ostr << ioboardNodeConfig()->posInMachine() << "-" << _iopos;
            _location = ostr.str();
        }
    }

    int jtag() {
        if (_pos.nodeCard() == 0)
            return _iopos.jtagPort();
        return _pos.jtagPort();
    }

    void reset_state()
    {
        _state = NST_IN_RESET;
        _initialized = false;
        _haltComplete = false;
    }

private:
    static Personality_t default_personality;
};

} } // namespace mmcs::server

#endif
