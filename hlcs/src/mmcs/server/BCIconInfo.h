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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef MMCS_SERVER_BC_ICON_INFO_H_
#define MMCS_SERVER_BC_ICON_INFO_H_

#include "BCTargetInfo.h"

#include <sstream>
#include <string>
#include <vector>

namespace mmcs {
namespace server {

class BCIconInfo : public BCTargetInfo
{
public:
    unsigned         _card;             // icon location - node card index
    bool            _ioboard;           // This is an io board
    std::vector<BCTargetInfo*> _nodes;  // nodes or link chips contained in this card
    virtual std::string cardName() = 0;
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
        for (std::vector<BCTargetInfo*>::iterator node = _nodes.begin(); node != _nodes.end(); ++node) {
            if ((*node)->_open)
                return true;
        }
        return false;
    }
    virtual ~BCIconInfo() {}
};

} } // namespace mmcs::server

#endif
