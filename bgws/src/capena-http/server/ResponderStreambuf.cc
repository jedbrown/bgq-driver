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

#include "ResponderStreambuf.hpp"

#include "Connection.hpp"

#include <utility/include/Log.h>

#include <iosfwd>
#include <string>


using std::string;


LOG_DECLARE_FILE( "capena-http" );


namespace capena {
namespace server {


ResponderStreambuf::ResponderStreambuf(
        ConnectionPtr connection_ptr
    ) :
        _buffer(BUFFER_SIZE),
        _connection_ptr(connection_ptr)
{
    setp( _buffer.data(), _buffer.data() + (BUFFER_SIZE - 1) );
}


int ResponderStreambuf::_flushBuffer()
{
    int num(pptr()-pbase());

    if ( num > 0 ) {
        string output_str( _buffer.begin(), _buffer.begin() + num );

        _connection_ptr->postResponseBodyData(
                output_str,
                DataContinuesIndicator::EXPECT_MORE_DATA
            );

        pbump( -num );
    }

    return num;
}


ResponderStreambuf::int_type ResponderStreambuf::overflow( int_type c )
{
    if ( c != EOF ) {
        *pptr() = c;
        pbump( 1 );
    }

    if ( _flushBuffer() == EOF ) {
        return EOF;
    }

    return c;
}


int ResponderStreambuf::sync()
{
    if ( _flushBuffer() == EOF ) {
        return -1;
    }
    return 0;
}


} } // namespace capena::server
