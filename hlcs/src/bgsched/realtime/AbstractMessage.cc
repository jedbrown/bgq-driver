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
#include "AbstractMessage.h"

#include <utility/include/Log.h>

#include <boost/algorithm/string.hpp>

#include <boost/archive/text_oarchive.hpp>
#include <boost/archive/text_iarchive.hpp>

#include <boost/serialization/shared_ptr.hpp>

#include <sstream>
#include <string>

using std::istringstream;
using std::ostringstream;
using std::string;

LOG_DECLARE_FILE( "bgsched" );

namespace bgsched {
namespace realtime {

std::string AbstractMessage::toString( ConstPtr ptr )
{
    ostringstream oss;

    {
        boost::archive::text_oarchive oa( oss );

        const AbstractMessage *p(ptr.get());
        oa << p;
    }

    string msg_str(oss.str());

    LOG_TRACE_MSG( "unescaped message= '''\n" << msg_str << "\n'''" );

    _escape( msg_str );

    return msg_str;
}


AbstractMessage::Ptr AbstractMessage::fromString( const std::string& s )
{
    string unescaped_msg_str(s);
    _unescape( unescaped_msg_str );

    LOG_TRACE_MSG( "unescaped message='''\n" << unescaped_msg_str << "\n'''" );

    istringstream iss( unescaped_msg_str );

    Ptr ptr;

    {
        boost::archive::text_iarchive ia( iss );

        AbstractMessage *p(NULL);
        ia >> p;

        ptr.reset( p );
    }

    return ptr;
}


void AbstractMessage::_escape( std::string& msg_in_out )
{
    boost::algorithm::replace_all( msg_in_out, "\\", "\\\\" );
    boost::algorithm::replace_all( msg_in_out, "\n", "\\n" );
}

void AbstractMessage::_unescape( std::string& msg_in_out )
{
    boost::algorithm::replace_all( msg_in_out, "\\n", "\n" );
    boost::algorithm::replace_all( msg_in_out, "\\\\", "\\" );
}


AbstractMessage::~AbstractMessage()
{
    // Nothing to do.
}


} } // namespace bgsched::realtime
