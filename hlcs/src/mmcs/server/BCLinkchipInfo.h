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

#ifndef MMCS_SERVER_BC_LINKCHIP_INFO_H_
#define MMCS_SERVER_BC_LINKCHIP_INFO_H_

#include "BCTargetInfo.h"

#include <control/include/mc/cardsDefs.h>

#include <ctrlnet/include/boot/LinkChipPersonality.h>

#include <boost/assert.hpp>

namespace mmcs {
namespace server {

class BCLinkchipInfo : public BCTargetInfo
{
public:
    unsigned _jtag; //!< jtag position
    bool _ioboard;  //!< This is an io board
    LinkChipPersonality _personality;
    BCLinkchipInfo() : BCTargetInfo(), _jtag(0), _ioboard(false) {}
    BCLinkchipInfo(const std::string& loc) : BCTargetInfo(loc), _jtag(0), _ioboard(false) {}
    std::string linkchipPos()
    {
        std::string result;
        if ( _ioboard) {
            result = IO_CARD_JTAGPORT_TO_LCTN[_jtag];
        } else {
            result = NODE_CARD_JTAGPORT_TO_LCTN[_jtag];
        }

        return result;
    }

    void init_location()
    {
        std::ostringstream ostr;
        if ( midplaneNodeConfig() && !_ioboard ) {
            ostr << midplaneNodeConfig()->posInMachine();
        } else if ( ioboardNodeConfig() ) {
            ostr << ioboardNodeConfig()->posInMachine();
        } else {
            BOOST_ASSERT( !"Not an I/O board or a midplane." );
        }

        ostr << "-" << linkchipPos();
        _location = ostr.str();
    }
};

} } // namespace mmcs::server

#endif
