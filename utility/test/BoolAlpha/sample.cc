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
#include <utility/include/BoolAlpha.h>

#include <utility/include/Log.h>
#include <utility/include/Properties.h>

#include <boost/program_options.hpp>

#include <iostream>

using namespace bgq::utility;

int
main()
{
    namespace po = boost::program_options;

    bgq::utility::initializeLogging( *Properties::create() );

    BoolAlpha flag;

    po::options_description options;
    options.add_options()
        ( "some-bool-value", po::value(&flag)->implicit_value(true), "some flag" )
        ;

    return 0;
}

