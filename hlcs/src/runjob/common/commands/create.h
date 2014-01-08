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
#ifndef RUNJOB_COMMON_COMMANDS_CREATE_H
#define RUNJOB_COMMON_COMMANDS_CREATE_H
/*!
 * \file runjob/common/commands/create.h
 * \brief runjob::commands::create free function.
 * \ingroup command_protocol
 */

#include <hlcs/include/runjob/commands/KillJob.h>
#include <hlcs/include/runjob/commands/Header.h>
#include <hlcs/include/runjob/commands/RefreshConfig.h>
#include <hlcs/include/runjob/commands/Response.h>

#include "common/commands/ChangeCiosConfig.h"
#include "common/commands/DumpProctable.h"
#include "common/commands/JobAcl.h"
#include "common/commands/JobStatus.h"
#include "common/commands/LogLevel.h"
#include "common/commands/MuxStatus.h"
#include "common/commands/ServerStatus.h"
#include "common/commands/ToolStatus.h"

#include <boost/assert.hpp>

namespace runjob {
namespace commands {

/*!
 * \brief create a Response message based on a Message::Tag type
 * \ingroup command_protocol
 */
inline Response::Ptr
create(
        runjob::commands::Message::Tag::Type tag    //!< [in]
      )
{
    Response::Ptr result;
    if ( tag == Message::Tag::JobAcl ) {
        result.reset( new response::JobAcl );
    } else if ( tag == Message::Tag::JobStatus ) {
        result.reset( new response::JobStatus );
    } else if ( tag == Message::Tag::KillJob ) {
        result.reset( new response::KillJob );
    } else if ( tag == Message::Tag::LogLevel ) {
        result.reset( new response::LogLevel );
    } else if ( tag == Message::Tag::MuxStatus ) {
        result.reset( new response::MuxStatus );
    } else if ( tag == Message::Tag::RefreshConfig ) {
        result.reset( new response::RefreshConfig );
    } else if ( tag == Message::Tag::ServerStatus ) {
        result.reset( new response::ServerStatus );
    } else if ( tag == Message::Tag::ToolStatus ) {
        result.reset( new response::ToolStatus );
    } else if ( tag == Message::Tag::ChangeCiosConfig ) {
        result.reset( new response::ChangeCiosConfig );
    } else if ( tag == Message::Tag::DumpProctable ) {
        result.reset( new response::DumpProctable );
    } else {
        BOOST_ASSERT(!"unhandled response tag");
    }

    return result;
}

} // commands
} // runjob

#endif
