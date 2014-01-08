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
#ifndef RUNJOB_COMMANDS_CONVERT_H
#define RUNJOB_COMMANDS_CONVERT_H

#include "common/logging.h"

#include <hlcs/include/runjob/commands/Header.h>
#include <hlcs/include/runjob/commands/KillJob.h>
#include <hlcs/include/runjob/commands/RefreshConfig.h>
#include <hlcs/include/runjob/commands/Response.h>

#include "common/commands/ChangeCiosConfig.h"
#include "common/commands/DumpProctable.h"
#include "common/commands/EndTool.h"
#include "common/commands/JobAcl.h"
#include "common/commands/JobStatus.h"
#include "common/commands/LocateRank.h"
#include "common/commands/LogLevel.h"
#include "common/commands/MuxStatus.h"
#include "common/commands/ServerStatus.h"
#include "common/commands/ToolStatus.h"

#include <boost/archive/archive_exception.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/asio/streambuf.hpp>

#include <boost/assert.hpp>

namespace runjob {
namespace commands {

/*!
 * \brief convert a buffer into a Message based on its Header.
 * \ingroup command_protocol
 */
inline Message::Ptr
__attribute__ ((visibility("hidden"))) 
convertRequest(
        const runjob::commands::Header& header,
        boost::archive::text_iarchive& ar
       )
{
    Message::Ptr result;

    if ( header._tag == Message::Tag::ChangeCiosConfig ) {
        request::ChangeCiosConfig* derived( new request::ChangeCiosConfig );
        ar & *derived;
        result = request::ChangeCiosConfig::Ptr( derived );
    } else if ( header._tag == Message::Tag::DumpProctable ) {
        request::DumpProctable* derived( new request::DumpProctable );
        ar & *derived;
        result = request::DumpProctable::Ptr( derived );
    } else if ( header._tag == Message::Tag::EndTool ) {
        request::EndTool* derived( new request::EndTool );
        ar & *derived;
        result = request::EndTool::Ptr( derived );
    } else if ( header._tag == Message::Tag::JobAcl ) {
        request::JobAcl* derived( new request::JobAcl );
        ar & *derived;
        result = request::JobAcl::Ptr( derived );
    } else if ( header._tag == Message::Tag::JobStatus ) {
        request::JobStatus* derived( new request::JobStatus );
        ar & *derived;
        result = request::JobStatus::Ptr( derived );
    } else if ( header._tag == Message::Tag::KillJob ) {
        request::KillJob* derived( new request::KillJob );
        ar & *derived;
        result = request::KillJob::Ptr( derived );
    } else if ( header._tag == Message::Tag::LogLevel ) {
        request::LogLevel* derived( new request::LogLevel );
        ar & *derived;
        result = request::LogLevel::Ptr( derived );
    } else if ( header._tag == Message::Tag::LocateRank ) {
        request::LocateRank* derived( new request::LocateRank );
        ar & *derived;
        result = request::LocateRank::Ptr( derived );
    } else if ( header._tag == Message::Tag::MuxStatus ) {
        request::MuxStatus* derived( new request::MuxStatus );
        ar & *derived;
        result = request::MuxStatus::Ptr( derived );
    } else if ( header._tag == Message::Tag::RefreshConfig ) {
        request::RefreshConfig* derived( new request::RefreshConfig );
        ar & *derived;
        result = request::RefreshConfig::Ptr( derived );
    } else if ( header._tag == Message::Tag::ServerStatus ) {
        request::ServerStatus* derived( new request::ServerStatus );
        ar & *derived;
        result = request::ServerStatus::Ptr( derived );
    } else if ( header._tag == Message::Tag::ToolStatus ) {
        request::ToolStatus* derived( new request::ToolStatus );
        ar & *derived;
        result = request::ToolStatus::Ptr( derived );
    } else {
        BOOST_ASSERT( !"unhandled request tag" );
    }

    return result;
}

/*!
 * \brief
 */
inline Message::Ptr
__attribute__ ((visibility("hidden"))) 
convertResponse(
        const runjob::commands::Header& header,
        boost::archive::text_iarchive& ar
       )
{
    Message::Ptr result;

    if ( header._tag == Message::Tag::ChangeCiosConfig ) {
        response::ChangeCiosConfig* derived( new response::ChangeCiosConfig );
        ar & *derived;
        result = response::ChangeCiosConfig::Ptr( derived );
    } else if ( header._tag == Message::Tag::DumpProctable ) {
        response::DumpProctable* derived( new response::DumpProctable );
        ar & *derived;
        result = response::DumpProctable::Ptr( derived );
    } else if ( header._tag == Message::Tag::EndTool ) {
        response::EndTool* derived( new response::EndTool );
        ar & *derived;
        result = response::EndTool::Ptr( derived );
    } else if ( header._tag == Message::Tag::JobAcl ) {
        response::JobAcl* derived( new response::JobAcl );
        ar & *derived;
        result = response::JobAcl::Ptr( derived );
    } else if ( header._tag == Message::Tag::JobStatus ) {
        response::JobStatus* derived( new response::JobStatus );
        ar & *derived;
        result = response::JobStatus::Ptr(derived);
    } else if ( header._tag == Message::Tag::KillJob ) {
        response::KillJob* derived( new response::KillJob );
        ar & *derived;
        result = response::KillJob::Ptr(derived);
    } else if ( header._tag == Message::Tag::LogLevel ) {
        response::LogLevel* derived( new response::LogLevel );
        ar & *derived;
        result = response::LogLevel::Ptr(derived);
    } else if ( header._tag == Message::Tag::LocateRank ) {
        response::LocateRank* derived( new response::LocateRank );
        ar & *derived;
        result = response::LocateRank::Ptr(derived);
    } else if ( header._tag == Message::Tag::MuxStatus ) {
        response::MuxStatus* derived( new response::MuxStatus );
        ar & *derived;
        result = response::MuxStatus::Ptr( derived );
    } else if ( header._tag == Message::Tag::RefreshConfig ) {
        response::RefreshConfig* derived( new response::RefreshConfig );
        ar & *derived;
        result = response::RefreshConfig::Ptr( derived );
    } else if ( header._tag == Message::Tag::ServerStatus ) {
        response::ServerStatus* derived( new response::ServerStatus );
        ar & *derived;
        result = response::ServerStatus::Ptr( derived );
    } else if ( header._tag == Message::Tag::ToolStatus ) {
        response::ToolStatus* derived( new response::ToolStatus );
        ar & *derived;
        result = response::ToolStatus::Ptr( derived );
    } else {
        BOOST_ASSERT(!"unhandled response tag");
    }

    return result;
}

/*!
 * \brief
 */
inline Message::Ptr
convert(
        const runjob::commands::Header& header, //!< [in]
        boost::asio::streambuf& message         //!< [in]
        )
{
    try {
        std::istream is( &message );
        boost::archive::text_iarchive ar( is );
        if ( header._type == Message::Header::Request ) {
            return convertRequest( header, ar );
        } else if ( header._type == Message::Header::Response ) {
            return convertResponse( header, ar );
        } else {
            BOOST_ASSERT(!"unhandled message header type");
        }
    } catch ( const boost::archive::archive_exception& e ) {
        const log4cxx::LoggerPtr log_logger_ = runjob::getLogger();
        LOG_WARN_MSG( e.what() );
    }

    return Message::Ptr();
}

} // commands
} // runjob

#endif
