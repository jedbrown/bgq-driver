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
#include <hlcs/include/runjob/commands/KillJob.h>

#include <boost/make_shared.hpp>

#include <cstring>
#include <sstream>

int
main()
{
    using namespace runjob::commands;

    // create request
    const Message::Ptr request( 
            boost::make_shared<request::KillJob>()
            );

    // serialize request
    std::ostringstream buf;
    request->serialize( buf );

    // create header
    Header header;
    memset( &header, 0, sizeof(header) );
    header._type = Message::Header::Request;
    header._tag = Message::Tag::ServerStatus;
    header._length = static_cast<uint32_t>(buf.str().size());

    // send header then message
    // ...
    
    return 0;
}
