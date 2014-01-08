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
#ifndef RUNJOB_CLIENT_STREAM_INPUT_H
#define RUNJOB_CLIENT_STREAM_INPUT_H

#include "client/Input.h"

#include <boost/enable_shared_from_this.hpp>

namespace runjob {
namespace client {

class MuxConnection;

/*!
 * \brief Handle input for stdin from a stream (tty or pipe)
 */
class StreamInput : public Input, public boost::enable_shared_from_this<StreamInput>
{
public:
    /*!
     * \brief ctor.
     */
    StreamInput(
            boost::asio::io_service& io_service,            //!< [in]
            const boost::weak_ptr<MuxConnection>& mux,      //!< [in]
            int fd                                          //!< [in]
            );

    /*!
     * \brief
     */
    void stop();

    /*!
     * \copdoc Input::read
     */
    void read(
            uint32_t rank,
            size_t length,
            const Uci& location
            );

private:
    void __attribute__ ((visibility("hidden"))) readHandler(
            uint32_t rank,
            uint32_t length,
            const Uci& location,
            const boost::system::error_code& error,
            size_t bytes_transferred
            );

private:
    boost::asio::posix::stream_descriptor _input;
    boost::asio::streambuf _buffer;
};

} // client
} // runjob

#endif

