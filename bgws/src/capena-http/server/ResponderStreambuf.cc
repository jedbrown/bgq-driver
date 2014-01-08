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

#include <string>


using std::string;


namespace capena {
namespace server {


ResponderStreambuf::ResponderStreambuf(
        NotifyDataFn notify_data_fn
    ) :
        _buffer(BUFFER_SIZE),
        _notify_data_fn(notify_data_fn)
{
    setp( _buffer.data(), _buffer.data() + (BUFFER_SIZE - 1) );
}


int ResponderStreambuf::_flushBuffer()
{
    int num(pptr()-pbase());

    if ( num > 0 ) {
        string output_str( _buffer.begin(), _buffer.begin() + num );

        _notify_data_fn(
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
