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

#include <utility/include/Log.h>
#include <utility/include/Properties.h>
#include <utility/include/UserId.h>

#include <utility/include/portConfiguration/SslConfiguration.h>

#include <boost/program_options.hpp>

#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <grp.h>
#include <pwd.h>
#include <unistd.h>

#include <sys/types.h>


using std::cerr;
using std::string;


int main( int argc, char* argv[] )
{
    namespace po = boost::program_options;

    try {

        bgq::utility::Properties::ProgramOptions properties_program_options;
        string username;


        po::options_description visible_options_desc( "Options" );

        properties_program_options.addTo( visible_options_desc );

        po::options_description all_options_desc( "Options" );

        all_options_desc.add( visible_options_desc );
        all_options_desc.add_options()
                ( "username", po::value( &username ), "Username" )
            ;

        po::positional_options_description p;
        p.add( "username", 1 );

        po::variables_map vm;
        po::store( po::command_line_parser( argc, argv ).options( all_options_desc ).positional( p ).run(), vm );
        po::notify( vm );

        if ( username == string() ) {
            cerr << argv[0] << ": No username provided\n";
            return 2;
        }

        bgq::utility::Properties::ConstPtr properties_ptr( bgq::utility::Properties::create( properties_program_options.getFilename() ) );

        bgq::utility::initializeLogging( *properties_ptr );

        bgq::utility::UserId user_id( username );

        if ( setgid( user_id.getGroups().front().first ) ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "setgid failed" ) );
        }

        std::vector<gid_t> groups;

        for ( bgq::utility::UserId::GroupList::const_iterator i(user_id.getGroups().begin()) ; i != user_id.getGroups().end() ; ++i ) {
            if ( i == user_id.getGroups().begin() )  continue; // skip the first one.
            groups.push_back( i->first );
        }

        if ( setgroups( groups.size(), groups.data() ) ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "setgroups failed" ) );
        }

        if ( setuid( user_id.getUid() ) ) {
            BOOST_THROW_EXCEPTION( std::runtime_error( "setuid failed" ) );
        }

        bgq::utility::SslConfiguration ssl_configuration( bgq::utility::SslConfiguration::Use::Client, bgq::utility::SslConfiguration::Certificate::Administrative, properties_ptr );

        const string &admin_private_key_filename(ssl_configuration.getMyPrivateKeyFilename());

        if ( euidaccess( admin_private_key_filename.c_str(), R_OK ) == 0 ) {
            // user has access.
            return 0;
        }

        // user doesn't have access.
        return 1;

    } catch ( std::exception& e ) {

        cerr << argv[0] << ": exception, " << e.what() << "\n";
        return 2;
    }

    return 2;
}
