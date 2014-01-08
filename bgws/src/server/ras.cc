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


#include "ras.hpp"

#include "UserInfo.hpp"

#include <db/include/api/BGQDBlib.h>

#include <ras/include/RasEventHandlerChain.h>
#include <ras/include/RasEventImpl.h>


namespace bgws {
namespace ras {


void postAdminAuthorityFailure(
        const UserInfo& user_info,
        const std::string& command
    )
{
    // Log a RAS event for user denied administrative access.

    static const uint32_t UserDeniedAdminAccessRasMessageId(0x000063001);
    static const std::string UserNameWhenAnonymous("nobody");

    RasEventImpl ras_event(
            UserDeniedAdminAccessRasMessageId
        );

    ras_event.setDetail( "USER", user_info.isAnonymous() ? UserNameWhenAnonymous : user_info.getUserId().getUser() );
    ras_event.setDetail( "COMMAND", std::string() + "BGWS server command: " + command );

    RasEventHandlerChain::handle( ras_event );

    BGQDB::putRAS( ras_event );
}


} } // namespace bgws::ras
