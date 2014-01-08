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

#include <bgsched/runjob/kill.h>
#include <bgsched/bgsched.h>
#include <bgsched/InputException.h>

#include <iostream>

#include <boost/lexical_cast.hpp>

int
main( int argc, char** argv )
{
    bgsched::init( std::string() );

    if ( argc < 3 ) {
        std::cerr << argv[0] << " job signal" << std::endl;
        exit( EXIT_FAILURE );
    }

    bgsched::Job::Id job;
    try {
        job = boost::lexical_cast<bgsched::Job::Id>( argv[1] );
    } catch ( const boost::bad_lexical_cast& e ) {
        std::cerr << "garbage job ID: " << argv[1] << std::endl;
        exit( EXIT_FAILURE );
    }

    int signal;
    try {
        signal = boost::lexical_cast<int>( argv[2] );
    } catch ( const boost::bad_lexical_cast& e ) {
        std::cerr << "garbage signal: " << argv[2] << std::endl;
        exit( EXIT_FAILURE );
    }

    try {
        const int rc = bgsched::runjob::kill( job, signal );
        exit( rc );
    } catch ( const bgsched::InputException& e ) {
        std::cerr << e.what() << std::endl;
    }
    exit( EXIT_FAILURE );
}
