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

#ifndef BLOCKCONTROLLERICONINFO_H
#define BLOCKCONTROLLERICONINFO_H

#include "BlockControllerTargetInfo.h"
#include "BlockControllerBllInfo.h"

#include <string>
#include <sstream>
#include <vector>

class BCIconInfo: public BCTargetInfo
{
public:
    unsigned         _card;             // icon location - node card index
    bool            _ioboard;           // This is an io board
    std::vector<BCTargetInfo*> _nodes;  // nodes or link chips contained in this card
    virtual const char *cardName() = 0;
    void init_location()
    {
        if (midplaneNodeConfig() && !_ioboard)
        {
            std::ostringstream ostr;
            ostr << midplaneNodeConfig()->posInMachine()
                << "-" << cardName();
            _location = ostr.str();
        } else if (ioboardNodeConfig()) {  // Might be an io board
            std::ostringstream ostr;
            ostr << ioboardNodeConfig()->posInMachine()
                << "-" << cardName();
            _location = ostr.str();
        }

    }
public:
    BCIconInfo() :
        BCTargetInfo(),
        _card(0),
        _ioboard(false),
        _nodes()
    {

    }

    BCIconInfo(const std::string& loc) :
        BCTargetInfo(loc),
        _card(0),
        _ioboard(false),
        _nodes()
    {

    }

    bool nodesOpen()        // return true if any nodes contained in this card are open
    {
        for (std::vector<BCTargetInfo*>::iterator node = _nodes.begin(); node != _nodes.end(); ++node)
        {
        if ((*node)->_open)
            return true;
        }
        return false;
    }
    virtual ~BCIconInfo() {}
};

class BCNodecardInfo: public BCIconInfo
{
public:
    BCNodecardInfo() : BCIconInfo() {}
    BCNodecardInfo(const std::string& loc) : BCIconInfo(loc) {}
    void init_location(BGQIOBoardNodeConfig* nc = 0)
    {
        if (nc) {
            _location = nc->posInMachine();
        }
        else if (midplaneNodeConfig())
        {
            std::ostringstream ostr;
            ostr << midplaneNodeConfig()->posInMachine() << "-" << BGQTopology::nodeCardNameFromPos(_card);
            _location = ostr.str();
        } else if (ioboardNodeConfig()) {  // Might be an io board
            _location = ioboardNodeConfig()->posInMachine();
        }
    }
    const char *cardName() {
        if(midplaneNodeConfig()) return _location.substr(7,3).c_str();
        else if(ioboardNodeConfig()) return _location.substr(4,2).c_str();
        else return "00";
    }
    bool isIOcard() {
        if(midplaneNodeConfig()) return false;
        else if(ioboardNodeConfig()) return true;
        return false;
    }

    std::vector<BCLinkchipInfo*> _linkChips;
};

class BCServicecardInfo: public BCIconInfo
{
public:
    BCServicecardInfo(const std::string& loc) : BCIconInfo(loc) {}
    const char *cardName() { return "S"; }
};

class BCClockcardInfo: public BCIconInfo
{
public:
    BCClockcardInfo(const std::string& loc) : BCIconInfo(loc) {}
    std::string midplanePos() { return _location.substr(0,3); }
    const char *cardName() { return "K"; }
};

#endif
