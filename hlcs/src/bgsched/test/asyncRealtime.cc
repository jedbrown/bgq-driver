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

#include <iostream>

#include <bgsched/bgsched.h>

#include <bgsched/realtime/Client.h>

#include <utility/include/Log.h>
#include <utility/include/LoggingProgramOptions.h>
#include <utility/include/Properties.h>

#include <boost/asio.hpp>
#include <boost/bind.hpp>
#include <boost/program_options.hpp>

using std::cout;

class AsyncRealtime
{
public:

    AsyncRealtime()
        : _rt_desc( _io_service )
    {
        // Nothing to do.
    }

    void run()
    {
        _rt_client.connect();

        bool interrupted(false);

        _rt_client.requestUpdates( &interrupted );

        // cout << "requestUpdates interrupted: " << interrupted << "\n";

        _rt_client.setBlocking( false );

        int rt_fd( _rt_client.getPollDescriptor() );

        _rt_desc.assign( rt_fd );

        // cout << "doing async_read_some with null_buffers...\n";

        _rt_desc.async_read_some(
                boost::asio::null_buffers(),
                boost::bind( &AsyncRealtime::_readReady, this, boost::asio::placeholders::error )
            );

        _io_service.run();
    }

private:

    bgsched::realtime::Client _rt_client;

    boost::asio::io_service _io_service;
    boost::asio::posix::stream_descriptor _rt_desc;


    void _readReady(
            const boost::system::error_code& error
        )
    {
        if ( error ) {
            cout << "Error reading.\n";
            return;
        }

        do {
            // cout << "Notified read ready, err=" << error << "\n";

            bool interrupted(false), again(false), end(false);

            _rt_client.receiveMessages( &interrupted, &again, &end );

            // cout << "receiveMessages: int=" << interrupted << " again=" << again << " end=" << end << "\n";

            if ( end ) {
                return;
            }

            if ( interrupted ) {
                continue;
            }

            // cout << "doing async_read_some with null_buffers...\n";
            _rt_desc.async_read_some(
                    boost::asio::null_buffers(),
                    boost::bind( &AsyncRealtime::_readReady, this, boost::asio::placeholders::error )
                );

        } while ( false );
    }
};

int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    cout << "Working...\n";

    bgq::utility::LoggingProgramOptions logging_program_options( "ibm.bgsched.realtime" );

    po::options_description opts_desc;

    logging_program_options.addTo( opts_desc );

    po::variables_map vm;

    po::store( po::parse_command_line( argc, argv, opts_desc ), vm );

    po::notify( vm );

    bgq::utility::Properties properties;

    bgq::utility::initializeLogging( properties, logging_program_options );

    bgsched::init( "" );

    AsyncRealtime async_realtime;
    async_realtime.run();

    cout << "Done!\n";
}
