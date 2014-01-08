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

#ifndef MMCS_SERVER_BC_NODECARD_INFO_H_
#define MMCS_SERVER_BC_NODECARD_INFO_H_

#include "BCIconInfo.h"

#include "types.h"

namespace mmcs {
namespace server {

class BCNodecardInfo : public BCIconInfo
{
public:
    BCNodecardInfo() : BCIconInfo() {}
    BCNodecardInfo(const std::string& loc) : BCIconInfo(loc) {}
    void init_location(BGQIOBoardNodeConfig* nc = 0)
    {
        if (nc) {
            _location = nc->posInMachine();
        } else if (midplaneNodeConfig()) {
            std::ostringstream ostr;
            ostr << midplaneNodeConfig()->posInMachine() << "-" << BGQTopology::nodeCardNameFromPos(_card);
            _location = ostr.str();
        } else if (ioboardNodeConfig()) {  // Might be an io board
            _location = ioboardNodeConfig()->posInMachine();
        }
    }
    std::string cardName() {
        if (midplaneNodeConfig())
            return _location.substr(7,3);
        else if (ioboardNodeConfig())
            return _location.substr(4,2);
        else
            return "00";
    }
    bool isIOcard() {
        if (midplaneNodeConfig())
            return false;
        else if (ioboardNodeConfig())
            return true;
        return false;
    }

    std::vector<BCLinkchipInfo*> _linkChips;
};

} } // namespace mmcs::server

#endif
