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
/* (C) Copyright IBM Corp.  2011, 2012                              */
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
#include <boost/asio.hpp>

#include <iostream>

int main(int argc, char *argv[])
{
    if ( argc < 3 ) {
        std::cerr << argv[0] << " host port" << std::endl;
        exit( EXIT_FAILURE );
    }
    const char* host = argv[1];
    const char* service = argv[2];

    boost::asio::io_service io_service;
    boost::asio::ip::tcp::resolver resolver( io_service );

    size_t previous = 0;
    while ( true ) {
        boost::asio::ip::tcp::resolver::iterator i(
                resolver.resolve(
                    boost::asio::ip::tcp::resolver::query( host, service )
                    )
                );
        size_t count( 0 );
        while ( i != boost::asio::ip::tcp::resolver::iterator() ) {
            std::cout << i->endpoint() << std::endl;
            ++i;
            ++count;
        }

        std::cout << "got " << count << " addresses" << std::endl;
        if ( previous == 0 ) {
            previous = count;
        }
        assert( count == previous );

        sleep( 5 );
    }
}
