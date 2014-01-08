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
#include <utility/include/Symlink.h>

#include <boost/system/system_error.hpp>

#include <iostream>

using namespace bgq::utility;

int
main()
{
    boost::filesystem::path path;
    path = path / "/proc" / "self" / "exe";
    try {
        Symlink link( path );
        std::cout << path << " links to " << std::string(link) << std::endl;
    } catch ( const boost::system::system_error& e ) {
        std::cerr << e.what() << std::endl;
    }

    return 0;
}

