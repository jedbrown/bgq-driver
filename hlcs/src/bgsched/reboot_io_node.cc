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


#include <bgsched/bgsched.h>

#include <bgsched/core/core.h>

#include <utility/include/Properties.h>

#include <boost/bind.hpp>
#include <boost/program_options.hpp>
#include <boost/thread.hpp>
#include <boost/throw_exception.hpp>

#include <iostream>
#include <string>


using std::string;


class RebootIoNode
{
public:

    RebootIoNode() : _reboot_complete(false) {}


    void run(
            const std::string& block_name,
            const std::string& location
        )
    {
        bgsched::IOBlock::rebootIONode(
                block_name,
                location,
                boost::bind( &RebootIoNode::_complete, this, _1 )
            );

        boost::unique_lock<boost::mutex> lock( _reboot_complete_mtx );
        while ( ! _reboot_complete ) {
            _reboot_complete_cond.wait( lock );
        }

        if ( _exc_ptr ) {
            boost::rethrow_exception( _exc_ptr );
        }
    }


private:

    bool _reboot_complete;
    boost::mutex _reboot_complete_mtx;
    boost::condition_variable _reboot_complete_cond;

    boost::exception_ptr _exc_ptr;


    void _complete(
            boost::exception_ptr exc_ptr
        )
    {
        _exc_ptr = exc_ptr;

        boost::unique_lock<boost::mutex> lock( _reboot_complete_mtx );
        _reboot_complete = true;
        _reboot_complete_cond.notify_all();
    }

};


int main(
        int argc,
        char *argv[]
   )
{
    namespace po = boost::program_options;

    try {

        po::options_description visible_desc( "Allowed options" );

        bgq::utility::Properties::ProgramOptions properties_program_options;
        properties_program_options.addTo( visible_desc );

        bool help_ind(false);

        visible_desc.add_options()
                ( "help,h", po::bool_switch( &help_ind ), "help" )
            ;


        po::options_description all_desc( "All options" );

        string block_name;
        string location;

        all_desc.add_options()
                ( "block", po::value( &block_name ), "I/O block name" )
                ( "location", po::value( &location ), "I/O node location" )
            ;

        all_desc.add( visible_desc );


        po::positional_options_description p;
        p.add( "block", 1 );
        p.add( "location", 1 );

        po::variables_map vm;
        po::store( po::command_line_parser( argc, argv ).options( all_desc ).positional( p ).run(), vm);
        po::notify( vm );

        if ( help_ind ) {

            std::cout << "Usage: " << argv[0] << " [OPTION]... ioBlock ioNodeLocation\n"
                         "Reboot an I/O node.\n"
                         "\n"
                      << visible_desc << "\n";

            exit( 0 );
        }

        if ( block_name.empty() )  {
            BOOST_THROW_EXCEPTION( std::runtime_error( "The I/O block name must be specified." ) );
        }
        if ( location.empty() ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "The I/O node location must be specified." ) );
        }

        bgq::utility::Properties::Ptr properties_ptr(bgq::utility::Properties::create( properties_program_options.getFilename() ));

        bgsched::init( properties_program_options.getFilename() );

        RebootIoNode reboot_io_node;

        reboot_io_node.run(
                block_name,
                location
            );

    } catch ( std::exception& e ) {

        std::cerr << argv[0] << ": An error occurred. The error text is " << e.what() << "\n";
        exit( 1 );

    }
}
