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

#ifndef BGWS_UTILITY_PIPE_HPP_
#define BGWS_UTILITY_PIPE_HPP_


#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>


namespace bgws {
namespace utility {


class Pipe : boost::noncopyable
{
public:

    typedef boost::shared_ptr<boost::asio::posix::stream_descriptor> SdPtr;


    Pipe(
            boost::asio::io_service &io_service
        );

    SdPtr& writeSdPtr()  { return _write_sd_ptr; }
    SdPtr& readSdPtr()  { return _read_sd_ptr; }


private:

    SdPtr _write_sd_ptr;
    SdPtr _read_sd_ptr;

};


} // namespace bgws::utility
} // namespace bgws

#endif
