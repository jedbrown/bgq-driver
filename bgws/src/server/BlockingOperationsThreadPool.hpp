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
/* (C) Copyright IBM Corp.  2012, 2012                              */
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

#ifndef BGWS_BLOCKING_OPERATIONS_THREAD_POOL_HPP_
#define BGWS_BLOCKING_OPERATIONS_THREAD_POOL_HPP_


#include <boost/asio.hpp>
#include <boost/function.hpp>
#include <boost/thread.hpp>


namespace bgws {


class BlockingOperationsThreadPool
{
public:

    BlockingOperationsThreadPool();

    void start( unsigned size );

    void post( boost::function<void( void )> fn );

    void shutdown();


    boost::asio::io_service& getIoService()  { return _io_service; }


private:

    boost::asio::io_service _io_service;
    boost::shared_ptr<boost::asio::io_service::work> _work_ptr;

    boost::thread_group _thread_group;


    void _runThread();

};


} // namespace bgws


#endif
