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

#ifndef CAPENA_SERVER_RESPONDER_STREAMBUF_HPP_
#define CAPENA_SERVER_RESPONDER_STREAMBUF_HPP_


#include "fwd.hpp"

#include <streambuf>
#include <vector>


namespace capena {
namespace server {


/*! \brief streambuf that buffers output (the response body) from the application's Responder
 *
 *  When the internal buffer is full,
 *  ResponderStreambuf sends the data to the Connection for writing to the client.
 */
class ResponderStreambuf : public std::streambuf
{
public:

    static const int BUFFER_SIZE = 512;


    ResponderStreambuf(
            NotifyDataFn notify_data_fn
        );


protected:

    std::vector<char> _buffer;


    int _flushBuffer();

    // override
    virtual int_type overflow( int_type c );

    // override
    virtual int sync();


private:

    NotifyDataFn _notify_data_fn;
};


} } // namespace capena::server


#endif
