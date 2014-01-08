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

#ifndef BLOCKCONTROLLERTARGETINFO_HEADER
#define BLOCKCONTROLLERTARGETINFO_HEADER

#include <control/include/bgqconfig/BGQBlockNodeConfig.h>

#include <boost/utility.hpp>

// common information for locating and targeting Nodes, Idos, and Linkcards

class BCTargetInfo : private boost::noncopyable
{
public:
    BGQBlockNodeConfig* _block;	// BGQBlockNodeConfig that this configuration information relates to
    unsigned _locateId;		// target specifier of this node, returned by locate
                                // also index into the BlockController::_targets vector
    unsigned _boardcoordA;	// board A coordinate
    unsigned _boardcoordB;	// board B coordinate
    unsigned _boardcoordC;	// board C coordinate
    unsigned _boardcoordD;	// board D coordinate

    string   _location;		// unique identifier
    bool     _open;		// this target is open
    bool     _linkio;           // added as connected IO link not to be in the tset.
    bool     _passthru_only;
    BGQBlockNodeConfig*  blockNodeConfig() { return _block; }
    BGQMidplaneNodeConfig* midplaneNodeConfig() { return _block ? _block->getMidplaneNodeConfigAllMidplanes(_boardcoordA,_boardcoordB,_boardcoordC, _boardcoordD) : NULL; }
    BGQIOBoardNodeConfig* ioboardNodeConfig() { return _block ? _block->ioboardNodeConfig(_boardcoordA, _boardcoordB, _boardcoordC, _boardcoordD) : 0; }
    virtual std::string midplanePos() { return _location.substr(0,6); }
    BCTargetInfo(const string& loc) : _block(NULL), _locateId(0), _boardcoordA(0), _boardcoordB(0), _boardcoordC(0), _boardcoordD(0), _location(loc), _open(false), _linkio(false), _passthru_only(false) {}
    BCTargetInfo() : _block(NULL), _locateId(0), _boardcoordA(0), _boardcoordB(0), _boardcoordC(0), _boardcoordD(0), _open(false), _linkio(false), _passthru_only(false) {}
    virtual ~BCTargetInfo() {}
    virtual void init_location() {};
    string& location() { return _location; }
};

#endif
