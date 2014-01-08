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
#include <utility/include/Properties.h>
#include <utility/include/SignalHandler.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/enable_shared_from_this.hpp>

#include <iostream>

#include <unistd.h>

class MyHandler : public bgq::utility::SignalHandler<SIGALRM>, public boost::enable_shared_from_this<MyHandler>
{
public:
    MyHandler(
            boost::asio::io_service& io_service
            ) :
        bgq::utility::SignalHandler<SIGALRM>( io_service )
    {

    }

    void start()
    {
        this->async_wait(
                boost::bind(
                    &MyHandler::handler,
                    shared_from_this(),
                    _1,
                    _2
                    )
                );
    }

private:
    void handler(
            const boost::system::error_code& error,
            const siginfo_t& siginfo
           )
    {
        if ( !error ) {
            std::cout << "received signal " << siginfo.si_signo << std::endl;
        }
    }
};

int
main()
{
    boost::asio::io_service io_service;
    boost::shared_ptr<MyHandler> foo(
            new MyHandler( io_service )
            );
    foo->start();

    // other setup for your application
    alarm(1);

    // run I/O service
    io_service.run();

    return 0;
}

